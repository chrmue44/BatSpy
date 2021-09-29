/*********************************************************
 *         project: MemPool Library                      *
 *                                                       *
 *          Author: Christian Mueller                    *
 *                  Gutenbergstr 20                      *
 *                  D-64342 Seeheim-Jugenheim            *
 *                                                       *
 *********************************************************/

#include "clFixMemPool.h"
#include "clPool.h"
#include "cldebuglog.h"
#include <iostream>
#include <cstring>

clFixMemPool * clFixMemPool::m_pInstance = NULL;

void* operator new(std::size_t sz)
{
  return clFixMemPool::allocate(sz, "mpVector");
}

void operator delete(void* ptr) noexcept
{
    clFixMemPool::deallocate(ptr, "mpVector");
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Achtung!: innerhalb des Konstruktors darf KEINE Logging-Funktion aufgerufen
// werden, da diese sich bereits aus dem Memorypool bedienen!
clFixMemPool::clFixMemPool()
:
m_PoolTab(new(malloc(sizeof (tPoolTab))) tPoolTab),
m_PoolCount(0),
m_ChunkCount(0),
m_totalPoolSize(0),
m_totalAllocatedSize(0),
m_HeapMaxChunks(0),
m_HeapTab(new(malloc(sizeof (tHeapTab))) tHeapTab)
// hier gibts eine Lint-Meldung wegen m_MtxMemPool
{
#ifdef MULTITHREADING
  int32_t Err;
	Err = pthread_mutex_init(&m_MtxMemPool,NULL);
  PM_ASSERT(Err == 0, "clFixmempool: error init. mutex");
#endif
}

// Achtung!: innerhalb des Destruktors darf KEINE Logging-Funktion aufgerufen
// werden, dad diese sich bereits aus dem Memorypool bedienen!
//lint -e{1551} Function may throw exception in Destructor
clFixMemPool::~clFixMemPool()
{
  tPoolTabItr i;

  for(i=m_PoolTab->begin(); i<m_PoolTab->end(); i++)
  {
    m_totalPoolSize -= (*i)->getChunkSize()*(*i)->getChunkCount();
    (*i)->~clPool();
    free(*i);
  }
  m_PoolTab->~vector();
  free(m_PoolTab);
#ifdef MULTITHREADING
  int32_t Err;
  Err = pthread_mutex_destroy(&m_MtxMemPool);
  PM_ASSERT(Err == 0, "clFixmempool: error destroying mutex");
#endif
}

void* clFixMemPool::allocate_impl(size_t size, const char* ClassName, enAllocSrc s)
{
  clPool *pPool;
  void *p = NULL;
  tDbgLogSrc Src;

  if(s == ALLOC_STL)
    Src = DBGSRC_STLALLOC;
  else
    Src = DBGSRC_MEMFIXALLOC;
#ifdef MULTITHREADING
  int32_t Err;
	Err = pthread_mutex_lock(&m_MtxMemPool);
  PM_ASSERT(Err == 0, "clFixmempool: error locking mutex");
  PM_ASSERT(ClassName != NULL,"clFixmempool: class name must be defined");
 #endif 
  pPool = findBestFit(size);
  if(pPool != NULL)
  {
    p = pPool->allocate();
    if (p != NULL)
    {
      m_ChunkCount++;
      //lint -e{534,668} (Warning -- Ignoring return value of function
      if(clDebugLog::checkLogSource(Src))
      {
        char Buf[300];
        snprintf(Buf, sizeof(Buf),
                 "MEMPOOL: Chunks %u; alloc %s Addr %0x reqSize %u  chunkSize %u\n",
                 m_ChunkCount, ClassName, (unsigned long)p, size,pPool->getChunkSize());
        clDebugLog::add(Src,Buf);
      }
      m_totalAllocatedSize += pPool->getChunkSize();
    }
  }

  //  es ist kein freier Speicher fuer die geforderte Groesse mehr verfuegbar
  if(p == NULL)
  {
    stHeapChunk Chunk;
  //  try
    {
      p = malloc(sizeof(int8_t) * size);
      Chunk.Size = size;
      Chunk.pChunk = p;
      m_HeapTab->push_back(Chunk);
      if(m_HeapMaxChunks < m_HeapTab->size())
        m_HeapMaxChunks++;
    }
  /*  catch(const std::bad_alloc& xa)
    {
      m_HeapTab.pop_back();
      p = NULL;
      //lint -e774 Info -- Boolean within 'if' always evaluates to True
      PM_ASSERT(p != NULL,xa.what());
    }*/
    if(clDebugLog::checkLogSource(DBGSRC_MEMDYNALLOC))
    {
      //lint -e{534,668} (Warning -- Ignoring return value of function
      if(p != NULL)
      {
        char Buf[300];
        snprintf(Buf, sizeof(Buf),
                 "HEAP   : Chunks %ui; new %s; Addr %ui; reqSize %ui",
                  m_HeapTab->size(), ClassName, reinterpret_cast<unsigned long>(p), size);
        clDebugLog::add(DBGSRC_MEMDYNALLOC,Buf);
      }
      else
        clDebugLog::add(DBGSRC_MEMDYNALLOC,"NEW     : out of memory");
    }
  }
#ifdef MULTITHREADING  
  Err = pthread_mutex_unlock(&m_MtxMemPool);
  PM_ASSERT(Err == 0, "clFixmempool: error unlocking mutex");
#endif
  return p;
}

void clFixMemPool::deallocate_impl(void *p, const char *ClassName, enAllocSrc s)
{
  tPoolTabItr i;
  bool found = false;
  int32_t RetVal = 0;
  tDbgLogSrc Src;

  if(s == ALLOC_STL)
    Src = DBGSRC_STLALLOC;
  else
    Src = DBGSRC_MEMFIXALLOC;
#ifdef MULTITHREADING
	int32_t Err = pthread_mutex_lock(&m_MtxMemPool);
  PM_ASSERT(Err == 0, "clFixmempool::deallocate: error locking mutex");
#endif
  for(i = m_PoolTab->begin(); i < m_PoolTab->end(); i++)
  {
    found = (*i)->isInPool(p);
    if(found)
      break;
  }
  if(found)
  {
    RetVal = (*i)->deallocate(p);
    if(RetVal == 0)
    {
      m_ChunkCount--;
      m_totalAllocatedSize -= (*i)->getChunkSize();
      //lint -e{534} (Warning -- Ignoring return value of function
      if(clDebugLog::checkLogSource(Src))
      {
        char Buf[200];
        snprintf(Buf, sizeof(Buf),
        "MEMPOOL: Chunks %ul; free %s; Addr %ul; ChunkSize%ul ",
        m_ChunkCount, ClassName, reinterpret_cast<unsigned long>(p), (*i)->getChunkSize());
        clDebugLog::add(Src,Buf);
      }
    }
  }
  else
  {
    // delete NULL ist erlaubt, hat aber keine Auswirkung
    //lint -e{530,644} (Warning -- Symbol 'p' (line 152) not initialized)
    if(p != NULL)
    {
      tHeapTabIter Iter = findHeapChunk(p);
      if(Iter != m_HeapTab->end())
      {        
        free(p);
        //lint -e{534} ignoring return value
        m_HeapTab->erase(Iter);
        //lint -e{534} (Warning -- Ignoring return value of function
        if(clDebugLog::checkLogSource(DBGSRC_MEMDYNALLOC))
        {
          char Buf[200];
          snprintf(Buf, sizeof(Buf),
                 "HEAP   : Chunks %ul; delete %s; Addr %0x\n",
                 m_HeapTab->size(), ClassName, reinterpret_cast<unsigned long>(p));
          clDebugLog::add(DBGSRC_MEMDYNALLOC,Buf);
        }
      }
      else
      {
        if(clDebugLog::checkLogSource(DBGSRC_MEMFIXALLOC))
        {
          char Buf[200];
          snprintf(Buf, sizeof(Buf),
          "HEAP   : ERROR delete %s; Addr %0x\n",
          ClassName, reinterpret_cast<unsigned long>(p));
          clDebugLog::add(DBGSRC_MEMFIXALLOC,Buf);
        }
      }
    }
  }
  if(RetVal != 0)
  {
    clDebugLog::add(DBGSRC_MEMFIXALLOC,"error allocating memory");
  }
#ifdef MULTITHREADING
	Err = pthread_mutex_unlock(&m_MtxMemPool);
  PM_ASSERT(Err == 0, "clFixmempool::deallocate: error unlocking mutex");
#endif
}


int32_t clFixMemPool::addMemPool(uint32_t ChunkSize, uint32_t ChunkCnt)
{
  clPool *pPool;
  tPoolTabItr i;
  bool found = false;
  int32_t RetVal = 0;

  if(ChunkSize & CHUNKSIZE_MODULO)
  {
    //lint -e{506} (Warning -- Constant value Boolean)
    PM_ASSERT(false,"clFixmempool::addMemPool: ChunkSize is not modulo 4");
    ChunkSize += CHUNKSIZE_MODULO;
    ChunkSize &= ~CHUNKSIZE_MODULO;
  }
#ifdef MULTITHREADING  
  int32_t Err;
  Err = pthread_mutex_lock(&m_MtxMemPool);
  PM_ASSERT(Err == 0, "clFixmempool::addMemPool: error locking mutex");
#endif
/*  if (ChunkSize < sizeof(void*))
    RetVal = -2;  //@@@
  else  */
  {
  //  try
    {
      pPool = new(malloc(sizeof(clPool))) clPool(ChunkSize,ChunkCnt);
    }
  /*  catch (const std::bad_alloc &xa)
    {
      pPool = static_cast<clPool*>(NULL);
      PM_ASSERT(pPool != NULL,xa.what());
    }*/
    if (pPool != NULL)
    {
      for(i=m_PoolTab->begin(); i<m_PoolTab->end(); i++)
      {
        if ((*i)->getChunkSize() > ChunkSize)
        {
          //lint -e{534} (Warning -- Ignoring return value of function
          m_PoolTab->insert(i,pPool);
          found = true;
          break;
        }
      }
      if (!found)
        m_PoolTab->push_back(pPool);
      m_PoolCount++;
      m_totalPoolSize += pPool->getTotalPoolSize();
      RetVal = 0;
    }
    else
      RetVal = -1; //@@@

  }
#ifdef MULTITHREADING  
	Err = pthread_mutex_unlock(&m_MtxMemPool);
  PM_ASSERT(Err == 0, "clFixmempool::addMemPool: error unlocking mutex");
#endif
  //lint -e{429} (Warning -- Custodial pointer 'pPool' (line 236) has not been freed or returned)
  // ist hier ok, weil es im Destruktor erledigt wird
  return RetVal;
}

clPool* clFixMemPool::findBestFit(size_t size)
{
  tPoolTabItr i;
  clPool *RetVal = NULL;

	//if(m_PoolTab == NULL)
//		return NULL;
  for(i = m_PoolTab->begin(); i != m_PoolTab->end(); i++)
  {
    if ((*i)->getChunkSize() >= size)
    {
      RetVal = *i;
      break;
    }
  } 
  return RetVal;
}


void clFixMemPool::showPoolUsage()
{
  tPoolTabItr i;
  #ifdef MULTITHREADING
  Err = pthread_mutex_lock(&m_MtxMemPool);
  PM_ASSERT(Err == 0, "clFixmempool::showPoolUsage: error locking mutex");
  #endif
  std::cout << "FixMemPoolSize: " << m_totalAllocatedSize << std::endl;
  for(i = m_PoolTab->begin(); i < m_PoolTab->end(); i++)
  {
    std::cout << "Chunksize: " << (*i)->getChunkSize();
    std::cout << " allocated Blocks: " << (*i)->getAllocCount() << std::endl;
  }
  std::cout << "-------" << std::endl;

  #ifdef MULTITHREADING
	Err = pthread_mutex_unlock(&m_MtxMemPool);
  PM_ASSERT(Err == 0, "clFixmempool::showPoolUsage: error unlocking mutex");
  #endif
}


void clFixMemPool::showPoolStatistics()
{
  tPoolTabItr i;

  #ifdef MULTITHREADING
	Err = pthread_mutex_lock(&m_pInstance->m_MtxMemPool);
  PM_ASSERT(Err == 0, "clFixmempool::showPoolStatistics: error locking mutex");
  #endif
  std::cout << "------ Statistics Memory Pool -----------------" << std::endl;
  for(i = m_pInstance->m_PoolTab->begin(); i < m_pInstance->m_PoolTab->end(); i++)
  {
    std::cout << "Chunksize: " << (*i)->getChunkSize();
    float MaxBlocks = (float)(*i)->getAllocCountMax();
    std::cout << "  Blocks max.: " << MaxBlocks;
    float AvailableBlocks = (float)(*i)->getChunkCount();
    std::cout << "  available: " << AvailableBlocks;
    if(MaxBlocks >= AvailableBlocks*0.9)
      std::cout << "  WARNING";
    std::cout << std::endl;
  }
  if(m_pInstance->m_HeapMaxChunks > 0)
  {
    std::cout << "---- Statistics Heap ------------------------\n";
    std::cout << "  Blocks max.: " << m_pInstance->m_HeapMaxChunks << "\n";
  }
  std::cout << "-----------------------------------------------" << std::endl;
  #ifdef MULTITHREADING
  Err = pthread_mutex_unlock(&m_pInstance->m_MtxMemPool);
  PM_ASSERT(Err == 0, "clFixmempool::showPoolStatistics: error unlocking mutex");
  #endif
}

int32_t clFixMemPool::getAllocBlocks(size_t size)
{
  tPoolTabItr i;

  for(i = m_PoolTab->begin(); i < m_PoolTab->end(); i++)
  {
    if ((*i)->getChunkSize() == size)
    {
      return static_cast<int32_t>((*i)->getAllocCount());
    }
  } 
  return -1;
}

extern const tChunkTab memChunks[];
extern const size_t TabSize;
clFixMemPool* clFixMemPool::getInstance()
{
  if(m_pInstance == NULL)
  {
    m_pInstance = new(malloc(sizeof(clFixMemPool))) clFixMemPool;
  // Speicher einrichten
    uint32_t i;
//    PM_ASSERT(&Tab != NULL, "clFixMemPool: NULL pointer for chunk tab");
    uint32_t Size = 0;

    for(i = 0; i < TabSize; i++)
    //lint -e{613} Warning -- Possible use of null pointer 'Tab'
    {
      int32_t Err;
      PM_ASSERT(memChunks[i].Size > Size, "clFixMemPool: chunk table not ordered");
      Err = m_pInstance->addMemPool(memChunks[i].Size,memChunks[i].Count);
      PM_ASSERT(Err == 0, "clFixMemPool:: getInstance: error creating pool");
    }

  }
  return m_pInstance;
}

void clFixMemPool::destroyInstance()
{
  if(m_pInstance != NULL)
  {
    m_pInstance->~clFixMemPool();
    free(m_pInstance);
    m_pInstance = NULL;
  }
}


void clFixMemPool::showLeaks()
{
  tPoolTabItr i;

  clDebugLog::add(DBGSRC_MEMFIXALLOC, "Memory Leaks:");
  for(i = m_PoolTab->begin(); i < m_PoolTab->end(); i++)
  {
    if ((*i)->getAllocCount() != 0)
    {
      char buf[100];
      snprintf(buf, sizeof(buf), "ChunkSize: %ul;  Count: %ul",(*i)->getChunkSize(),(*i)->getAllocCount());
      clDebugLog::add(DBGSRC_MEMFIXALLOC, buf);
      (*i)->findAllocatedChunks();
    }
  } 
#ifdef WIN32
  while(!_kbhit());
#endif
}

size_t clFixMemPool::getAllocatePoolSize()
{
  tPoolTabItr i;
  size_t Size = 0;
  
  for(i = m_pInstance->m_PoolTab->begin(); i < m_pInstance->m_PoolTab->end(); i++)
  {
    Size += (*i)->getTotalPoolSize();
  }
  return Size;
}

tHeapTabIter clFixMemPool::findHeapChunk(const void* p)
{
  tHeapTabIter i;
  for(i = m_HeapTab->begin(); i != m_HeapTab->end(); i++)
  {
    if((*i).pChunk == p)
      return i;
  }
  return m_HeapTab->end();
}

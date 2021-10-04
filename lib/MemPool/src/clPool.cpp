/*********************************************************
 *         project: MemPool Library                      *
 *                                                       *
 *          Author: Christian Mueller                    *
 *                  Gutenbergstr 20                      *
 *                  D-64342 Seeheim-Jugenheim            *
 *                                                       *
 *********************************************************/

#include "clPool.h"
#include "cldebuglog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

clPool::clPool(size_t ChunkSize, uint32_t ChunkCount)
:
m_PoolSize(0),
m_AllocCount(0),
m_AllocCountMax(0),
m_ChunkSize(ChunkSize),
m_BrtChunkSize(0),
m_ChunkCnt(ChunkCount),
m_pBuf(NULL),
m_pBufEnd(NULL),
m_pNextFreeBlock(NULL)
{
  uint32_t i;
  tChunk *pN = NULL;  //Zeiger auf naechsten freien Block
  tChunk *pN2 = NULL; //Zeiger auf uebernaechsten freien Block
  int8_t *p;

  PM_ASSERT(ChunkCount != 0, "clPool::clPool(): minimum of 1 chunk required");
  PM_ASSERT(ChunkSize != 0, "clPool::clPool(): minimum  chunk size of 1 required");
  //Speicher auf dem Heap allokieren
  m_BrtChunkSize = m_ChunkSize + sizeof(tChunk) - MEM_ST_SIZE;
  m_PoolSize = m_BrtChunkSize * ChunkCount + sizeof(tChunk);
  m_pBuf = new int8_t[m_PoolSize];
  m_pBufEnd = m_pBuf + m_PoolSize - 1;

  //naechster freier Block ist zunaechst der erste
  m_pNextFreeBlock = reinterpret_cast<tChunk*>(m_pBuf);

  //verkettete Liste anlegen
  p = m_pBuf;
  for(i = 0; i < ChunkCount; i++)
  {
    pN = reinterpret_cast<tChunk*>(p); 
    pN->Index = i;
    p += ChunkSize + sizeof(tChunk) - MEM_ST_SIZE;
    pN->pNext = reinterpret_cast<tChunk*>(p);
    pN2 = reinterpret_cast<tChunk*>(p);
  }

  // der letzte Block hat keinen Nachfolger
  pN->pNext = static_cast<tChunk*>(NULL);
  // letzten Block nach oben begrenzen
  pN2->Index = ChunkCount;
}

clPool::~clPool()
{
  delete[] m_pBuf;
  m_pBufEnd = NULL;
  m_pNextFreeBlock = NULL;
}


void* clPool::allocate()
{
  void *p;
  tChunk *pC;

  if(m_AllocCount < m_ChunkCnt)
  {
    pC = m_pNextFreeBlock;
    p = &pC->mem[0];
    // der letzte Block zu allokieren?
    if((m_AllocCount + 1) < m_ChunkCnt)
    //noch nicht
    {  
      if(
          (reinterpret_cast<int8_t*>(pC->pNext) < m_pBuf) || 
          (reinterpret_cast<int8_t*>(pC->pNext) > m_pBufEnd)
        )
      {
        clDebugLog::add(DBGSRC_MEMFIXALLOC, "memory pointer courrupted");
        return NULL;
      }
      else
      {
        m_pNextFreeBlock = pC->pNext;
      }
    }
    else
    // der letzte Block
    {
      m_pNextFreeBlock = reinterpret_cast<tChunk*>(NULL);  //jetzt ist kein Block mehr frei!
    }
    pC->pNext = reinterpret_cast<tChunk*>(NULL);
    m_AllocCount++;
    if(m_AllocCountMax < m_AllocCount)
      m_AllocCountMax = m_AllocCount;
    if(static_cast<float>(m_AllocCount) >= 0.9 * m_ChunkCnt)
    {
      char Buf[80];
      snprintf(Buf, sizeof(Buf), "FixMemPool: Warning level Pool reached for ChunkSize %ul", m_ChunkSize);
      if(clDebugLog::checkLogSource(DBGSRC_MEMFIXALLOC))
        clDebugLog::add(DBGSRC_MEMFIXALLOC,Buf);
    }
#ifdef WIN32    
    if(!checkBorders(pC))
    {
      char Text[200];
      sprintf_s(Text,sizeof(Text), "alloc: %s Adr %p",
              clTextHandler::get(ERR_MEM_BOUNDS_EXCEEDED) ,&pC->mem[0]);
      clErrorHandler::add(ERRLVL_MEM, Text );
      clErrorHandler::add(ERRLVL_MEM, ERR_MEM_BOUNDS_EXCEEDED);
      repairBorders(pC);
    }
#endif
    return p;
  }
  else
  {
    char buf[100];
    snprintf(buf, sizeof(buf), "Warning no more block free for size %ul", m_ChunkSize);
    clDebugLog::add(DBGSRC_MEMFIXALLOC, buf);
  }
  return NULL;
}

int32_t clPool::deallocate(void *pFree)
{
  tChunk *pC;  //Zeiger auf Descriptor des zurueckzugebenden Blocks
  int32_t RetVal = 0;

  //lint -e{826} (Info -- Suspicious pointer-to-pointer conversion (area too small))
  pC = reinterpret_cast<tChunk*>
       ((reinterpret_cast<int8_t*>(pFree) - sizeof(tChunk)) + MEM_ST_SIZE);
#ifdef WIN32
  // pruefe, ob der freizugebende Block mit den richtigen Indexwerten eingerahmt ist
  if(!checkBorders(pC))
  {
    if(checkPointer(pC))
    {
      char Text[200];
      sprintf_s(Text,sizeof(Text), "dealloc: %s Adr %p", 
              clTextHandler::get(ERR_MEM_BOUNDS_EXCEEDED), &pC->mem[0]);
      clErrorHandler::add(ERRLVL_MEM, Text);
      clErrorHandler::add(ERRLVL_MEM, ERR_MEM_BOUNDS_EXCEEDED);
      repairBorders(pC);
    }
    else
    {
      clErrorHandler::add(ERRLVL_MEM, ERR_MEM_PTFREE);
      clDebugLog::add(DBGSRC_MEMFIXALLOC,ERR_MEM_PTFREE);
      return ERR_UTIL_DEALLOCATE;
    }
  }
#endif
  if((m_AllocCount > 0) && (pC->pNext == NULL))
  {
    m_AllocCount--;
    pC->pNext = m_pNextFreeBlock;
    m_pNextFreeBlock = pC;
  }
  else
  {
    clDebugLog::add(DBGSRC_MEMFIXALLOC, "memory freed twice");
    RetVal = 1;
  }
  return RetVal;
}


void clPool::findAllocatedChunks()
{
  uint32_t i;
  uint32_t *pArr = new uint32_t[m_ChunkCnt];
  tChunk *pC = m_pNextFreeBlock;
  
  PM_ASSERT(pArr != NULL,"clPool::findAllocatedBlock: error new");
  for(i = 0; i< m_ChunkCnt; i++)
    pArr[i] = 0;

  while(pC != NULL)
  {
    PM_ASSERT(reinterpret_cast<int8_t*>(pC) >= m_pBuf,"clPool::findAllocatedChunks: pointer error");
    i = static_cast<uint32_t>((reinterpret_cast<int8_t*>(pC) - m_pBuf)/static_cast<uint32_t>(m_BrtChunkSize));
    pArr[i] = 1;
    pC = pC->pNext;
  }

  for(i = 0; i < m_ChunkCnt; i++)
  {
    if(pArr[i] == 0)
    {
      char Buf[50];
      //lint -e{826} (Info -- Suspicious pointer-to-pointer conversion (area too small))
      tChunk *pC2 = reinterpret_cast<tChunk*>(m_pBuf + (i * m_BrtChunkSize));
      void *p = &pC2->mem[0];
      snprintf(Buf, sizeof(Buf), "Address: %0xl", (unsigned long)(p));
      clDebugLog::add(DBGSRC_MEMFIXALLOC,  Buf);
    }
  }
  delete[] pArr;
}

bool clPool::isInPool(const void *p) const
{
  return ((p >= m_pBuf) && (p <= m_pBufEnd));
}



bool clPool::checkBorders(tChunk *pC) const
{
  PM_ASSERT(reinterpret_cast<int8_t*>(pC) >= m_pBuf,"clPool::checkBorders: pointer error");
  tChunk *pCfollow = reinterpret_cast<tChunk*>(reinterpret_cast<int8_t*>(pC) + m_BrtChunkSize);
  uint32_t i = static_cast<uint32_t>
             (
               (reinterpret_cast<int8_t*>(pC) - m_pBuf) / 
                static_cast<uint32_t>(m_BrtChunkSize)
             );
  return ((pC->Index == i) && (pCfollow->Index == ( i + 1)));
}


bool clPool::checkPointer(const tChunk *pC) const
{
  PM_ASSERT(reinterpret_cast<const int8_t*>(pC) >= m_pBuf,"clPool::checkBorders: pointer error");
  uint32_t i = static_cast<uint32_t>
             (
               (reinterpret_cast<const int8_t*>(pC) - m_pBuf) /
                static_cast<uint32_t>(m_BrtChunkSize)
             );
  uint32_t j = static_cast<uint32_t>
             (
              (reinterpret_cast<const int8_t*>(pC) - m_pBuf) % 
               static_cast<uint32_t>(m_BrtChunkSize)
             );
  return((j == 0) && (i >= 0) && (static_cast<uint32_t>(i) < m_ChunkCnt));
}


void clPool::repairBorders(tChunk *pC) const
{
  PM_ASSERT(reinterpret_cast<int8_t*>(pC) >= m_pBuf,"clPool::checkBorders: pointer error");
  tChunk *pCfollow = reinterpret_cast<tChunk*>(reinterpret_cast<int8_t*>(pC) + m_BrtChunkSize);
  uint32_t i = static_cast<uint32_t>
             (
               (reinterpret_cast<int8_t*>(pC) - m_pBuf) /
                static_cast<uint32_t>(m_BrtChunkSize)
              );
  pC->Index = i;
  pCfollow->Index = i + 1;
}

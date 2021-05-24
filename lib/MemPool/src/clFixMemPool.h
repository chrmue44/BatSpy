#ifndef _CLFIXMEMPOOL_H_
#define _CLFIXMEMPOOL_H_
/*********************************************************
 *         project: MemPool Library                      *
 *                                                       *
 *          Author: Christian Mueller                    *
 *                  Gutenbergstr 20                      *
 *                  D-64342 Seeheim-Jugenheim            *
 *                                                       *
 *********************************************************/

#include <cstdint>
using namespace std;
//#define MULTITHREADING

#ifdef MULTITHREADING
#include "pthread.h"
#endif
#include <vector>
typedef std::size_t size_t;

#define CHUNKSIZE_MODULO  0x03L   // modulo 4

// Tabelleneintrag fuer Tabelle der Memory-Chunks
typedef struct
{
  uint32_t Size;
  uint32_t Count;
} tChunkTab;

struct stHeapChunk
{
  // Groesse des allokierten Speicherblocks
  size_t Size;
  // Zeiger auf Speicherblock
  void* pChunk;
};

enum enAllocSrc
{
  ALLOC_STL,
  ALLOC_OBJ
};

class clPool;

// Datentyp fuer die Tabelle der vorhandenen Memory-Pools
typedef std::vector<clPool*> tPoolTab;
typedef tPoolTab::iterator tPoolTabItr;

// Tabelle aller Chunks die auf dem Heap liegen
typedef std::vector<stHeapChunk> tHeapTab;

typedef tHeapTab::iterator tHeapTabIter;


// Realisierung einer einfachen Speicherverwaltung, die die Nachteile der
// Fragmentierung des Speichers bei Verwendung von new und delete umgehen soll:
// Der Applikation stehen eine feste Anzahl von Speicherbloecken in vorgegebenen
// Groessen zur Verfuegung.
// Ueber die Routine allocate() stellt die Klasse clFixMemPool Speicher fuer
// ein beliebiges Objekt zur Verfuegung. Die Methode liefert einen Zeiger auf
// das in der Groesse am besten passende Stueck Speicher aus dem Speicherpool.
// deallocate() gibt den Speicher wieder frei.
// Beim Aufruf von allocate() und deallocate() werden einige Pruefungen durch-
// gefuehrt und moegliche Fehler abgefangen:
// 1. mehrfaches delete auf ein und das selbe Objekt
// 2. delete auf einen Speicherbereich, der nicht zum Memory-Pool gehoert
// 3. delete auf einen Zeiger, der nicht auf den Anfang eines Memoryblocks zeigt
// 4. bei Ueberschreiten der verfuegbaren Speichermenge wird eine Warnmeldung
//    ausgegeben
// 5. In allocate() und deallocate() werden die Grenzen zum vorhergehenden und
//    nachfolgenden Block geprueft. Ist eine der Grenzen ueberschrieben worden,
//    wird eine Fehlermeldung ausgegeben und die Grenzen werden wiederhergestellt

class clFixMemPool  
{
	friend class clTestMemPool;
public:
  // Achtung!: innerhalb des Destruktors darf KEINE Logging-Funktion aufgerufen
  // werden, da diese sich bereits aus dem Memorypool bedienen!
	virtual ~clFixMemPool();

  //Liste aller allokierten Blocks fuer s�mtliche Pools ausgeben
  void showPoolUsage(void);

  // gibt eine Statistik der Speichernutzung auf der Konsole aus
  static void showPoolStatistics();

  //Speicher aus gesamtem Pool anfordern
  static inline void * allocate(
    // Groesse des zu allokierenden Speicherbereichs
    size_t size, 
    // Name des Objekttyps (dient ausschliesslich zu Debug-Zwecken)
    const char *ClassName,
    // Quelle der Allokierung 
    enAllocSrc LogSrc)
  {
    return m_pInstance->allocate_impl(size,ClassName,LogSrc);
  }

  //Speicher wieder freigeben
  static inline void deallocate(
    // Zeiger auf das zu deallokierende Objekt
    void* p,
    // Name des Objekttyps (dient ausschliesslich zu Debug-Zwecken)
    const char *ClassName,
    // Quelle der Allokierung 
    enAllocSrc LogSrc)
  {
    m_pInstance->deallocate_impl(p,ClassName, LogSrc);
  }

  // liefert den Zeiger auf die Instanz
  static clFixMemPool* getInstance(const tChunkTab* Tab, size_t TabSize);
  
  // zerstoert die Instanz, wenn sie vorhanden ist
  static void destroyInstance();

  // Liefert die Gesamtzahl der allokierten Bloecke aus dem statischen Bereich
  inline uint32_t getFixChunkCount()
  {
    return m_ChunkCount;
  }

  // Liefert die Gesamtzahl der allokierten Bloecke aus dem Heap
  inline size_t getHeapChunkCount()
  {
    return m_HeapTab.size();
  }
  
  // liefert die Brutto-Groesse des allokierten Memory-Pools
  static size_t getAllocatePoolSize();

  // zeige Memory-Leaks
  void showLeaks();


protected:
  // Achtung!: innerhalb des Konstruktors darf KEINE Logging-Funktion aufgerufen
  // werden, dad diese sich bereits aus dem Memorypool bedienen!
	clFixMemPool();
	clFixMemPool(const clFixMemPool&);
  clFixMemPool& operator= (const clFixMemPool&);

private:
  //weiteren Pool hinzufuegen
  int32_t addMemPool(uint32_t ChunkSize, uint32_t ChunkCnt);

  // sucht einen Chunk auf dem Heap
  // Wird der Chunk nicht gefunden liefert die Methode NULL zurueck, 
  // anderenfalls einen Iterator auf den Chunk
  tHeapTabIter findHeapChunk(const void* p);

  //zum Pool der Chunk-Groesse size die Anzahl der allokierten Blocks erfragen
  //return -1 bedeutet: zu dieser Groesse keinen Pool gefunden
  int32_t getAllocBlocks(size_t size);

  //finde den am besten passenden Sub-Pool
  clPool *findBestFit(size_t size);

  void* allocate_impl(size_t size, const char* ClassName, enAllocSrc Src);

  void deallocate_impl(void* p, const char* ClassName, enAllocSrc Src);

  //Liste der verfuegbaren Sub-Pools
  tPoolTab m_PoolTab;

  //Anzahl der angelegten Sub-Pools
  uint32_t m_PoolCount;
 
  // Anzahl der allokierten Speicherbloecke
  uint32_t m_ChunkCount;

  // gesamte Menge des zur Verfuegung stehenden Speichers
  size_t m_totalPoolSize;

  // gesamte Menge des allokierten Speichers
  size_t m_totalAllocatedSize;

  // zaehlt die Memory-Chunks, die ueber new aufgerufen werden
//  uint32_t m_HeapChunkCount;
  
  // speichert das Maximum an Chunks auf dem Heap
  uint32_t m_HeapMaxChunks;
#ifdef MULTITHREADING
  // zum verriegeln des Memory-Pools
  pthread_mutex_t m_MtxMemPool;
#endif
  // Tabelle aller Chunks auf dem heap
  tHeapTab m_HeapTab;

  // Zeiger auf die einzige Instanz
  static clFixMemPool *m_pInstance;
};


#endif // !defined(AFX_CLFIXMEMPOOL_H__9EDCE90A_A407_4D77_AE11_4737C6C0B3A8__INCLUDED_)


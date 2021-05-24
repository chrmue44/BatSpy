#ifndef _CLPOOL_H_
#define _CLPOOL_H_

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

#define MEM_ST_SIZE  4

struct tagChunk
{
  //fortlaufender Index
  uint32_t Index;
  
  //Zeiger auf naechsten freien Block
  struct tagChunk *pNext;

  //Anfang des Speichers
  uint8_t mem[MEM_ST_SIZE];
};

typedef struct tagChunk tChunk;

// Klasse zur Verwaltung eines Pools von Speicherbloecken gleicher Groesse
// 
// Jeder Speicherblock kann m_ChunkSize Bytes speichern. Der einzelne Block
// enthaelt Zusatzinformation, die in der Struktur tChunk am Anfang jedes
// Blocks gespeichert ist. In tChunk ist ein Eintrag Index, der fortlaufend
// durchnummeriert ist. Mithilfe dieser Information kann erkannt werden, ob
// ueber die Grenzen eines Blocks hinaus geschrieben wurde.
// Alle freien Speicherbloecke sind ueber eine verkettete Liste miteinander 
// verbunden.
// Beim Allokieren eines neuen Speicherblocks wird immer der erste Block aus
// der Liste zurueckgegeben und der Block aus der Liste entfernt.
// Beim Deallokieren wird der zurueckgegebene Speicherblock wieder an den
// Anfang der Liste eingetragen.

class clPool  
{
public:

  // Erzeugt den Pool in der gewuenschten Groesse
  clPool(size_t ChunkSize, uint32_t ChunkCount);

  virtual ~clPool();

  // prueft, ob die uebergebene Adresse innerhalb des Pools liegt
  bool isInPool(const void *p) const;
	
  // liefert die Bruttogroesse des Memorypools in Byte
  inline size_t getTotalPoolSize(void)
  {
    return m_PoolSize;
  }

  // Anzahl der allokierten Bloecke ausgeben
  inline uint32_t getAllocCount(void)
  {
    return m_AllocCount;
  }
  
  // Maximum der allokierten Bloecke anzeigen
  inline uint32_t getAllocCountMax(void)
  {
    return m_AllocCountMax;
  }

  // gibt die Groesse der Memorychunks des Pools aus
  inline size_t getChunkSize()
  {
    return m_ChunkSize;
  }

  // gibt die Anzahl der Memory-Chunks in diesem Pool aus
  inline uint32_t getChunkCount()
  {
    return m_ChunkCnt;
  }

  // gibt einen Speicherblock wieder frei
  // 
  // Zunaechst wird der uebergebene Zeiger geprueft. Liegt er ausserhalb
  // des Bereichs des Speicherpools oder nicht auf einer Anfangsadresse eines
  // Speicherblocks, wird ein Fehlercode zurueckgegeben.
  // Wenn der Speicherblock schon einmal zurueckgegeben wurde (er ist schon
  // in der Liste der freien Bloecke eingetragen) wird ebenfalls eine Fehler-
  // meldung zurueckgegeben.
  // Ist der uebergebene Zeiger i.O., wird der freie Speicherblock an den 
  // Anfang der Liste der freien Bloecke gestellt.
  int32_t deallocate(void*);

  // fordert einen Speicherblock der Groesse m_ChunkSize an
  //
  // Es wird der erste Block der FreeList zurueckgegeben, wenn noch freie
  // Bloecke vorhanden sind. Sind alle Speicherbloecke verbraucht, wird NULL
  // zurueckgegeben.
  void* allocate();

  // findet die momemtan allokierten Chunks und gibt die Adressen aus
  void findAllocatedChunks();

private:
  // stellt die Markierung der Grenzen eines Speicherbereichs wieder her
  void repairBorders(tChunk *pC) const;

  // prueft, ob der uebergebene Zeiger auf einen gueltigen Speicherblock zeigt
  bool checkPointer(const tChunk *pC) const;

  // Grenzen des Speicherblocks pruefen
  bool checkBorders(tChunk *pC) const;

	clPool();

  // Brutto-Groesse des MemoryPools in Byte
  size_t m_PoolSize;

  // Anzahl der momentan allokierten Chunks
  uint32_t m_AllocCount;

  // Anzahl der momentan allokierten Chunks
  uint32_t m_AllocCountMax;

  // Groesse der Memory-Chunks in Byte
  size_t m_ChunkSize;

  // Groesse der Memory-Chunks + Overheadinformation in Byte 
  size_t m_BrtChunkSize;

  // Anzahl der Memory-Chunks in diesem Pool
  uint32_t m_ChunkCnt;

  // Zeiger auf den Anfang des Pools
  int8_t *m_pBuf;

  //letzte Adresse im Speicher des Puffers
  int8_t* m_pBufEnd;

  // naechster freier Block
  tChunk *m_pNextFreeBlock;
};

#endif // !defined(AFX_CLPOOL_H__1DAE5C07_A921_4432_B058_64C137DD8C98__INCLUDED_)

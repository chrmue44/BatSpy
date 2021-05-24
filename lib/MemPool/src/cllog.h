#ifndef CLLOG_H_HEADER_
#define CLLOG_H_HEADER_
/*********************************************************
 *         project: MemPool Library                      *
 *                                                       *
 *          Author: Christian Mueller                    *
 *                  Gutenbergstr 20                      *
 *                  D-64342 Seeheim-Jugenheim            *
 *                                                       *
 *********************************************************/

#include "cSdCard.h"
#include "typedef.h"

enum enLogId
{
  LOGID_COMLOG = 5002,
  LOGID_ERRLOG = 5003,
  LOGID_DBGLOG = 5004
};
                                // tLogEntry.AddressId = uint64_t)
#define COMLOG_RECSIZE   80
#define MAXBITS_LOGSOURCES 100  // Bit tiefe zur Markierung der LogSourcen

#define MAX_LOG_ENTRIES 100000  //maximal Anzahl an Logeintraegen in ein File
                                //danach wird von vorne begonnen



// Basisklasse eines Loggers, die die grundlegenden Loggingmechanismen zur
// verfuegung stellt.
class clLog
{
public:
  // Log-Eintrag fuer Catman-Logformat
#pragma pack(2)
  struct tLogEntry
  {
    // Typ des Eintrags
    uint32_t Type;
    uint64_t AddressId;
    // Zeitstempel, relativ zum Start des Comlogs
    double TimeStamp;
    // Anzahl Bytes, die gesendet wurden
    uint32_t BytesSent;
    // Anzahl Bytes die empfangen wurden
    uint32_t BytesRecv;
    // Anzahl Bytes, die angefordert wurden
    uint32_t BytesReq;
    // Buffer fuer Log-Eintrag
    int8_t Buf[COMLOG_RECSIZE];
  };
  struct tLogHdr
  {
    uint32_t  ID;
    uint32_t  Offset;
    uint32_t  EntrySize;
    uint32_t  ComlogSize;
  };
#pragma pack()
  
  clLog();

  virtual ~clLog();

  // maximale Dateigroesse einstellen
  void setMaxFileSize(size_t Size);

  // Setzen des Log-Modus
  void setLogMode(tLogMode Mode)
  {
    m_LogMode = Mode;
  }
  
  // Einstellen, was passiert, wenn das Ende der Datei erreicht ist:
  // Ueberschreiben, stoppen
  void setLogFormat(tLogFormat Format)
  {
    m_LogFormat = Format;
  }
  
  // einstellen, ob mit oder ohne Zeit geloggt werden soll
  void enableTimeLogging(bool LogActive);
  
  void close();
  
  bool getIsOpened()
  {
    return m_IsOpen;
  }

  tLogMode getLogMode()
  {
    return m_LogMode;
  }

  tLogFormat getLogFormat()
  {
    return m_LogFormat;
  }

  size_t getMaxFileSize()
  {
    return m_MaxLogSize;
  }

  const char* getFileName()
  {
    return m_LogFileName;
  }

  void setFileName(const char* FileName)
  {
    strncpy(m_LogFileName, FileName, sizeof(m_LogFileName));
  }

protected:
  // Setzen des Ausgabemediums, wo die Log Texte ausgegeben werden.
  // Rueckgabe: 0=ok, <0:Fehler
  int32_t open_impl(
    // Ausgabemedium und Art (ASCII oder Binaer)
    tLogMode Mode, 
    // Ausgabeformat append oder wrap
    tLogFormat Format,
    // Dateiname 
    const char *FileName,
    // maximal erlaubte Groesse der Logdatei (bei Ueberschreiten der
    // hier angegebenen Groesse wird die Datei umgebrochen)
    uint32_t MaxSize,
    // Art des Logfiles
    enLogId LogId);
  
  // einen Logeintrag in den Log eintragen
  void addLogEntry(
    // Art des Log-Eintrages (vorerst nur relevant fuer PM-Log)
    uint32_t Type, 
    // String, der in das Log eingetragen werden soll
    const char* Str,
    // Geraetenummer
    uint64_t DeviceId);

  // einen Logeintrag in den Log eintragen
  void addLogEntry(
    // Art des Log-Eintrages (vorerst nur relevant fuer PM-Log)
    uint32_t Type, 
    // String, der in das Log eingetragen werden soll
    const char* Str,
    // Laenge des Eintrags
    size_t Len,
    // Geraetenummer
    uint64_t DeviceId);
  
  // Flag zum Kenzeichnung der Log Quellen
  uint32_t m_Log;
private:

  // prueft, ob bereits die maximale Laenge seit File-Anfang
  // eingetragen wurde und setzt ggf. den Schreibezeiger auf Datei-Start zurueck
  void checkMaxLen(size_t Len);

private:

  // gibt an, ob das Log geoeffnet ist
  bool m_IsOpen;
  // Ausgabemedium und Art (ASCII oder Binaer)
  tLogMode m_LogMode;
  // Ausgabeformat append oder wrap
  tLogFormat m_LogFormat;

  // Alle Log-Zeiten relativ zu LogTimeOffset
  double  m_LogTimeOffset;
  
  //true, wenn die Uhrzeit mitgeloggt werden soll
  bool m_LogTime;
#ifdef MULTITHREDING  
  // Dient zur synchronisierung der Ausgabe
  clMutex m_MtxWrite;
#endif
	// Name des Logfiles
  // hier kein tPM_string, weil Log-Files vor Mempool geoeffnet werden koennen
  char m_LogFileName[40];
  
  // momentane Groesse des Logs
  size_t m_LogSize;

  // maximal zulaessige Groesse des Logs
  size_t m_MaxLogSize;

  // maximal Offset des naechsten binaeren Log-Eintrags
  uint32_t m_ComlogOffset;

  // maximal Anzahl von binaeren Log-Eintraegen
  uint32_t m_ComlogSize;

  enLogId m_LogId;
  
  cSdCard& m_sd;

  tFILE m_file;
};



#endif /* CLLOG_H_HEADER_INCLUDED_BD6CADE4 */


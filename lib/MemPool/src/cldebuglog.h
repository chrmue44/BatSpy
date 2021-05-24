#ifndef _CLDEBUGLOG_H_
#define _CLDEBUGLOG_H_

/*********************************************************
 *         project: MemPool Library                      *
 *                                                       *
 *          Author: Christian Mueller                    *
 *                  Gutenbergstr 20                      *
 *                  D-64342 Seeheim-Jugenheim            *
 *                                                       *
 *********************************************************/

#include "cllog.h"

// Klasse, die fuer das Loggen von Debug-Messages zustaendig ist
// Als Singleton, kann von clDebugLog genau eine Instanz existieren
// Die eigentliche Loggingfunktionalitaet steckt in der Basisklasse clLog.
// Werden ueber setDebugSource() Sources enabled, so koennen diese nicht
// disabled werden. Das DebugSource wird ggf. in clGlobalFarmeObjs gesetzt.
// lint -e{1932} Note -- Base class 'clLog' is not abstract
class clDebugLog : public clLog
{
public:
  explicit clDebugLog();
  ~clDebugLog();

   // liefert einen Zeiger auf die (einzige) Instanz des Loggers
  static clDebugLog *getInstance();

  // zerstoert die Instanz, wenn sie vorhanden ist
  static void destroyInstance();

  void setDebugSource(uint32_t DebugSource)
  {
    m_DebugSource = DebugSource;
    enableLogSource(m_DebugSource);
  }

  uint32_t getDebugSource()
  {
    return m_DebugSource;
  }

  //lint -e{1511} Warning -- Member hides non-virtual member
  // in Ordnung, da die Methode der Basisklasse nicht static ist, wir
  // hier aber ein singleton haben
  static void close()
  {
    m_pInstance->clLog::close();
  }
  
  //lint -e{1511} Warning -- Member hides non-virtual member
  // in Ordnung, da die Methode der Basisklasse nicht static ist, wir
  // hier aber ein singleton haben
  static int32_t open(tLogMode Mode, tLogFormat Format, const char *FileName,
                   uint32_t MaxSize = 2000000)
  {
    return m_pInstance->clLog::open_impl(Mode,Format,FileName,MaxSize,LOGID_DBGLOG);
  }

  // Erzeugt einen Eintrag ins Logfile fuer eine Quelle.
  static void add(
    // Quelle, die in die LogDatei schreiben moechte.
    tDbgLogSrc LogSource, 
    // Die Message, die ins LogFile Einzutragen ist
    const char* Message);

  // dienst zur Ueberpruefung ob das Logging fuer die angegebene Quelle aktiviert
  // ist.
  //lint -e{1511} Warning -- Member hides non-virtual member
  // in Ordnung, da die Methode der Basisklasse nicht static ist, wir
  // hier aber ein singleton haben
  static bool checkLogSource(
    // Quelle, die in die LogDatei schreiben moechte.
    tDbgLogSrc LogSource)
  {
    if(m_pInstance == NULL) return(false);
    if(m_pInstance->m_Log & LogSource)
      return true;
    else
      return false;
  }

  // aktiviert das Logging von einer bestimmten Quelle.
  static void enableLogSource(uint32_t LogSource);

  // Deaktiviert das Logging von einer bestimmten Quelle.
  static void disableLogSource(tDbgLogSrc LogSource);

protected:
  clDebugLog(const clDebugLog&);
  clDebugLog& operator=(const clDebugLog&);


private:
  // Zeiger auf die (einzige vorhandene) Instanz des Loggers
  static clDebugLog *m_pInstance;
  
  // BitFeld, das angibt. welche LogSources nicht disabled werden koennen.
  // Hilfreich zum Debuggen von z.B. Speicherloechern
  // disableLogSource geht dann nicht
  uint32_t m_DebugSource;
};

#endif /* CLDEBUGLOG_H_HEADER_INCLUDED_BCFFF028 */

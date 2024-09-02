/*********************************************************
 *         project: MemPool Library                      *
 *                                                       *
 *          Author: Christian Mueller                    *
 *                  Gutenbergstr 20                      *
 *                  D-64342 Seeheim-Jugenheim            *
 *                                                       *
 *********************************************************/


#include "cllog.h"
//#include "clStreamUtils.h"
#include "cltimeutils.h"
//#include "PCK_Os/clSwap.h"

clLog::clLog()
:
m_Log(0),
m_IsOpen(false),
m_LogMode(LOG_ASCII),
m_LogFormat(LOG_WRAP),
m_LogTimeOffset(0),
#ifdef MULTITHREADING
m_MtxWrite(true, false),
#endif
m_LogTime(true),
m_LogSize(0),
m_MaxLogSize(0),
m_ComlogSize(0),
m_sd(cSdCard::inst())
{
}


clLog::~clLog()
{
  m_sd.closeFile(m_file);
}

void clLog::setMaxFileSize(size_t Size)
{
  m_MaxLogSize = Size;
}

void clLog::checkMaxLen(size_t Len)
{
  if((m_LogMode == LOG_ASCII) && (m_LogFormat == LOG_WRAP))
  {
    if(m_LogSize + Len > m_MaxLogSize)
    {
      if(m_IsOpen)
        m_sd.setFilePos(m_file, 0);
      m_LogSize = 0;
    }
  }
}

void clLog::addLogEntry(uint32_t Type, const char* Str, uint64_t DeviceId)
{
  size_t Len = (Str != NULL) ? strlen(Str):0;

  clLog::addLogEntry(Type, Str, Len, DeviceId);

}

void clLog::addLogEntry(uint32_t Type, const char* Str, size_t Len, uint64_t DeviceId)
{
  if(!m_IsOpen)
    return;
#ifdef MULTITHREADING    
	m_MtxWrite.lock("clLog::addLogEntry();");
#endif
  if( (m_LogMode == COUT) || (m_LogMode == LOG_ASCII) )
  {
    if (m_IsOpen && (Str != nullptr))
    {
      char TimeStr[40];
      char buf[200];
      checkMaxLen(Len);
      // Uhrzeit von gerade jetzt holen
      if(m_LogTime)
      {
        clTimeUtils::getTimeString(false, TimeStr, sizeof(TimeStr));
      }
      snprintf(buf, sizeof(buf), "%s: %s", TimeStr, Str);
      m_LogSize += strlen(buf);
    }
  }
/*  else if( m_LogMode == LOG_BINAER )
  {
    uint32_t    i;
    tLogHdr   LogHeader;
    tLogEntry LogEntry;
    double    TimeStamp;

    LogEntry.Type = clSwap::swap(Type);
    LogEntry.AddressId = clSwap::swap(DeviceId);
    err = clTimeUtils::getSysTime(TimeStamp);
    LogEntry.TimeStamp = clSwap::swap(TimeStamp - m_LogTimeOffset);
    //LogEntry.BytesSent = clSwap::swap(ComLogBytesSent);
    //LogEntry.BytesRecv = clSwap::swap(ComLogBytesReceived);
    //LogEntry.BytesReq  = clSwap::swap(ComLogBytesRequested);
    LogEntry.BytesSent = clSwap::swap(Len);                  // ???
//=========================================================================
    i = 0;
    do
    {
      uint32_t DataLen = ((Len-i)<COMLOG_RECSIZE)? (Len-i):COMLOG_RECSIZE;
      memset(LogEntry.Buf,0,COMLOG_RECSIZE);
      memcpy(LogEntry.Buf,Str+i,DataLen); 
      m_Stream->write((char*)&LogEntry, sizeof(tLogEntry));
      if( m_ComlogOffset > m_ComlogSize) m_ComlogSize = m_ComlogOffset;
      m_ComlogOffset++;
      if( (m_LogFormat == LOG_WRAP) && (m_ComlogOffset >= m_MaxLogSize/sizeof(tLogEntry)) )
      {
        m_ComlogOffset = 1;
        m_Stream->seekp(sizeof(tLogEntry),std::ios::beg);
      }
      LogEntry.TimeStamp = clSwap::swap(static_cast<double>(-1));
      i += DataLen;
    }while(i<Len);

    // Header aktualisieren
    // --------------------------------------------------------------------
    LogHeader.ID = clSwap::swap(static_cast<uint32_t>(m_LogId));
    LogHeader.Offset = m_ComlogOffset - 1;
    if( LogHeader.Offset < 1 ) LogHeader.Offset = m_MaxLogSize/sizeof(tLogEntry) - 1;
    clSwap::swap(&LogHeader.Offset);
    LogHeader.EntrySize = clSwap::swap(static_cast<uint32_t>(sizeof(tLogEntry)));
    LogHeader.ComlogSize = clSwap::swap(m_ComlogSize);
    m_Stream->seekp(0,std::ios::beg);
    m_Stream->write((char*)&LogHeader, sizeof(LogHeader));
    m_Stream->seekp(m_ComlogOffset * sizeof(tLogEntry),std::ios::beg);

//=========================================================================
 
  }
	*/
#ifdef MULTITHREADING
	m_MtxWrite.unlock("clLog::addLogEntry();");
#endif
}


/*void clLog::addLogEntry(uint32_t Type, std::istream* pStream, uint64_t DeviceId)
{
  char *pStr;
  uint32_t Len;
  
  if(pStream != NULL)
  {
    int32_t OldPos = pStream->tellg();
  
    Len = clStreamUtils::getStreamSize(pStream,0);
    // hier wird ausnahmsweise new benutzt, da sonst das loggen der
    // Speicherverwaltung nicht funktionieren wuerde
    pStr = new char[Len+1];
    pStream->read(pStr, static_cast<int32_t>(Len));

    checkMaxLen(clStreamUtils::getStreamSize(pStream));

    if( (m_LogMode == COUT) || (m_LogMode == LOG_ASCII) )
    {
      //lint -e{534} Ignoring return value of function 
      if ((m_Stream != NULL))
      {
        char TimeStr[80];
				m_MtxWrite.lock("clLog::addLogEntry();");
        // Uhrzeit von gerade jetzt holen
        if(m_LogTime)
        {
          clTimeUtils::getTimeString(false,TimeStr);
          *m_Stream << TimeStr;
          m_LogSize += strlen(TimeStr);
        }
        *m_Stream << ": "; 
        m_Stream->write(pStr,static_cast<int32_t>(Len));
        *m_Stream << std::endl;
        // Laenge von ": " und std::endl
        m_LogSize += 4 + Len;
				m_MtxWrite.unlock("clLog::addLogEntry();");
      
      }
    }
    else if( m_LogMode == LOG_BINAER )
    {
      addLogEntry(Type,pStr,Len,DeviceId);
    }
    delete[] pStr;
    pStream->seekg(OldPos, std::ios_base::beg);
  }
}
*/

void clLog::enableTimeLogging(bool LogActive)
{
  m_LogTime = LogActive;
}


int32_t clLog::open_impl(tLogMode Mode, tLogFormat Format, const char *pFileName, uint32_t MaxSize, enLogId LogId)
{
  int32_t RetVal = 0;

  if(m_IsOpen) 
    return 0;
#ifdef MULTITHREADING	
	m_MtxWrite.lock("clLog::open();");
#endif
  if(RetVal == 0)
  {

    m_LogMode = Mode;
    m_LogFormat = Format;
    setFileName(pFileName);
    m_LogId = LogId;

    if((pFileName == NULL) || (*pFileName == 0))
    {
      m_IsOpen = false;
      RetVal = 1;
    }

    if(RetVal == 0)
    {
      if(Mode == COUT)
      {
        m_IsOpen = true;
      }
      else
      {
        //lint -e{655} (Warning -- bit-wise operation uses (compatible) enum's)
        enSdRes ret = m_sd.openFile(pFileName, m_file, enMode::WRITE);
        if (ret == enSdRes::OK)
        {
          m_IsOpen = true;
          m_MaxLogSize = MaxSize;
        }
        else
        {
          m_IsOpen = false;
          RetVal = 2;
        }
      }
    }
  }
#ifdef MULTITHREADING  
	m_MtxWrite.unlock("clLog::open();");
#endif

  return RetVal;
}


void clLog::close()
{
  int32_t err = 0;

//  geht leider nicht wg. Debuglog, muss anders gemacht werden
//  so gibts eine Exception beim runterfahren
//  clErrorHandler::removeFromStatus(ERRSTATLVL_LOGOPEN,ERR_SYS_OPENLOGFILE,this);

  if( !m_IsOpen ) 
    return;
#ifdef MULTITHREADING	
	m_MtxWrite.lock("clLog::close();");
#endif
  if (m_IsOpen)
  {
    m_sd.closeFile(m_file);
    PM_ASSERT(err == 0,"clLog::close error locking mutex");
  }
  m_IsOpen = false;
#ifdef MULTITHREADING	
	m_MtxWrite.unlock("clLog::close();");
#endif
}

/*********************************************************
 *         project: ProcessMon                           *
 *                                                       *
 *              (C) 2010 Christian Mueller               *
 *                       Odenwaldstrasse 134g            *
 *                       D-64372 Ober-Ramstadt           *
 *                                                       *
 *********************************************************/

#include "cldebuglog.h"
//#include "Util/clTextHandler.h"

clDebugLog* clDebugLog::m_pInstance = NULL;


clDebugLog::clDebugLog():
clLog(),
m_DebugSource(0)
{
  if(m_pInstance == NULL)
  {
    m_pInstance = this;
  }
  else
    //lint -e{506,774} Warning -- Constant value Boolean
    PM_ASSERT(0,"DebugLogger instanciated twice");
}

clDebugLog::~clDebugLog()
{
}

clDebugLog *clDebugLog::getInstance()
{
  if(m_pInstance == NULL)
    m_pInstance = new clDebugLog();

  return m_pInstance;  
}

void clDebugLog::destroyInstance()
{
  if(m_pInstance != NULL)
  {
    delete m_pInstance;
    m_pInstance = NULL;
  }
}

void clDebugLog::add(tDbgLogSrc LogSource, const char* Message)
{
  if(checkLogSource(LogSource))
    m_pInstance->addLogEntry(LogSource + LOGVIEWER_DBGSRC_OFFS,Message,0);
}

void clDebugLog::enableLogSource(uint32_t LogSource)
{
  m_pInstance->m_Log |= LogSource;
}

void clDebugLog::disableLogSource(tDbgLogSrc LogSource)
{
  // es koennen nur Sources abgeschaltet werden, die nicht gesperrt sind
  if((m_pInstance->m_DebugSource & LogSource) == 0)
    m_pInstance->m_Log &= ~LogSource;
}



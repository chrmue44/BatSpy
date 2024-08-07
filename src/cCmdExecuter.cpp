/*************************************************************
 * BatSpy: Teensy 4.0 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * The command executer handles all commands that have to be *
 * delayed until a recording is finished                     *
 * ***********************************************************/

#include "cCmdExecuter.h"
#include "globals.h"
#include "pnlparams.h"


void cmdLog(void* pData)
{
  const char* pString = (const char*)pData;
  sysLog.log(pString);
}

void cmdUpdateInfos(void* pData)
{
  size_t freeSpace;  size_t totSpace;
  cSdCard::inst().getFreeMem(freeSpace, totSpace);
  devStatus.freeSpace.set(freeSpace / 1024/1024);
  if(devPars.recAuto.get() == enRecAuto::TWILIGHT)
    calcSunrise();
  logStatus();
  checkSupplyVoltage();
}

void cmdMeasTemperature(void* pData)
{
  float humidity;
  float temp = readTemperature(humidity);
  if((temp > 100) && (hasDisplay() == enDisplayType::OLED_128))
  if(isBackLightOn())
  {
    initDisplay(devPars.dispOrient.get(), 255,  devPars.displayMode.get(), false);
    sysLog.log("reinitialized display due to temp sensor error");
  }
  devStatus.temperature.set(temp);
  devStatus.humidity.set(humidity);
}


void cmdCheckAndSetTime(void* pData)
{
  rtc.checkAndSetTime(gps.getYear(), gps.getMonth(), gps.getDay(),
                      gps.getHour(), gps.getMinute(), gps.getSec(),
                     (enDlSaving)devPars.daylightSav.get());

}


void cmdBacklightOff(void* pData)
{
  setBackLight(false);
}


void cmdBacklightOn(void* pData)
{
  setBackLight(true);
  menue.resetTimer();
}

void cmdCloseProject(void* pData)
{
  audio.closeProject();
}

void cmdPowerOff(void* pData)
{
  powerOff();
}

const stCmdTabEntry cmdTab[] =
{
	{ enCmd::LOG,                &cmdLog },
  { enCmd::UPDATE_INFOS,       &cmdUpdateInfos },
  { enCmd::MEAS_TEMPERATURE,   &cmdMeasTemperature },
  { enCmd::CHECK_AND_SET_TIME, &cmdCheckAndSetTime },
  { enCmd::BACKLIGHT_OFF,      &cmdBacklightOff },
  { enCmd::BACKLIGHT_ON,       &cmdBacklightOn },
  { enCmd::CLOSE_PROJECT,      &cmdCloseProject },
  { enCmd::POWER_OFF,          &cmdPowerOff },
};

cCmdExecuter::cCmdExecuter()
{
  memset(m_queue, 0, sizeof(m_queue));
}

void cCmdExecuter::incQueuePtr(stQueueEntry*& ptr)
{
  if ((ptr < (&m_queue[0] + CMD_QUEUE_LEN - 1)) || (ptr < &m_queue[0]))
    ptr++;
  else
	ptr = &m_queue[0];
}

void cCmdExecuter::addToQueue(enCmd cmd, void* pData)
{
  if(m_queueFull)
    return;

  m_pWrite->cmd = cmd;
  m_pWrite->pData = pData;
  incQueuePtr(m_pWrite);
  if(m_pRead == m_pWrite)
	m_queueFull = true;
}

stQueueEntry* cCmdExecuter::getCmd()
{
  if(m_pWrite == m_pRead)
    return nullptr;

  stQueueEntry* retVal = m_pRead;
  incQueuePtr(m_pRead);
  m_queueFull = false;
  return retVal;
}

void cCmdExecuter::execCmd()
{
  if(isAvailable())
  {
	  stQueueEntry* pCmd = getCmd();
    for(const stCmdTabEntry* p = &cmdTab[0];  p < &cmdTab[sizeof(cmdTab)/sizeof(cmdTab[0])]; p++)
	  {
	    if(p->cmd == pCmd->cmd)
	    {
		    (p->pFunc)(pCmd->pData);
	    }
	  }
  }
}


#include "types.h"
#include "cCmdExecuter.h"
#include "globals.h"
#include "pnlmain.h"
#include "debug.h"

static enMainState mainState = enMainState::INIT;



void handleIdleState()
{
  enRecStatus recOn = audio.isRecordingActive();
  if(year() < 2025)
  {
    recOn = enRecStatus::REC_OFF;
    devStatus.recStatus.set("!!!");
  }
  else if (recOn == enRecStatus::REC_DISK_FULL)
  {
    devStatus.recStatus.set(Txt::get(113));
  }
  else
  {
    if (devStatus.playStatus.get() == static_cast<uint32_t>(enPlayStatus::STOP))
    {
      if (recOn != enRecStatus::REC_OFF)
        audio.setRecStatus(enPlayStatus::STOP);
      else
        devStatus.recStatus.set("\xF1");
    }
  }

  if (menue.keyPauseLongEnough(devPars.backLightTime.get() * 1000))
  {
    if(isBackLightOn())
	    command.addToQueue(enCmd::BACKLIGHT_OFF);
  }

  audio.checkAutoRecording(recOn);
  setHwOperationMode(recOn ? enCpuMode::RECORDING : enCpuMode::POWER_SAVE);

  if(audio.isRecording())
    mainState = enMainState::RECORD;
  else
    mainState = enMainState::COMMAND;
}

void stateMachine()
{
  switch(mainState)
  {
	  default:
	  case enMainState::INIT:
	    mainState = enMainState::IDLE;
	    break;

	  case enMainState::IDLE:
        handleIdleState();
	    break;

	  case enMainState::RECORD:
	    if(!audio.isRecording())
	      mainState = enMainState::COMMAND;
	    break;

	  case enMainState::COMMAND:
	    if (command.isAvailable())
		    command.execCmd();
	    if(!command.isAvailable())
	      mainState = enMainState::IDLE;
  	  break;
  }
}
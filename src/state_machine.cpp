#include "types.h"
#include "cCmdExecuter.h"
#include "globals.h"
#include "pnlmain.h"

static enMainState mainState = enMainState::INIT;



void handleIdleState()
{
  bool rtFft;
  if(hasDisplay() == enDisplayType::TFT_320)
    rtFft = (menue.getFocusPanel() == pnlLive) ||
              ((menue.getMainPanel() == pnlLive) && (menue.getFocusPanel() == menue.getFkeyPanel()));
  else
    rtFft = terminal.isOnline();
  audio.operate( rtFft );

  bool recOn = audio.isRecordingActive();
  if(year() < 2024)
  {
    recOn = false;
    devStatus.recStatus.set("!!!");
  }
  else
  {
    if(!audio.isRecording())
      devStatus.recStatus.set(recOn ? "\xF2" : "\xF1");
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
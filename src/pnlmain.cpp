/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include <Arduino.h>
#include "Adafruit_ILI9341.h"
#include "cutils.h"
#include "debug.h"
#include "cfileinfo.h"
#include "config.h"
#define OWN_H
#include "pnlmain.h"
#undef OWN_H
#include "pnllive.h"
#include "globals.h"
#include "pnlparams.h"




void setHeaderPanelText(cMenuesystem* pThis, thText t)
{
  hdrMainPanel = pThis->getHdrPanel();
  pThis->getPan(hdrMainPanel)->clear();
  pThis->getPan(hdrMainPanel)->addTextItem(t, 5, 1, pThis->getWidth() - 5, pThis->getHdrHeight() - 3);
}


// *******************************************
// drop down panel F1 for main panel

const char* MSG_PWR_DN_MENUE = "power down via menue";

void MEMP powerOffFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  switch (key)
  {
    case enKey::YES:
	  command.addToQueue(enCmd::LOG, (void*)MSG_PWR_DN_MENUE);
	  command.addToQueue(enCmd::POWER_OFF);
      break;

    default:
    case enKey::NO:
      break;
  }
}

void MEMP f1DropFuncExpert(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  switch (pThis->getFocusItem()) {
    case 0:
      pThis->setMainPanel(panGeo);
      pThis->setHdrPanel(hdrMainPanel);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
    case 1:
      devStatus.grafLive.initPlot(true);
      setSampleRateLivePan();
      pThis->setMainPanel(pnlLive);
      pThis->setHdrPanel(hdrPanWaterfall);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
    case 2:
      pThis->setMainPanel(panFileBrowser);
      pThis->setHdrPanel(hdrPanWaterfall);
      pThis->setFkeyPanel(fkeyFilePan);
     break;
    case 3:
      devStatus.graph.initPlot(true);
      pThis->setMainPanel(panTime);
      pThis->setHdrPanel(hdrPanWaterfall);;
      pThis->setFkeyPanel(fkeyWaterPan);
      break;
    case 4:
      devStatus.waterf.initPlot(true);
      pThis->setMainPanel(panWaterfall);
      pThis->setHdrPanel(hdrPanWaterfall);
      pThis->setFkeyPanel(fkeyWaterPan);
      break;
    case 5:
      pThis->setMainPanel(panInfo);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
    case 6:
      pThis->setMainPanel(panBats);
      pThis->setHdrPanel(hdrBatInfo);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
    case 7:
      pThis->setMainPanel(panFont);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
    case 8:
      pThis->showMsg(enMsg::YESNO, powerOffFunc, hasDisplay() == enDisplayType::OLED_128, Txt::get(1010));
      break;
  }
}

void MEMP f1DropFuncHandheld(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  switch (pThis->getFocusItem()) {
  case 0:
    pThis->setMainPanel(panGeo);
    pThis->setHdrPanel(hdrMainPanel);
    pThis->setFkeyPanel(fkeyMainPan);
    break;
  case 1:
    devStatus.grafLive.initPlot(true);
    setSampleRateLivePan();
    pThis->setMainPanel(pnlLive);
    pThis->setHdrPanel(hdrPanWaterfall);
    pThis->setFkeyPanel(fkeyMainPan);
    break;
  case 2:
    pThis->setMainPanel(panFileBrowser);
    pThis->setHdrPanel(hdrPanWaterfall);
    pThis->setFkeyPanel(fkeyFilePan);
    break;
  case 3:
    devStatus.graph.initPlot(true);
    pThis->setMainPanel(panTime);
    pThis->setHdrPanel(hdrPanWaterfall);;
    pThis->setFkeyPanel(fkeyWaterPan);
    break;
  case 4:
    devStatus.waterf.initPlot(true);
    pThis->setMainPanel(panWaterfall);
    pThis->setHdrPanel(hdrPanWaterfall);
    pThis->setFkeyPanel(fkeyWaterPan);
    break;
  case 5:
    pThis->setMainPanel(panInfo);
    pThis->setFkeyPanel(fkeyMainPan);
    break;
  case 6:
    pThis->setMainPanel(panBats);
    pThis->setHdrPanel(hdrBatInfo);
    pThis->setFkeyPanel(fkeyMainPan);
    break;
  case 7:
    pThis->showMsg(enMsg::YESNO, powerOffFunc, hasDisplay() == enDisplayType::OLED_128, Txt::get(1010));
    break;
  }
}

void MEMP f1DropFuncCompact(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  switch (pThis->getFocusItem()) {
  case 0:
    pThis->setMainPanel(panGeo);
    setHeaderPanelText(pThis, 100);
    pThis->setHdrPanel(hdrMainPanel);
    pThis->setFkeyPanel(fkeyMainPan);
    break;

  case 1:
    setHeaderPanelText(pThis, 105);
    pThis->setHdrPanel(hdrMainPanel);
    pThis->setMainPanel(panInfo);
    pThis->setFkeyPanel(fkeyMainPan);
    break;
  case 2:
#ifndef SIMU_DISPLAY
    while (digitalRead(PIN_ROT_LEFT_S) == 0);
#endif
    showSplashScreen(*gpDisplay, true);
    pThis->refreshAll();
    break;
  }
}

void MEMP f1DropFuncRecorder(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  switch (pThis->getFocusItem()) {
  case 0:
    pThis->setMainPanel(panGeo);
    setHeaderPanelText(pThis, 100);
    pThis->setHdrPanel(hdrMainPanel);
    pThis->setFkeyPanel(fkeyMainPan);
    break;
  case 1:
    devStatus.grafLive.initPlot(true);
    setSampleRateLivePan();
    pThis->setMainPanel(pnlLive);
    setHeaderPanelText(pThis, 101);
    pThis->setHdrPanel(hdrMainPanel);
    pThis->setFkeyPanel(fkeyMainPan);
    break;
  case 2:
    pThis->setMainPanel(panFileBrowser);
    setHeaderPanelText(pThis, 102);
    pThis->setHdrPanel(hdrMainPanel);
    pThis->setFkeyPanel(fkeyFilePan);
    break;
  case 3:
    setHeaderPanelText(pThis, 105);
    pThis->setMainPanel(panInfo);
    pThis->setHdrPanel(hdrMainPanel);
    pThis->setFkeyPanel(fkeyMainPan);
    break;
  case 4:
    pThis->showMsg(enMsg::YESNO, powerOffFunc, hasDisplay() == enDisplayType::OLED_128, Txt::get(1010));
    break;
  }
}


void MEMP initFunctionItemsExpert()
{
  f1MainItems.addItem(100);
  f1MainItems.addItem(101);
  f1MainItems.addItem(102);
  f1MainItems.addItem(103);
  f1MainItems.addItem(104);
  f1MainItems.addItem(105);
  f1MainItems.addItem(106);
  f1MainItems.addItem(107);
  f1MainItems.addItem(108);

  f4MainItems.addItem(1000);
  f4MainItems.addItem(1001);
  f4MainItems.addItem(1021);
  f4MainItems.addItem(1030);
  f4MainItems.addItem(1034);
  f4MainItems.addItem(1031);
  f4MainItems.addItem(1033);
  f4MainItems.addItem(1035);
}

void MEMP initFunctionItemsHandheld()
{
  f1MainItems.addItem(100);
  f1MainItems.addItem(101);
  f1MainItems.addItem(102);
  f1MainItems.addItem(103);
  f1MainItems.addItem(104);
  f1MainItems.addItem(105);
  f1MainItems.addItem(106);
  f1MainItems.addItem(108);

  f4MainItems.addItem(1000);
  f4MainItems.addItem(1001);
  f4MainItems.addItem(1021);
  f4MainItems.addItem(1030);
  f4MainItems.addItem(1034);
  f4MainItems.addItem(1031);
  f4MainItems.addItem(1033);
  f4MainItems.addItem(1035);
}

void MEMP initFunctionItemsRecorder()
{
  f1MainItems.addItem(100);
  f1MainItems.addItem(101);
  f1MainItems.addItem(102);
  f1MainItems.addItem(105);
  f1MainItems.addItem(108);

  f4MainItems.addItem(1000);
  f4MainItems.addItem(1001);
  f4MainItems.addItem(1021);
  f4MainItems.addItem(1030);
  f4MainItems.addItem(1034);
  f4MainItems.addItem(1031);
  f4MainItems.addItem(1033);
  f4MainItems.addItem(1035);
}

void MEMP initFunctionsCompact()
{
  f1MainItems.clear();
  f1MainItems.addItem(100);
  f1MainItems.addItem(105);
  f1MainItems.addItem(1000);

  f4MainItems.clear();
  f4MainItems.addItem(1030);
  f4MainItems.addItem(1034);
  f4MainItems.addItem(1360);
  f4MainItems.addItem(1031);
  f4MainItems.addItem(1035);
  f4MainItems.addItem(1001);
  f4MainItems.addItem(1021);
}

void MEMP f1Func(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  stPanelItem item;
  item.type = ITEM_ENUM;
  item.p = &f1MainItems;
  item.f = nullptr;
  switch (devPars.menueType.get())
  {
  case enMenueType::EXPERT:
    pThis->createDropDown(item, 1, DISP_HEIGHT_TFT - f1MainItems.size() * LINE_HEIGHT_TFT - 15, 120, f1DropFuncExpert);
    break;
  case enMenueType::RECORDER:
    pThis->createDropDown(item, 1, DISP_HEIGHT_TFT - f1MainItems.size() * LINE_HEIGHT_TFT - 15, 120, f1DropFuncRecorder);
    break;
  case enMenueType::HANDHELD:
    pThis->createDropDown(item, 1, DISP_HEIGHT_TFT - f1MainItems.size() * LINE_HEIGHT_TFT - 15, 120, f1DropFuncHandheld);
    break;
  case enMenueType::COMPACT:
    pThis->createDropDown(item, 1, DISP_HEIGHT_OLED - f1MainItems.size() * LINE_HEIGHT_OLED - 13, 90, f1DropFuncCompact);
    break;
  }
}

void MEMP f2Func(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  if ((devStatus.opMode.get() == enOpMode::HEAR_HET) ||
      (devStatus.opMode.get() == enOpMode::HEAR_DIRECT))
  {
    if(devPars.recAuto.get() == enRecAuto::ON)
    {
      if (devStatus.playStatus.get() == static_cast<uint32_t>(enPlayStatus::STOP))
        devStatus.playStatus.set(static_cast<uint32_t>(enPlayStatus::REC));
      else
        devStatus.playStatus.set(static_cast<uint32_t>(enPlayStatus::STOP));
    }
    else 
      pThis->showMsg(enMsg::INFO, nullptr, Txt::get(1015), Txt::get(1016));
  }
  else
  {
    if (devStatus.playStatus.get() == static_cast<uint32_t>(enPlayStatus::STOP))
      devStatus.playStatus.set(static_cast<uint32_t>(enPlayStatus::PLAY));
    else
      devStatus.playStatus.set(static_cast<uint32_t>(enPlayStatus::STOP));
  }
  pThis->refreshMainPanel();
}

void MEMP f4LoadFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  switch (key)
  {
    case YES:
      pThis->load();
      break;

    case enKey::NO:
    default:
      break;
  }
}

void MEMP f4SaveFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  switch (key)
  {
    case enKey::YES:
      pThis->save();
      break;
    
    case enKey::NO:
    default:
      break;
  }
}

void MEMP f4FactSetFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  switch (key)
  {
  case enKey::YES:
    pThis->setFactoryDefaults();
    break;

  case enKey::NO:
  default:
    break;
  }
}

void MEMP f4DropFunc(cMenuesystem* pThis, enKey key, cParBase* pItem) 
{
  switch (pThis->getFocusItem()) 
  {
    case 0:
#ifndef SIMU_DISPLAY
      while(digitalRead(PIN_ROT_LEFT_S) == 0);
#endif
      showSplashScreen(*gpDisplay, true);
      pThis->refreshAll();
      break;

    case 1:
      pThis->showMsg(enMsg::YESNO, f4LoadFunc, Txt::get(1005), Txt::get(1006));
      break;
    
    case 2:
      pThis->showMsg(enMsg::YESNO, f4SaveFunc, Txt::get(1005), Txt::get(1025));
      break;
    
    case 3:
      pThis->setMainPanel(panParams);
      setHeaderPanelText(pThis, 1030);
      pThis->setHdrPanel(hdrMainPanel);
      pThis->setFkeyPanel(fkeyMainPan);
      break;

    case 4:
      pThis->setMainPanel(panParRec);
      setHeaderPanelText(pThis, 1034);
      pThis->setHdrPanel(hdrMainPanel);
      pThis->setFkeyPanel(fkeyMainPan);
      break;

    case 5:
      devStatus.year.set((float)devStatus.date.getYear());
      devStatus.month.set((float)devStatus.date.getMonth());
      devStatus.day.set((float)devStatus.date.getDay());
      devStatus.hour.set((float)devStatus.time.getHour());
      devStatus.minute.set((float)devStatus.time.getMin());
      pThis->setMainPanel(panDateTime);
      setHeaderPanelText(pThis, 1031);
      pThis->setHdrPanel(hdrMainPanel);
      pThis->setFkeyPanel(fkeyMainPan);
      break;

    case 6:
      devStatus.latDeg.set((float)devStatus.geoPos.getDegLat());
      devStatus.latMin.set((float)devStatus.geoPos.getMinLat());
      devStatus.latSec.set((float)devStatus.geoPos.getSecLat());
      if(devStatus.latDeg.get() > 0)
        devStatus.latSign.set(0);
      else
        devStatus.latSign.set(1);
      devStatus.lonDeg.set((float)devStatus.geoPos.getDegLon());
      devStatus.lonMin.set((float)devStatus.geoPos.getMinLon());
      devStatus.lonSec.set((float)devStatus.geoPos.getSecLon());
      if(devStatus.lonDeg.get() > 0)
        devStatus.lonSign.set(0);
      else
        devStatus.lonSign.set(1);
      pThis->setMainPanel(panPosition);
      setHeaderPanelText(pThis, 1033);
      pThis->setHdrPanel(hdrMainPanel);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
    
    case 7:
      pThis->showMsg(enMsg::YESNO, f4FactSetFunc, Txt::get(1004), Txt::get(1006));

  }
}


void MEMP f4DropFuncCompact(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  switch (pThis->getFocusItem())
  {
    case 0:
      pThis->setMainPanel(panParams);
      setHeaderPanelText(pThis, 1030);
      pThis->setHdrPanel(hdrMainPanel);
      pThis->setFkeyPanel(fkeyMainPan);
      break;

    case 1:
      pThis->setMainPanel(panParRec);
      setHeaderPanelText(pThis, 1034);
      pThis->setHdrPanel(hdrMainPanel);
      pThis->setFkeyPanel(fkeyMainPan);
      break;

    case 2:
      pThis->setMainPanel(panParTrig);
      setHeaderPanelText(pThis, 1360);
      pThis->setHdrPanel(hdrMainPanel);
      pThis->setFkeyPanel(fkeyMainPan);
      break;

    case 3:
      devStatus.year.set((float)devStatus.date.getYear());
      devStatus.month.set((float)devStatus.date.getMonth());
      devStatus.day.set((float)devStatus.date.getDay());
      devStatus.hour.set((float)devStatus.time.getHour());
      devStatus.minute.set((float)devStatus.time.getMin());
      pThis->setMainPanel(panDateTime);
      setHeaderPanelText(pThis, 1031);
      pThis->setHdrPanel(hdrMainPanel);
      pThis->setFkeyPanel(fkeyMainPan);
      break;

    case 4:
      pThis->showMsg(enMsg::YESNO, f4FactSetFunc, true, Txt::get(1007), Txt::get(1009), Txt::get(1006));
      break;

    case 5:
      pThis->showMsg(enMsg::YESNO, f4LoadFunc, true, Txt::get(1007), Txt::get(1008), Txt::get(1006));
      break;

    case 6:
      pThis->showMsg(enMsg::YESNO, f4SaveFunc, true, Txt::get(1007), Txt::get(1008), Txt::get(1025));
      break;
  }
}

void MEMP f3Func(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  stPanelItem item;
  item.type = ITEM_ENUM;
  if(gps.isLogOpen())
    gps.closeLog();
  else
    gps.openLog();
  pThis->refreshMainPanel();
  setGpsLog(pThis, gps.isLogOpen());
}

void MEMP f4Func(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  stPanelItem item;
  item.type = ITEM_ENUM;
  item.p = &f4MainItems;
  switch ((enMenueType)devPars.menueType.get())
  {
    case enMenueType::EXPERT:
    case enMenueType::HANDHELD:
    case enMenueType::RECORDER:
      pThis->createDropDown(item, DISP_WIDTH_TFT - 120 - 1, DISP_HEIGHT_TFT - f4MainItems.size() * LINE_HEIGHT_TFT - 15, 120, f4DropFunc);
      break;
    case enMenueType::COMPACT:
      pThis->createDropDown(item, DISP_WIDTH_OLED - 110, DISP_HEIGHT_OLED - f4MainItems.size() * LINE_HEIGHT_OLED - 13, 107, f4DropFuncCompact);
      break;
  }
}


int MEMP initFkeyPanel(cPanel* pan, tCoord lf)
{
  int retVal;
  // Serial.println("initDialogs2");
 
  retVal = pan->addTextItem(1, 0, 227, 80, lf, true, f1Func);
  retVal |= pan->addTextItem(2,  81, 227, 79, lf, true, f2Func);
  if(devPars.srcPosition.get() != static_cast<uint32_t>(enPositionMode::FIX))
    retVal |= pan->addTextItem(6, 161, 227, 79, lf, true, f3Func);
  retVal |= pan->addTextItem(4, 241, 227, 79, lf, true, f4Func);
  return retVal;
}

int MEMP initCompactFkeyPanel(cPanel* pan, tCoord lf)
{
  int retVal;

  retVal =  pan->addTextItem(7,  1, DISP_HEIGHT_OLED - lf , 63, lf, true, f1Func);
  retVal |= pan->addTextItem(4, 65, DISP_HEIGHT_OLED - lf , 63, lf, true, f4Func);
  return retVal;
}

int visRecCntIndex = 0;

void MEMP setVisibilityRecCount(cMenuesystem* pThis)
{
  if (hasDisplay() == enDisplayType::TFT_320)
  {
    thPanel i = pThis->getMainPanel();
    cPanel* p = pThis->getPan(i);
    if (devPars.recAuto.get() > 0)
      p->itemList[visRecCntIndex].isVisible = true;
    else
      p->itemList[visRecCntIndex].isVisible = false;
  }
}

const char* PROGMEM MSG_REC_MODE_ON = "set recording mode ON";
const char* PROGMEM MSG_REC_MODE_OFF = "set recording mode OFF";

void MEMP dispModeFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{

  setVisibilityRecCount(pThis);
  calcSunrise();
  if (devPars.recAuto.get() == enRecAuto::ON)
  {
    audio.openProject();
    enableEditTimes(pThis, false);
    if (devPars.checkDebugLevel(DBG_COMMANDS))
      command.addToQueue(enCmd::LOG, (void*)MSG_REC_MODE_ON);
  }
  else if (devPars.recAuto.get() == enRecAuto::OFF)
  {
    audio.closeProject();
    enableEditTimes(pThis, false);
    if (devPars.checkDebugLevel(DBG_COMMANDS))
      command.addToQueue(enCmd::LOG, (void*)MSG_REC_MODE_OFF);
  }
  else
    enableEditTimes(pThis, true);
}




void MEMP setFileToDisplay(const char* buf)
{
  char infoFile[FILENAME_LEN];
  cUtils::replace(buf, ".raw", ".xml", infoFile, sizeof(infoFile));
  cFileInfo info;
  uint32_t sampleRate;
  int ret = info.readParameter(infoFile, sampleRate);
  if(ret != 0)
    sampleRate = cAudio::getSampleRateHz((enSampleRate)devPars.sampleRate.get());
  devStatus.freqMax.set((float)sampleRate / 2000);
  devStatus.graph.setPlotFile(devPars.fileName.get(), sampleRate);
  devStatus.waterf.setPlotFile(devPars.fileName.get(), sampleRate);
}

void setGpsLog(cMenuesystem* pThis, bool on)
{
  cPanel* pan = pThis->getPan(panGeo);
  pan->itemList[25].isVisible = on;
}

/*
void setGpsStatus(cMenuesystem* pThis)
{
  bool on = devPars.srcPosition.get() == enSrcPosition::GPS;

  cPanel* pan = pThis->getPan(panGeo);
  pan->itemList[23].isVisible = on;
  pan->itemList[24].isVisible = on;
  pan->itemList[26].isVisible = on;
  pan->itemList[27].isVisible = on;
} */

int MEMP initMainPanelExpert(cPanel* pan, tCoord lf)
{
  visRecCntIndex = 2;
  int r = 2;
  int err = pan->addTextItem(202,                  3,  30,             80, lf);
  err |= pan->addEnumItem(&devStatus.opMode,     150,  30,            140, lf, true);
  err |= pan->addNumItem(&devStatus.recCount,    260,  30 + r   * lf,  40, lf, false);
  pan->itemList[visRecCntIndex].isVisible = false;
  err |= pan->addTextItem(30,                      3,  30 +       lf,  80, lf);
  err |= pan->addEnumItem(&devStatus.playStatus, 150,  30 +       lf, 120, lf, false);
  err |= pan->addTextItem(25,                      3,  30 + r   * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.recAuto,      150,  30 + r++ * lf, 100, lf, true, dispModeFunc);
  err |= pan->addTextItem(32,                      3,  30 + r   * lf, 120, lf);
  err |= pan->addListItem(&devStatus.notes1,     150,  30 + r++ * lf, 169, lf, true);
  err |= pan->addTextItem(33,                      3,  30 + r   * lf, 120, lf);
  err |= pan->addListItem(&devStatus.notes2,     150,  30 + r++ * lf, 169, lf, true);
  err |= pan->addTextItem(203,                     3,  30 + r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.mixFreq,       150,  30 + r++ * lf,  15, lf, true);
  err |= pan->addTextItem(204,                     3,  30 + r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.volume,        150,  30 + r++ * lf,  20, lf, true);
  err |= pan->addTextItem(1325,                    3,  30 + r   * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.preAmpGain,   150,  30 + r++ * lf, 120, lf, true);
  err |= pan->addTextItem(200,                     3,  30 + r   * lf,  80, lf);
  err |= pan->addGeoItem(&devStatus.geoPos,      150,  30 + r++ * lf, 150, lf);
  err |= pan->addTextItem(193,                     3,  30 + r   * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.height,      150,  30 + r++ * lf, 150, lf, false);
  err |= pan->addTextItem(190,                     3,  30 + r   * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.satCount,    150,  30 + r++ * lf,  30, lf, false);
  err |= pan->addTextItem(191,                   200,  30 + r   * lf,  80, lf);
  setGpsLog(&menue, false);
  err |= pan->addTextItem(195,                     3,  30 + r   * lf,  80, lf);
  err |= pan->addEnumItem(&devStatus.posValid,   150,  30 + r   * lf, 150, lf, false);
  err |= pan->addTextItem(201,                     3, 200 + lf,        70, lf);
  err |= pan->addDateItem(&devStatus.date,       100, 200 + lf,        70, lf);
  err |= pan->addTimeItem(&devStatus.time,       180, 200 + lf,        70, lf);
  //setGpsStatus(&menue);
  return err;
}

int MEMP initMainPanelRecorder(cPanel* pan, tCoord lf)
{
  visRecCntIndex = 0;
  int r = 0;
  int x = 140;
  int err = pan->addNumItem(&devStatus.recCount, 260, 30 + r * lf, 40, lf, false);
  pan->itemList[visRecCntIndex].isVisible = false;
  err |= pan->addTextItem(25,                    3, 30 + r   * lf, 80, lf);
  err |= pan->addEnumItem(&devPars.recAuto,      x, 30 + r++ * lf, 100, lf, true, dispModeFunc);
  err |= pan->addTextItem(1365,                  3, 30 + r   * lf, 80, lf);
  err |= pan->addEnumItem(&devStatus.playStatus, x, 30 + r++ * lf, 120, lf, false);
  r++;
  err |= pan->addTextItem(32,                    3, 30 + r   * lf, 120, lf);
  err |= pan->addListItem(&devStatus.notes1,     x, 30 + r++ * lf, 169, lf, true);
  err |= pan->addTextItem(33,                    3, 30 + r   * lf, 120, lf);
  err |= pan->addListItem(&devStatus.notes2,     x, 30 + r++ * lf, 169, lf, true);
//  err |= pan->addTextItem(1325,                  3, 30 + r   * lf, 80, lf);
//  err |= pan->addEnumItem(&devPars.preAmpGain,   x, 30 + r++ * lf, 120, lf, true);

  if (devPars.srcPosition.get() != static_cast<uint32_t>(enPositionMode::FIX))
  {
    err |= pan->addTextItem(200, 3, 30 + r * lf, 80, lf);
    err |= pan->addGeoItem(&devStatus.geoPos,     x, 30 + r++ * lf, 150, lf, false);
    err |= pan->addTextItem(190,                  3, 30 + r * lf, 80, lf);
    err |= pan->addNumItem(&devStatus.satCount,   x, 30 + r++ * lf, 30, lf, false);
    err |= pan->addTextItem(191,                200, 30 + r * lf, 80, lf);
    setGpsLog(&menue, false);
    err |= pan->addTextItem(195,                  3, 30 + r * lf, 80, lf);
    err |= pan->addEnumItem(&devStatus.posValid,  x, 30 + r++ * lf, 150, lf, false);
  }
  else
  {
    err |= pan->addEnumItem(&devStatus.latSign,   x, 30 + r * lf, 10, lf, true, setPosFunc);
    err |= pan->addNumItem(&devStatus.latDeg,   x + 10, 30 + r * lf, 17, lf, true, setPosFunc);
    err |= pan->addTextItem(1340,               x + 25, 30 + r * lf, 10, lf);
    err |= pan->addNumItem(&devStatus.latMin,   x + 30, 30 + r * lf, 17, lf, true, setPosFunc);
    err |= pan->addTextItem(1345,               x + 45, 30 + r * lf, 10, lf);
    err |= pan->addNumItem(&devStatus.latSec,   x + 50, 30 + r * lf, 25, lf, true, setPosFunc);
    err |= pan->addEnumItem(&devStatus.lonSign, x + 90, 30 + r * lf, 10, lf, true, setPosFunc);
    err |= pan->addNumItem(&devStatus.lonDeg,   x + 100, 30 + r * lf, 25, lf, true, setPosFunc);
    err |= pan->addTextItem(1340,               x + 125, 30 + r * lf, 10, lf);
    err |= pan->addNumItem(&devStatus.lonMin,   x + 130, 30 + r * lf, 17, lf, true, setPosFunc);
    err |= pan->addTextItem(1345,               x + 145, 30 + r * lf, 10, lf);
    err |= pan->addNumItem(&devStatus.lonSec,   x + 150, 30 + r++ * lf, 25, lf, true, setPosFunc);
  }
  err |= pan->addTextItem(193,                  3,   30 + r * lf, 80, lf);
  err |= pan->addNumItem(&devStatus.height,     x,   30 + r++ * lf, 50, lf, devPars.srcPosition.get() == static_cast<uint32_t>(enPositionMode::FIX));
  err |= pan->addTextItem(440,                  3,   30 + r * lf, 80, lf);
  err |= pan->addNumItem(&devStatus.freeSpace,  x,   30 + r++ * lf, 80, lf, false);
  err |= pan->addTextItem(450,                  3,   30 + r * lf, 80, lf);
  err |= pan->addNumItem(&devStatus.voltage,    x,   30 + r * lf, 40, lf, false);
  err |= pan->addNumItem(&devStatus.digits,   245,   30 + r * lf, 40, lf, false);
  err |= pan->addTextItem(201,                   3, 200 + lf, 70, lf);
  err |= pan->addDateItem(&devStatus.date,     100, 200 + lf, 70, lf);
  err |= pan->addTimeItem(&devStatus.time,     180, 200 + lf, 70, lf);
  //setGpsStatus(&menue);
  return err;
}

bool editPos = true;


my_vector<int, 10> posIndex;

void enableEditPosition(cMenuesystem* pThis, bool on)
{
  if (devPars.menueType.get() == enMenueType::COMPACT)
  {
    cPanel* pan = pThis->getPan(panGeo);
    for (size_t i = 0; i < posIndex.size(); i++)
    {
      int itemIndex = posIndex[i];
      pan->itemList[itemIndex].isEdit = on;
    }
  }
}

my_vector<int, 10> timeIndex;
void enableEditTimes(cMenuesystem* pThis, bool on)
{
  if (devPars.menueType.get() == enMenueType::COMPACT)
  {
    cPanel* pan = pThis->getPan(panGeo);
    for (size_t i = 0; i < timeIndex.size(); i++)
    {
      int itemIndex = timeIndex[i];
      pan->itemList[itemIndex].isVisible = on;
    }
  }
}

void switchOffFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  pThis->showMsg(enMsg::YESNO, powerOffFunc, hasDisplay() == enDisplayType::OLED_128, Txt::get(1011), Txt::get(1012));
}

void MEMP positionModeFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  enPositionMode mode =static_cast<enPositionMode>(devPars.srcPosition.get());
  switch (mode)
  {
    case enPositionMode::FIX:
      enableEditPosition(pThis, true);
      gps.setMode(enGpsMode::GPS_OFF);
      break;

    case enPositionMode::GPS_ON:
      enableEditPosition(pThis, false);
      gps.setMode(enGpsMode::GPS_ON);
      break;

    case enPositionMode::GPS_AUTO:
      enableEditPosition(pThis, false);
      gps.setMode(enGpsMode::GPS_AUTO_OFF_AFTER_FIX);
      break;
  }
}

int MEMP initMainPanelCompact(cPanel* pan, tCoord lf)
{
  int err = 0;
  int r = 0;
  int x = 60;
  int y = lf + 3;

  err |= pan->addDateItem(&devStatus.date,      10, y + r   * lf, 70, lf);
  err |= pan->addTimeItem(&devStatus.time,      67, y + r++ * lf, 70, lf);
  err |= pan->addStrItem(&devStatus.battSymbol,   1, y + r   * lf, 25, lf);
  err |= pan->addStrItem(&devStatus.recStatus,  30, y + r   * lf, 8, lf);
  //err |= pan->addNumItem(&devStatus.chargeLevel,30, y + r * lf, 13, lf, false);
  //err |= pan->addTextItem(453,                  48, y + r * lf, 6, lf);
  err |= pan->addTextItem(441,                  60, y + r   * lf, 10, lf);
  err |= pan->addNumItem(&devStatus.freeSpace,  70, y + r   * lf, 13, lf, false);
  err |= pan->addTextItem(454,                  85, y + r++ * lf, 13, lf);
  y += 4;
  err |= pan->addTextItem(26,                    1, y + r   * lf, 70, lf);
  err |= pan->addEnumItem(&devPars.recAuto,  x - 7, y + r++ * lf, 73, lf, true, dispModeFunc);
  
  timeIndex.clear();
  timeIndex.push_back(pan->itemList.size());
  err |= pan->addNumItem(&devPars.startH,       26, y + r   * lf, 13, lf, true);
  timeIndex.push_back(pan->itemList.size());
  err |= pan->addTextItem(1182,                 40, y + r   * lf, 5, lf);
  timeIndex.push_back(pan->itemList.size());
  err |= pan->addNumItem(&devPars.startMin,     45, y + r   * lf, 13, lf, true);
  timeIndex.push_back(pan->itemList.size());
  err |= pan->addTextItem(1183,                 60, y + r   * lf, 6, lf);
  timeIndex.push_back(pan->itemList.size());
  err |= pan->addNumItem(&devPars.stopH,        68, y + r   * lf, 13, lf, true);
  timeIndex.push_back(pan->itemList.size());
  err |= pan->addTextItem(1182,                 82, y + r   * lf, 5, lf);
  timeIndex.push_back(pan->itemList.size());
  err |= pan->addNumItem(&devPars.stopMin,      87, y + r++ * lf, 13, lf, true);
  err |= pan->addTextItem(27,                    1, y + r   * lf, 45, lf);
  err |= pan->addNumItem(&devStatus.recCount,    x, y + r++ * lf, 25, lf, false);
  //err |= pan->addTextItem(1365,                 80, y + r * lf, 45, lf);
  y += 4;
  err |= pan->addEnumItem(&devPars.srcPosition,  1, y + r++ * lf, 118, lf, true, positionModeFunc);
  posIndex.clear();
  posIndex.push_back(pan->itemList.size());
  err |= pan->addEnumItem(&devStatus.latSign,    1, y + r   * lf, 6, lf, true, setPosFunc);
  posIndex.push_back(pan->itemList.size());
  err |= pan->addNumItem(&devStatus.latDeg,      7, y + r   * lf, 13, lf, true, setPosFunc);
  err |= pan->addTextItem(1340,                 19, y + r   * lf, 5, lf);
  posIndex.push_back(pan->itemList.size());
  err |= pan->addNumItem(&devStatus.latMin,     24, y + r   * lf, 13, lf, true, setPosFunc);
  err |= pan->addTextItem(1345,                 36, y + r   * lf, 5, lf);
  posIndex.push_back(pan->itemList.size());
  err |= pan->addNumItem(&devStatus.latSec,    39, y + r * lf, 18, lf, true, setPosFunc);
  posIndex.push_back(pan->itemList.size());
  err |= pan->addEnumItem(&devStatus.lonSign,  63, y + r * lf, 6, lf, true, setPosFunc);
  posIndex.push_back(pan->itemList.size());
  err |= pan->addNumItem(&devStatus.lonDeg,    69, y + r * lf, 19, lf, true, setPosFunc);
  err |= pan->addTextItem(1340,                87, y + r * lf, 5, lf);
  posIndex.push_back(pan->itemList.size());
  err |= pan->addNumItem(&devStatus.lonMin,    92, y + r * lf, 13, lf, true, setPosFunc);
  err |= pan->addTextItem(1345,               104, y + r * lf, 5, lf);
  posIndex.push_back(pan->itemList.size());
  err |= pan->addNumItem(&devStatus.lonSec,   107, y + r++ * lf, 18, lf, true, setPosFunc);

  err |= pan->addBtnItem(108, 15, y + r * lf + 5, 90, 15, switchOffFunc);

  /*
  err |= pan->addTextItem(26,                    1,      y + r * lf,   70, lf);
  err |= pan->addEnumItem(&devPars.recAuto,      x -  7, y + r++ * lf, 73, lf, true, dispModeFunc);
  err |= pan->addTextItem(27,                    1,      y + r * lf,   70, lf);
  err |= pan->addNumItem(&devStatus.recCount,    x,      y + r++ * lf, 50, lf, false);
 // pan->itemList[visRecCntIndex].isVisible = false;
  err |= pan->addTextItem(1365,                  1,      y + r * lf,   45, lf);
  err |= pan->addStrItem(&devStatus.recStatus,  45,      y + r * lf,    8, lf);
  err |= pan->addEnumItem(&devStatus.playStatus, x,      y + r++ * lf,120, lf, false);

 // err |= pan->addTextItem(193,                 1,      y + r * lf,   80, lf);
 // err |= pan->addNumItem(&devStatus.height,    x,      y + r++ * lf, 50, lf, devPars.srcPosition.get() == enPositionMode::POS_FIX);
//  err |= pan->addTextItem(451,                   1,      y + r * lf,   70, lf);
  err |= pan->addStrItem(&devStatus.batSymbol,    1,     y + r * lf, 25, lf);
  err |= pan->addNumItem(&devStatus.chargeLevel, 30,     y + r * lf,   13, lf, false);
  err |= pan->addTextItem(453,                   45,     y + r * lf,    6, lf);
  err |= pan->addTextItem(441,                   60,     y + r * lf,   10, lf);
  err |= pan->addNumItem(&devStatus.freeSpace,   70,     y + r * lf,   13, lf, false);
  err |= pan->addTextItem(454,                   85,     y + r++ * lf, 13, lf);
  r++;
  //  err |= pan->addNumItem(&devStatus.voltage,     x + 32, y + r * lf,   23, lf, false);
//  err |= pan->addTextItem(452,                   x + 56, y + r++ * lf,  8, lf);
  err |= pan->addDateItem(&devStatus.date,       1,      y + r * lf,   70, lf);
  err |= pan->addTimeItem(&devStatus.time,       x,      y + r++ * lf, 70, lf);
  err |= pan->addTextItem(460,                   1,      y + r * lf,   70, lf);
  err |= pan->addEnumItem(&devStatus.gpsStatus,  x - 30, y + r * lf,   35, lf, false);
  err |= pan->addTextItem(461,                   x + 15, y + r * lf,   23, lf);
  err |= pan->addNumItem(&devStatus.satCount,    x + 40, y + r++ * lf, 15, lf, false);

  err |= pan->addBtnItem(108, 15, y + r * lf + 5, 80, 15, switchOffFunc);
  */
  return err;
}



/*

int MEMP initGpsPanelCompact(cPanel* pan, tCoord lf)
{
  int y = lf + 5;
  int r = 1;
  int err = 0;
  int xg = 10;
  int x = 70;
  err |= pan->addTextItem(1421,                   1, y + r   * lf, 60, lf);
  err |= pan->addEnumItem(&devPars.srcPosition,   x, y + r++ * lf, 50, lf, true, positionModeFunc);
  r++;
  posIndex.clear();
  posIndex.push_back(pan->itemList.size());
  err |= pan->addEnumItem(&devStatus.latSign,    xg, y + r  * lf, 10, lf, true, setPosFunc);
  posIndex.push_back(pan->itemList.size());
  err |= pan->addNumItem(&devStatus.latDeg,      xg + 16, y + r * lf, 13, lf, true, setPosFunc);
  err |= pan->addTextItem(1340, xg + 30, y + r * lf, 5, lf);
  posIndex.push_back(pan->itemList.size());
  err |= pan->addNumItem(&devStatus.latMin, xg + 38, y + r * lf, 13, lf, true, setPosFunc);
  err |= pan->addTextItem(1345, xg + 51, y + r * lf, 5, lf);
  posIndex.push_back(pan->itemList.size());
  err |= pan->addNumItem(&devStatus.latSec, xg + 56, y + r++ * lf, 25, lf, true, setPosFunc);
  posIndex.push_back(pan->itemList.size());
  err |= pan->addEnumItem(&devStatus.lonSign, xg, y + r * lf, 10, lf, true, setPosFunc);
  posIndex.push_back(pan->itemList.size());
  err |= pan->addNumItem(&devStatus.lonDeg, xg + 10, y + r * lf, 21, lf, true, setPosFunc);
  err |= pan->addTextItem(1340, xg + 30, y + r * lf, 5, lf);
  posIndex.push_back(pan->itemList.size());
  err |= pan->addNumItem(&devStatus.lonMin, xg + 38, y + r * lf, 13, lf, true, setPosFunc);
  err |= pan->addTextItem(1345, xg + 51, y + r * lf, 5, lf);
  posIndex.push_back(pan->itemList.size());
  err |= pan->addNumItem(&devStatus.lonSec, xg + 56, y + r++ * lf, 25, lf, true, setPosFunc);
  r++;
  err |= pan->addTextItem(30, 1, y + r * lf, 70, lf);
  err |= pan->addEnumItem(&devStatus.gpsStatus, x, y + r++ * lf, 35, lf, false);
  err |= pan->addTextItem(461, 1, y + r * lf, 23, lf);
  err |= pan->addNumItem(&devStatus.satCount, x, y + r++ * lf, 15, lf, false);
  return err;
}

*/
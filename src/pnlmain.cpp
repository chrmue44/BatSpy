/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include <Arduino.h>
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


#include "startup_pic.c_"

void setHeaderPanelText(cMenuesystem* pThis, thText t)
{
  hdrMainPanel = pThis->getHdrPanel();
  pThis->getPan(hdrMainPanel)->clear();
  pThis->getPan(hdrMainPanel)->addTextItem(t, LINE_HEIGHT, 1, 180, LINE_HEIGHT);
}
void MEMP showSplashScreen(ILI9341_t3& tft, bool waitBtn)
{
   char buf[80];

  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.drawRect(2, 2, DISP_WIDTH - 2, DISP_HEIGHT - 2, ILI9341_YELLOW);
  tft.fillRect(3, 3, DISP_WIDTH - 4, DISP_HEIGHT - 4, COL_TEXTDROPBACK);

  tft.setCursor(140, 10);
  tft.print("BatSpy");
  tft.setCursor(30, 25);
  tft.print(Txt::get(1700));
  tft.writeRect(96,55,128, 128, startup_pic);
  tft.setCursor(30, 195);
  tft.print(Txt::get(1702));
  tft.print(devStatus.version.get());
  tft.setCursor(30, 210);
  tft.print("(C) 2021..23 Christian M" CH_UEs "ller");
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setCursor(140, 225);
  cUtils::replaceUTF8withInternalCoding(Txt::get(1704), buf, sizeof(buf));
  tft.print(buf);

  bool exit = false;
  if(waitBtn)
  {
    do
    {
      exit = !digitalRead(PIN_ROT_LEFT_S);
    } while (!exit);
  }
  else
    delay(1000);
  tft.fillScreen(ILI9341_BLACK);
}

// *******************************************
// drop down panel F1 for main panel

void MEMP powerOffFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  switch (key) 
  {
    case enKey::YES:
      powerOff();
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
      pThis->showMsg(enMsg::YESNO, powerOffFunc, Txt::get(1010));
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
    pThis->showMsg(enMsg::YESNO, powerOffFunc, Txt::get(1010));
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
    pThis->showMsg(enMsg::YESNO, powerOffFunc, Txt::get(1010));
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

void MEMP f1Func(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  stPanelItem item;
  item.type = ITEM_ENUM;
  item.p = &f1MainItems;
  item.f = nullptr;
  switch (devPars.menueType.get())
  {
  case enMenueType::EXPERT:
    pThis->createDropDown(item, 1, DISP_HEIGHT - f1MainItems.size() * LINE_HEIGHT - 15, 120, f1DropFuncExpert);
    break;
  case enMenueType::RECORDER:
    pThis->createDropDown(item, 1, DISP_HEIGHT - f1MainItems.size() * LINE_HEIGHT - 15, 120, f1DropFuncRecorder);
    break;
  case enMenueType::HANDHELD:
    pThis->createDropDown(item, 1, DISP_HEIGHT - f1MainItems.size() * LINE_HEIGHT - 15, 120, f1DropFuncHandheld);
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
      if (devStatus.playStatus.get() == enPlayStatus::ST_STOP)
        devStatus.playStatus.set(enPlayStatus::ST_REC);
      else
        devStatus.playStatus.set(enPlayStatus::ST_STOP);
    }
    else 
      pThis->showMsg(enMsg::INFO, nullptr, Txt::get(1015), Txt::get(1016));
  }
  else
  {
    if (devStatus.playStatus.get() == enPlayStatus::ST_STOP)
      devStatus.playStatus.set(enPlayStatus::ST_PLAY);
    else
      devStatus.playStatus.set(enPlayStatus::ST_STOP);
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
  pThis->createDropDown(item, DISP_WIDTH - 120 - 1, DISP_HEIGHT - f4MainItems.size() * LINE_HEIGHT - 15, 120, f4DropFunc);
}


int MEMP initFkeyPanel(cPanel* pan, tCoord lf)
{
  int retVal;
  // Serial.println("initDialogs2");
 
  retVal = pan->addTextItem(1, 0, 227, 80, lf, true, f1Func);
  retVal |= pan->addTextItem(2,  81, 227, 79, lf, true, f2Func);
  if(devPars.srcPosition.get() != enPositionMode::POS_FIX)
    retVal |= pan->addTextItem(6, 161, 227, 79, lf, true, f3Func);
  retVal |= pan->addTextItem(4, 241, 227, 79, lf, true, f4Func);
  return retVal;
}

int visRecCntIndex = 0;

void MEMP setVisibilityRecCount(cMenuesystem* pThis)
{
  thPanel i = pThis->getMainPanel();
  cPanel* p = pThis->getPan(i);
  if(devPars.recAuto.get() > 0)
    p->itemList[visRecCntIndex].isVisible = true; 
  else
    p->itemList[visRecCntIndex].isVisible = false;
}

void MEMP dispModeFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  setVisibilityRecCount(pThis);
  calcSunrise();
  if (devPars.recAuto.get() == enRecAuto::ON)
    audio.openProject();
  if (devPars.recAuto.get() == enRecAuto::OFF)
    audio.closeProject();
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
  if (hasAmpRevB())
  {
    err |= pan->addTextItem(1320,                  3,  30 + r   * lf,   80, lf);
    err |= pan->addEnumItem(&devPars.preAmpType, 150,  30 + r++ * lf,  120, lf, true);
  }
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
  if (!hasAmpRevB())
  {
    err |= pan->addTextItem(1320,                  3, 30 + r   * lf, 80, lf);
    err |= pan->addEnumItem(&devPars.preAmpType,   x, 30 + r++ * lf, 120, lf, true);
  }
//  err |= pan->addTextItem(1325,                  3, 30 + r   * lf, 80, lf);
//  err |= pan->addEnumItem(&devPars.preAmpGain,   x, 30 + r++ * lf, 120, lf, true);

  if (devPars.srcPosition.get() != enPositionMode::POS_FIX)
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
  err |= pan->addNumItem(&devStatus.height,     x,   30 + r++ * lf, 50, lf, devPars.srcPosition.get() == enPositionMode::POS_FIX);
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

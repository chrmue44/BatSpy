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
#define OWN_H
#include "pnlmain.h"
#undef OWN_H
#include "pnllive.h"
#include "globals.h"

cParEnum f1MainItems(0);
cParEnum f4MainItems(0);

#include "startup_pic.c_"

void MEMP showSplashScreen(ILI9341_t3& tft, bool waitBtn)
{
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
  tft.print("(C) 2021 Christian M" CH_UEs "ller");
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setCursor(140, 225);
  tft.print(Txt::get(1704));

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
      sysLog.close();
      gpsLog.close();
      digitalWrite(PIN_POWER_OFF, 0);
      delay(1000);
      for(;;)
        ;
      break;
      
    default:
    case enKey::NO:
      break;
  }
}

void MEMP f1DropFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
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


void MEMP initFunctionItems()
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
}


void MEMP f1Func(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  stPanelItem item;
  item.type = ITEM_ENUM;
  item.p = &f1MainItems;
  item.f = nullptr;
  pThis->createDropDown(item, 1, DISP_HEIGHT - f1MainItems.size() * LINE_HEIGHT - 15, 120, f1DropFunc);
}

void MEMP f2Func(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  if ((devStatus.opMode.get() == enOpMode::HEAR_HET) ||
      (devStatus.opMode.get() == enOpMode::HEAR_DIRECT))
  {
    if(devPars.projectType.get() == enProjType::DATE_TIME)
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

void MEMP f4DropFunc(cMenuesystem* pThis, enKey key, cParBase* pItem) 
{
  switch (pThis->getFocusItem()) 
  {
    case 0:
      while(digitalRead(PIN_ROT_LEFT_S) == 0);
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
      pThis->setHdrPanel(hdrParams);
      pThis->setFkeyPanel(fkeyMainPan);
      break;

    case 4:
      pThis->setMainPanel(panParRec);
      pThis->setHdrPanel(hdrParams);
      pThis->setFkeyPanel(fkeyMainPan);
      break;

    case 5:
      devStatus.year.set(devStatus.date.getYear());
      devStatus.month.set(devStatus.date.getMonth());
      devStatus.day.set(devStatus.date.getDay());
      devStatus.hour.set(devStatus.time.getHour());
      devStatus.minute.set(devStatus.time.getMin());
      pThis->setMainPanel(panDateTime);
      pThis->setHdrPanel(hdrParams);
      pThis->setFkeyPanel(fkeyMainPan);
      break;

    case 6:
      devStatus.latDeg.set(devStatus.geoPos.getDegLat());
      devStatus.latMin.set(devStatus.geoPos.getMinLat());
      devStatus.latSec.set(devStatus.geoPos.getSecLat());
      if(devStatus.latDeg.get() > 0)
        devStatus.latSign.set(0);
      else
        devStatus.latSign.set(1);
      devStatus.lonDeg.set(devStatus.geoPos.getDegLon());
      devStatus.lonMin.set(devStatus.geoPos.getMinLon());
      devStatus.lonSec.set(devStatus.geoPos.getSecLon());
      if(devStatus.lonDeg.get() > 0)
        devStatus.lonSign.set(0);
      else
        devStatus.lonSign.set(1);
      pThis->setMainPanel(panPosition);
      pThis->setHdrPanel(hdrParams);
      pThis->setFkeyPanel(fkeyMainPan);
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
  pThis->createDropDown(item, DISP_WIDTH - 120 - 1, DISP_HEIGHT - f4MainItems.size() * LINE_HEIGHT - 15, 120, f4DropFunc);
}


int MEMP initFkeyPanel(cPanel* pan, tCoord lf)
{
  int retVal;
  // Serial.println("initDialogs2");
  retVal  = pan->addTextItem(1,   0, 227, 80, lf, true, f1Func);
  retVal |= pan->addTextItem(2,  81, 227, 79, lf, true, f2Func);
  retVal |= pan->addTextItem(6, 161, 227, 79, lf, true, f3Func);
  retVal |= pan->addTextItem(4, 241, 227, 79, lf, true, f4Func);
  return retVal;
}


void MEMP setVisibilityRecCount(cMenuesystem* pThis)
{
  thPanel i = pThis->getMainPanel();
  cPanel* p = pThis->getPan(i);
  if(devPars.recAuto.get() > 0)
    p->itemList[2].isVisible = true;
  else
    p->itemList[2].isVisible = false;
}

void MEMP dispModeFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  setVisibilityRecCount(pThis);
}


void MEMP setFileToDisplay(const char* buf)
{
  char infoFile[FILENAME_LEN];
  cUtils::replace(buf, ".raw", ".xml", infoFile, sizeof(infoFile));
  cFileInfo info;
  uint32_t sampleRate;
  int ret = info.readParameter(infoFile, sampleRate);
  ////if(ret != 0)
  ////  sampleRate = cAudio::getSampleRateHz((enSampleRate)devPars.sampleRate.get());
  devStatus.freqMax.set(sampleRate / 2000);
  devStatus.graph.setPlotFile(devPars.fileName.get(), sampleRate);
  devStatus.waterf.setPlotFile(devPars.fileName.get(), sampleRate);
}

void setGpsLog(cMenuesystem* pThis, bool on)
{
  cPanel* pan = pThis->getPan(panGeo);
  pan->itemList[21].isVisible = on;
}

int MEMP initMainPanel(cPanel* pan, tCoord lf)
{
  int err = pan->addTextItem(202,                  3,  30,            80, lf);
  err |= pan->addEnumItem(&devStatus.opMode,     150,  30,           140, lf, true);
  err |= pan->addNumItem(&devStatus.recCount,    260,  30 +  2 * lf,  40, lf, false);
  pan->itemList[2].isVisible = false;
  err |= pan->addTextItem(30,                      3,  30 +      lf,  80, lf);
  err |= pan->addEnumItem(&devStatus.playStatus, 150,  30 +      lf, 120, lf, false);
  err |= pan->addTextItem(25,                      3,  30 +  2 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.recAuto,      150,  30 +  2 * lf,  80, lf, true, dispModeFunc);
  err |= pan->addTextItem(203,                     3,  30 +  3 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.mixFreq,       150,  30 +  3 * lf,  15, lf, true);
  err |= pan->addTextItem(204,                     3,  30 +  4 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.volume,        150,  30 +  4 * lf,  20, lf, true);
  err |= pan->addTextItem(1320,                    3,  30 +  5 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.preAmpType,   150,  30 +  5 * lf, 120, lf, true);
  err |= pan->addTextItem(1325,                    3,  30 +  6 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.preAmpGain,   150,  30 +  6 * lf, 120, lf, true);
  err |= pan->addTextItem(200,                     3,  30 +  8 * lf,  80, lf);
  err |= pan->addGeoItem(&devStatus.geoPos,      150,  30 +  8 * lf, 150, lf);
  err |= pan->addTextItem(193,                     3,  30 +  9 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.height,      150,  30 +  9 * lf, 150, lf, false);
  err |= pan->addTextItem(190,                     3,  30 + 10 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.satCount,    150,  30 + 10 * lf,  30, lf, false);
  err |= pan->addTextItem(191,                   200,  30 + 10 * lf,  80, lf);
  setGpsLog(&menue, false);
  err |= pan->addTextItem(195,                     3,  30 + 11 * lf,  80, lf);
  err |= pan->addEnumItem(&devStatus.posValid,   150,  30 + 11 * lf, 150, lf, false);
  err |= pan->addTextItem(201,                     3, 200 + lf,       70, lf);
  err |= pan->addDateItem(&devStatus.date,       100, 200 + lf,       70, lf);
  err |= pan->addTimeItem(&devStatus.time,       180, 200 + lf,       70, lf);
  return err;
}

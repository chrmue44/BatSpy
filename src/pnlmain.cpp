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
#include "cAudio.h"
#include "cfileinfo.h"
#define OWN_H
#include "pnlmain.h"
#undef OWN_H
#include "pnllive.h"

cParEnum f1MainItems(0);
cParEnum f4MainItems(0);



// *******************************************
// drop down panel F1 for main panel

void powerOffFunc(cMenuesystem* pThis, enKey key, cParBase* pItem) {
  switch (key) 
  {
    case enKey::YES:
      digitalWrite(PIN_POWER_OFF, 0);
      delay(1000);
      break;
      
    default:
    case enKey::NO:
      break;
  }
}

void f1DropFunc(cMenuesystem* pThis, enKey key, cParBase* pItem) {
  switch (pThis->getFocusItem()) {
    case 0:
      pThis->setMainPanel(panGeo);
      pThis->setHdrPanel(hdrMainPanel);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
    case 1:
      devStatus.graph.initPlot(true);
      pThis->setMainPanel(panTime);
      pThis->setHdrPanel(hdrPanWaterfall);;
      pThis->setFkeyPanel(fkeyWaterPan);
      break;
    case 2:
      devStatus.waterf.initPlot(true);
      pThis->setMainPanel(panWaterfall);
      pThis->setHdrPanel(hdrPanWaterfall);
      pThis->setFkeyPanel(fkeyWaterPan);
      break;
    case 3:
      devStatus.grafLive.initPlot(true);
      setSampleRateLivePan();
      pThis->setMainPanel(pnlLive);
      pThis->setHdrPanel(hdrPanWaterfall);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
    case 4:
      pThis->setMainPanel(panBats);
      pThis->setHdrPanel(hdrBatInfo);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
    case 5:
      pThis->setMainPanel(panFont);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
    case 6:
      pThis->setMainPanel(panInfo);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
    case 7:
      pThis->setMainPanel(panFileBrowser);
      pThis->setHdrPanel(hdrPanWaterfall);
      pThis->setFkeyPanel(fkeyFilePan);
    break;
    case 8:
      pThis->showMsg(enMsg::YESNO, powerOffFunc, Txt::get(1010));
      break;
  }
}


void initFunctionItems()
{
  f1MainItems.addItem(100);
  f1MainItems.addItem(101);
  f1MainItems.addItem(102);
  f1MainItems.addItem(103);
  f1MainItems.addItem(104);
  f1MainItems.addItem(105);
  f1MainItems.addItem(106);
  f1MainItems.addItem(112);
  f1MainItems.addItem(110);

  f4MainItems.addItem(1001);
  f4MainItems.addItem(1021);
  f4MainItems.addItem(1030);
  f4MainItems.addItem(1034);
  f4MainItems.addItem(1031);
  f4MainItems.addItem(1033);
}


void f1Func(cMenuesystem* pThis, enKey key, cParBase* pItem) {
  stPanelItem item;
  item.type = ITEM_ENUM;
  item.p = &f1MainItems;
  item.f = nullptr;
  pThis->createDropDown(item, 1, DISP_HEIGHT - f1MainItems.size() * LINE_HEIGHT - 15, 120, f1DropFunc);

}

void f2Func(cMenuesystem* pThis, enKey key, cParBase* pItem) {
  if ((devStatus.opMode.get() == enOpMode::HEAR_HET) ||
      (devStatus.opMode.get() == enOpMode::HEAR_DIRECT)) {
    if (devStatus.playStatus.get() == 0)
      devStatus.playStatus.set(2);
    else
      devStatus.playStatus.set(0);
  }
  else {
    if (devStatus.playStatus.get() == 0)
      devStatus.playStatus.set(1);
    else
      devStatus.playStatus.set(0);
  }
}

void f4LoadFunc(cMenuesystem* pThis, enKey key, cParBase* pItem) {
  switch (key) {
    case YES:
      pThis->load();
      break;

    case enKey::NO:
    default:
      break;
  }
}

void f4SaveFunc(cMenuesystem* pThis, enKey key, cParBase* pItem) {
  switch (key) {
    case enKey::YES:
      pThis->save();
      break;
    
    case enKey::NO:
    default:
      break;
  }
}

void f4DropFunc(cMenuesystem* pThis, enKey key, cParBase* pItem) {
  switch (pThis->getFocusItem()) {
    case 0:
      pThis->showMsg(enMsg::YESNO, f4LoadFunc, Txt::get(1005), Txt::get(1006));
      break;
    
    case 1:
      pThis->showMsg(enMsg::YESNO, f4SaveFunc, Txt::get(1005), Txt::get(1025));
      break;
    
    case 2:
      pThis->setMainPanel(panParams);
      pThis->setHdrPanel(hdrParams);
      pThis->setFkeyPanel(fkeyMainPan);
      break;

    case 3:
      pThis->setMainPanel(panParRec);
      pThis->setHdrPanel(hdrParams);
      pThis->setFkeyPanel(fkeyMainPan);
      break;

    case 4:
      devStatus.year.set(devStatus.date.getYear());
      devStatus.month.set(devStatus.date.getMonth());
      devStatus.day.set(devStatus.date.getDay());
      devStatus.hour.set(devStatus.time.getHour());
      devStatus.minute.set(devStatus.time.getMin());
      pThis->setMainPanel(panDateTime);
      pThis->setHdrPanel(hdrParams);
      pThis->setFkeyPanel(fkeyMainPan);
      break;

    case 5:
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

void f4Func(cMenuesystem* pThis, enKey key, cParBase* pItem) {
  stPanelItem item;
  item.type = ITEM_ENUM;
  item.p = &f4MainItems;
  pThis->createDropDown(item, DISP_WIDTH - 120 - 1, DISP_HEIGHT - f4MainItems.size() * LINE_HEIGHT - 15, 120, f4DropFunc);
}


int initFkeyPanel(cPanel* pan, tCoord lf)
{
  int retVal;
  // Serial.println("initDialogs2");
  retVal  = pan->addTextItem(1,   0, 227, 80, lf, true, f1Func);
  retVal |= pan->addTextItem(2,  81, 227, 79, lf, true, f2Func);
  retVal |= pan->addTextItem(3, 161, 227, 79, lf, false);
  retVal |= pan->addTextItem(4, 241, 227, 79, lf, true, f4Func);
  return retVal;
}


void setVisibilityRecCount(cMenuesystem* pThis)
{
  thPanel i = pThis->getMainPanel();
  cPanel* p = pThis->getPan(i);
  if(devPars.recAuto.get() > 0)
    p->itemList[2].isVisible = true;
  else
    p->itemList[2].isVisible = false;
}

void dispModeFunc(cMenuesystem* pThis, enKey key, cParBase* pItem) {
  setVisibilityRecCount(pThis);
}


void setFileToDisplay(const char* buf) {
  char infoFile[FILENAME_LEN];
  cUtils::replace(buf, ".raw", ".xml", infoFile, sizeof(infoFile));
  cFileInfo info;
  uint32_t sampleRate;
  int ret = info.readParameter(infoFile, sampleRate);
  if(ret != 0)
    sampleRate = cAudio::getSampleRateHz((enSampleRate)devPars.sampleRate.get());
  devStatus.freqMax.set(sampleRate / 2000);
  devStatus.graph.setPlotFile(devPars.fileName.get(), sampleRate);
  devStatus.waterf.setPlotFile(devPars.fileName.get(), sampleRate);
}

void fileFunc(cMenuesystem* pThis, enKey key, cParBase* pItem) {
  enSdRes rc = enSdRes::OK;
  cSdCard& sd = cSdCard::inst();
  enFocusState state = pThis->getFocusState();
  char buf[FILENAME_LEN];

  if (pThis->isDropDownInFocus()) {
    strncpy(buf, sd.getActDir(), FILENAME_LEN);
    size_t len = strlen(buf);
    if (buf[len - 1] != '/')
      strcat(buf, "/");
    strcat(buf, devPars.fileSel.getActText());
    devPars.fileName.set(buf);
  }
  //
  else {
    if ((state == SELECT) && (key == enKey::KEY_OK)) {
      tDirInfo p;
      rc = sd.dir(p);
      devPars.fileSel.clear();
      if (rc == 0) {
        tDirInfo& dir = p;
        for (size_t i = 0; i < dir.size(); i++) {
          char ext[8];
          if (!dir[i].isDir) {
            char* pName = dir[i].name;
            cUtils::getExtension(pName, ext, sizeof(ext));
            if((strcmp(ext,"RAW") == 0) || (strcmp(ext,"raw") == 0))
              devPars.fileSel.addItem(dir[i].name);
          }
        }
      }
    }
  }
  setFileToDisplay(buf);
}


void dirFunc(cMenuesystem* pThis, enKey key, cParBase* pItem) {
  enSdRes rc = enSdRes::OK;
  cSdCard& sd = cSdCard::inst();
  enFocusState state = pThis->getFocusState();
  // leaving dropdown
  if (pThis->isDropDownInFocus()) {
    sd.chdir(devPars.dirSel.getActText());
    devPars.fileName.set(sd.getActDir());
    sd.dir(devStatus.dir);
    devStatus.fileIndex = 1;
  }
  // before opening dropdown
  else {
    if ((state == SELECT) && (key = enKey::KEY_OK)) {
      tDirInfo p;
      DPRINTLN1("dirFunc"); //@@@
      rc = sd.dir(p);
      devPars.dirSel.clear();
      if (rc == OK) {
        tDirInfo& dir = p;
        for (size_t i = 0; i < dir.size(); i++) {
          if (dir[i].isDir) {
            devPars.dirSel.addItem(dir[i].name);
            DPRINTF2("dir name: %s\n", dir[i].name);
          }
        }
      }
      DPRINTF2("devPars.dirSel.size: %u\n", devPars.dirSel.size());
    }
  }
}

int initMainPanel(cPanel* pan, tCoord lf)
{
  int err = pan->addTextItem(202,                  3, 30,            80, lf);
  err |= pan->addEnumItem(&devStatus.opMode,     150, 30,           140, lf, true);
  err |= pan->addNumItem(&devStatus.recCount,    260, 30 +  2 * lf,  40, lf, false);
  err |= pan->itemList[2].isVisible = false;
  err |= pan->addTextItem(30,                      3, 30 +      lf,  80, lf);
  err |= pan->addEnumItem(&devStatus.playStatus, 150, 30 +      lf, 120, lf, false);
  err |= pan->addTextItem(25,                      3, 30 +  2 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.recAuto,      150, 30 +  2 * lf,  80, lf, true, dispModeFunc);
  err |= pan->addTextItem(206,                     3, 30 +  3 * lf,  80, lf);
  err |= pan->addListItem(&devPars.dirSel,       100, 30 +  3 * lf, 210, lf, true, dirFunc);
  err |= pan->addTextItem(205,                     3, 30 +  4 * lf,  80, lf);
  err |= pan->addListItem(&devPars.fileSel,      100, 30 +  4 * lf, 210, lf, true, fileFunc);
  err |= pan->addTextItem(208,                     3, 30 +  5 * lf,  80, lf);
  err |= pan->addStrItem(&devPars.fileName,      100, 30 +  5 * lf, 210, lf);
  err |= pan->addTextItem(203,                     3, 30 +  7 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.mixFreq,       150, 30 +  7 * lf,  15, lf, true);
  err |= pan->addTextItem(204,                     3, 30 +  8 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.volume,        150, 30 +  8 * lf,  20, lf, true);
  err |= pan->addTextItem(1320,                    3, 30 +  9 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.preAmpType,   150, 30 +  9 * lf, 120, lf, true);
  err |= pan->addTextItem(1325,                    3, 30 + 10 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.preAmpGain,   150, 30 + 10 * lf, 120, lf, true);

  err |= pan->addTextItem(201,                     3, 200 + lf,     70, lf);
  err |= pan->addDateItem(&devStatus.date,       100, 200 + lf,     70, lf);
  err |= pan->addTimeItem(&devStatus.time,       180, 200 + lf,     70, lf);
  return err;
}

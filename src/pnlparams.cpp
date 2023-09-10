/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "pnlparams.h"
#include "globals.h"
#include "debug.h"
#include "config.h"
#include "csunrise.h"
#include "pnlmain.h"

extern cRtc rtc;

void MEMP languageFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  switch(devPars.lang.get()) {
    case 0:
      Txt::setLang(LANG_GER);
      break;
    case 1:
      Txt::setLang(LANG_EN);
      break;
  }
}

void MEMP voltageFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  float fact = calcVoltageFactor(devStatus.setVoltage.get());
  devPars.voltFactor.set(fact);
}

void MEMP calcSunrise()
{
  int srH, srM, ssH, ssM;
  cSunRise::getSunSetSunRise(devStatus.geoPos.getLat(), devStatus.geoPos.getLon(),
                                 year(), month(), day(), srH, srM, ssH, ssM);
  devPars.startH.set(ssH);
  devPars.startMin.set(ssM);
  devPars.stopH.set(srH);
  devPars.stopMin.set(srM);
}

void MEMP fuCalcSunrise(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  if(devPars.recAuto.get() == enRecAuto::TWILIGHT)
    calcSunrise();
  else
  {
    devPars.startH.set(cMenue::readInt16FromEep(0x0054));   //if addr changes see also pnlparams.cpp
    devPars.startMin.set(cMenue::readInt16FromEep(0x0056)); //if addr changes see also pnlparams.cpp
    devPars.stopH.set(cMenue::readInt16FromEep(0x0058));    //if addr changes see also pnlparams.cpp
    devPars.stopMin.set(cMenue::readInt16FromEep(0x005A));  //if addr changes see also pnlparams.cpp
  }
}

void MEMP fuMenuMode(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  enMenueType m = (enMenueType)devPars.menueType.get();
  f1MainItems.clear();
  f4MainItems.clear();
  switch (m)
  {
    default:
    case enMenueType::EXPERT:
      initFunctionItemsExpert();
      break;
    case enMenueType::HANDHELD:
      initFunctionItemsHandheld();
      break;
    case enMenueType::RECORDER:
      initFunctionItemsRecorder();
      break;
  }
}

int MEMP initParRec(cPanel* pan, tCoord lf)
{
  int  err = 0;
  int r = 1;
  err |= pan->addTextItem(1110,                  15, 20            ,  80, lf);
  err |= pan->addEnumItem(&devPars.sampleRate,  190, 20            ,  30, lf, true);
  err |= pan->addTextItem(1120,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.recTime,      190, 20 +  r++ * lf,  30, lf, true);
  err |= pan->addTextItem(1140,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.recThreshhold,190, 20 +  r++ * lf,  30, lf, true);
  err |= pan->addTextItem(1360,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.triggerType, 190, 20 +  r++ * lf, 120, lf, true);
  err |= pan->addTextItem(1370,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.minEventLen,  190, 20 +  r++ * lf,  40, lf, true);
  err |= pan->addTextItem(1170,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.filtFreq,     190, 20 +  r++ * lf,  30, lf, true);
  err |= pan->addTextItem(1175,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.filtType,    190, 20 +  r++ * lf,  70, lf, true);
  err |= pan->addTextItem(1330,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.preTrigger,   190, 20 +  r++ * lf,  80, lf, true);
  err |= pan->addTextItem(1144,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.deadTime,     190, 20 +  r++ * lf,  20, lf, true);
  err |= pan->addTextItem(1145,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.recFmt,      190, 20 +  r++ * lf,  40, lf, true);
  err |= pan->addTextItem(1190,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.projectType, 190, 20 +  r++ * lf,  80, lf, true);
  err |= pan->addTextItem(25,                    15, 20 +  r   * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.recAuto,     190, 20 +  r++ * lf,  80, lf, true, fuCalcSunrise);
  err |= pan->addTextItem(1180,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.startH,       190, 20 +  r   * lf,  16, lf, true);
  err |= pan->addTextItem(1182,                 206, 20 +  r   * lf,   5, lf);
  err |= pan->addNumItem(&devPars.startMin,     215, 20 +  r++ * lf,  16, lf, true);
  err |= pan->addTextItem(1181,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.stopH,        190, 20 +  r   * lf,  16, lf, true);
  err |= pan->addTextItem(1182,                 206, 20 +  r   * lf,   5, lf);
  err |= pan->addNumItem(&devPars.stopMin,      215, 20 +  r++ * lf,  16, lf, true);
  return err;
}


int MEMP initParPan(cPanel* pan, tCoord lf)
{
  int  err = 0;
  err |= pan->addTextItem(1100,                  15, 20,            80, lf);
  err |= pan->addEnumItem(&devPars.lang,        170, 20,           100, lf, true, languageFunc);
  err |= pan->addTextItem(1710,                  15, 20 +  1 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.menueType,   170, 20 +  1 * lf, 100, lf, true, fuMenuMode);
  err |= pan->addTextItem(1148,                  15, 20 +  2 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.backLightTime,170, 20 +  2 * lf,  25, lf, true);
  err |= pan->addTextItem(1320,                  15, 20 +  3 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.preAmpType,  170, 20 +  3 * lf,  80, lf, true);
  err |= pan->addTextItem(1325,                  15, 20 +  4 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.preAmpGain,  170, 20 +  4 * lf,  80, lf, true);
  err |= pan->addTextItem(1150,                  15, 20 +  5 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.knobRotation,170, 20 +  5 * lf, 100, lf, true);
  err |= pan->addTextItem(1160,                  15, 20 +  6 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.dispOrient,  170, 20 +  6 * lf,  80, lf, true);
  err |= pan->addTextItem(1165,                  15, 20 +  7 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.setVoltage, 170, 20 +  7 * lf,  80, lf, true, voltageFunc);
  err |= pan->addTextItem(1167,                  15, 20 +  8 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.sendDelay,    170, 20 +  8 * lf,  80, lf, true);

  return err;
}



void MEMP setTimeFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
    rtc.setTime(
    devStatus.year.get(), devStatus.month.get(), devStatus.day.get(),
    devStatus.hour.get(), devStatus.minute.get(), 0
    );
}

int MEMP initDateTimePan(cPanel* pan, tCoord lf)
{
  int err = pan->addTextItem(1142,                 120, 20 + 4 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.day,        155, 20 + 4 * lf,  20, lf, true);
  err |= pan->addNumItem(&devStatus.month,      180, 20 + 4 * lf,  20, lf, true);
  err |= pan->addNumItem(&devStatus.year,       205, 20 + 4 * lf,  30, lf, true);
  err |= pan->addTextItem(1143,                 120, 20 + 5 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.hour,       155, 20 + 5 * lf,  20, lf, true);
  err |= pan->addNumItem(&devStatus.minute,     180, 20 + 5 * lf,  20, lf, true);
  err |= pan->addBtnItem(1032,                  120, 20 + 7 * lf,  50, lf + 3, setTimeFunc);
  return err;
}

void MEMP setPosFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
    float lat = devStatus.latDeg.get() + devStatus.latMin.get()/60.0 + devStatus.latSec.get()/60000.0;
    if(devStatus.latSign.get() == 1)
      lat = -lat;
    devStatus.geoPos.setLat(lat);
    float lon = devStatus.lonDeg.get() + devStatus.lonMin.get()/60.0 + devStatus.lonSec.get()/60000.0;
    if(devStatus.lonSign.get() == 1)
      lon = -lon;
    devStatus.geoPos.setLon(lon);
}

int MEMP initPositionPan(cPanel* pan, tCoord lf)
{
  int err = 0;
  err |= pan->addTextItem(1420,                    5, 20 + 2 * lf, 120, lf);
  err |= pan->addEnumItem(&devPars.srcPosition,  140, 20 + 2 * lf,  80, lf, true, setPosFunc);
  err |= pan->addEnumItem(&devStatus.latSign,    140, 20 + 4 * lf,  10, lf, true, setPosFunc);
  err |= pan->addNumItem(&devStatus.latDeg,      150, 20 + 4 * lf,  17, lf, true, setPosFunc);
  err |= pan->addTextItem(1340,                  175, 20 + 4 * lf,  10, lf);
  err |= pan->addNumItem(&devStatus.latMin,      185, 20 + 4 * lf,  17, lf, true, setPosFunc);
  err |= pan->addTextItem(1345,                  200, 20 + 4 * lf,  10, lf);
  err |= pan->addNumItem(&devStatus.latSec,      210, 20 + 4 * lf,  25, lf, true, setPosFunc);
  err |= pan->addEnumItem(&devStatus.lonSign,    140, 20 + 5 * lf,  10, lf, true, setPosFunc);
  err |= pan->addNumItem(&devStatus.lonDeg,      150, 20 + 5 * lf,  25, lf, true, setPosFunc);
  err |= pan->addTextItem(1340,                  175, 20 + 5 * lf,  10, lf);
  err |= pan->addNumItem(&devStatus.lonMin,      185, 20 + 5 * lf,  17, lf, true, setPosFunc);
  err |= pan->addTextItem(1345,                  200, 20 + 5 * lf,  10, lf);
  err |= pan->addNumItem(&devStatus.lonSec,      210, 20 + 5 * lf,  25, lf, true, setPosFunc);
  err |= pan->addTextItem(193,                    70, 20 + 7 * lf,  70, lf);
  err |= pan->addNumItem(&devStatus.height,      140, 20 + 7 * lf,  70, lf, true);

  return err;
}


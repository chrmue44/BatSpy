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
#include "pnlfilebrowser.h"
#include "cEeprom.h"

extern cRtc rtc;

bool checkTwilight(enRecAuto r)
{
  return (r == TWILIGHT_ALL) || (r == TWILIGHT_BATS) || (r == TWILIGHT_BIRDS);
}

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

void MEMP displayModeFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  setDisplayColorsInverse(devPars.displayMode.get());
  menue.refreshAll();
}

void MEMP voltageFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  float voltage = devStatus.setVoltage.get();
  float fact = calcVoltageFactor(voltage);
  devPars.voltFactor.set(fact);
  sysLog.logf("voltage factor: %2.4f; voltage: %2.3f", devPars.voltFactor.get(), voltage); 
}


void MEMP displayTestFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  testDisplay();
  pThis->refreshAll();
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
  if(checkTwilight((enRecAuto)devPars.recAuto.get()))
    calcSunrise();
  else
  {
    devPars.startH.set(readInt16FromEep(EEPADDR_START_H));
    devPars.startMin.set(readInt16FromEep(EEPADDR_START_MIN));
    devPars.stopH.set(readInt16FromEep(EEPADDR_STOP_H));
    devPars.stopMin.set(readInt16FromEep(EEPADDR_STOP_MIN));
  }
}

void MEMP fuMenuMode(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  enMenueType m = (enMenueType)devPars.menueType.get();
  f1MainItems.clear();
  f4MainItems.clear();
  switch (m)
  {
    case enMenueType::HANDHELD:
      initFunctionItemsHandheld();
      break;
    default:
    case enMenueType::COMPACT:
      initFunctionsCompact();
      break;
  }
}

int MEMP initParRec(cPanel* pan, tCoord lf)
{
  int  err = 0;
  int r = 1;
  err |= pan->addTextItem(1110,                  15, 20            ,  80, lf);
  err |= pan->addEnumItem(&devPars.sampleRate[PARS_BAT],  190, 20            ,  30, lf, true);
  err |= pan->addTextItem(1120,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.recTime[PARS_BAT],      190, 20 +  r++ * lf,  30, lf, true);
  err |= pan->addTextItem(1140,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.recThreshhold[PARS_BAT],190, 20 +  r++ * lf,  30, lf, true);
  err |= pan->addTextItem(1360,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.triggerType[PARS_BAT], 190, 20 +  r++ * lf, 120, lf, true);
  err |= pan->addTextItem(1370,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.minEventLen[PARS_BAT],  190, 20 +  r++ * lf,  40, lf, true);
  err |= pan->addTextItem(1170,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.trigFiltFreq[PARS_BAT], 190, 20 +  r++ * lf,  30, lf, true);
  err |= pan->addTextItem(1175,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.trigFiltType[PARS_BAT],190, 20 +  r++ * lf,  70, lf, true);
  err |= pan->addTextItem(1330,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.preTrigger[PARS_BAT],   190, 20 +  r++ * lf,  80, lf, true);
  err |= pan->addTextItem(1144,                  15, 20 +  r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.deadTime[PARS_BAT],     190, 20 +  r++ * lf,  20, lf, true);
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

int MEMP initParRecCompact(cPanel* pan, tCoord lf, size_t parSet)
{
  int  err = 0;
  int r = 2;
  int x = 95;
  err |= pan->addTextItem(1120,                  1,      r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.recTime[parSet],       x,      r++ * lf,  30, lf, true);
  err |= pan->addTextItem(1110,                  1,      r   * lf,   x, lf);
  err |= pan->addEnumItem(&devPars.sampleRate[parSet],   x,      r++ * lf,  30, lf, true);
  err |= pan->addTextItem(1325,                  1,      r   * lf,   x, lf);
  err |= pan->addEnumItem(&devPars.preAmpGain[parSet],   x - 20, r++ * lf,  45, lf, true);
  err |= pan->addTextItem(1177,                  1,      r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.recFiltFreq[parSet],   x - 20, r++ * lf,  30, lf, true);
  err |= pan->addTextItem(1175,                  1,      r   * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.recFiltType[parSet],  x - 20, r++ * lf,  50, lf, true);
  err |= pan->addTextItem(1145,                  1,      r   * lf,  80, lf);
  err |= pan->addNumItem(&devPars.deadTime[parSet],      x - 20, r++ * lf,  20, lf, true);

  return err;
}

int MEMP initParTriggerCompact(cPanel* pan, tCoord lf, size_t parSet)
{
  int x = 80;
  int err = 0;
  int r = 2;
  err |= pan->addTextItem(1359,                  1,      r   * lf, 80, lf);
  err |= pan->addEnumItem(&devPars.triggerType[parSet],  x - 30, r++ * lf, 72, lf, true);
  err |= pan->addTextItem(1141,                  1,      r   * lf, 80, lf);
  err |= pan->addNumItem(&devPars.recThreshhold[parSet], x + 15, r++ * lf, 30, lf, true);
  err |= pan->addTextItem(1370,                  1,      r   * lf, 80, lf);
  err |= pan->addNumItem(&devPars.minEventLen[parSet],   x + 15, r++ * lf, 30, lf, true);
  err |= pan->addTextItem(1176,                  1,      r   * lf, 80, lf);
  err |= pan->addNumItem(&devPars.trigFiltFreq[parSet],  x + 15, r++ * lf, 30, lf, true);
  err |= pan->addTextItem(1178,                  1,      r   * lf, 80, lf);
  err |= pan->addEnumItem(&devPars.trigFiltType[parSet], x - 5, r++ * lf, 50, lf, true);
  err |= pan->addTextItem(1330,                  1,      r   * lf, 80, lf);
  err |= pan->addNumItem(&devPars.preTrigger[parSet],    x + 15, r++ * lf, 30, lf, true);
  return err;
}

int MEMP initParPan(cPanel* pan, tCoord lf)
{
  int  err = 0;
  err |= pan->addTextItem(1100,                    15, 20,            80, lf);
  err |= pan->addEnumItem(&devPars.lang,          170, 20,           100, lf, true, languageFunc);
  err |= pan->addTextItem(1710,                    15, 20 +  1 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.menueType,     170, 20 +  1 * lf, 100, lf, true, fuMenuMode);
  err |= pan->addTextItem(1148,                    15, 20 +  2 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.backLightTime,  170, 20 +  2 * lf,  25, lf, true);
  err |= pan->addTextItem(1150,                    15, 20 +  5 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.knobRotation,  170, 20 +  5 * lf, 100, lf, true);
  err |= pan->addTextItem(1160,                    15, 20 +  6 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.dispOrient,    170, 20 +  6 * lf,  80, lf, true);
  err |= pan->addTextItem(1165,                    15, 20 +  7 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.setVoltage,   170, 20 +  7 * lf,  80, lf, true, voltageFunc);
  err |= pan->addTextItem(1166,                    15, 20 +  8 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.ShutoffVoltage, 170, 20 +  8 * lf,  80, lf, true);
  err |= pan->addTextItem(1167,                    15, 20 +  9 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.sendDelay,      170, 20 +  9 * lf,  80, lf, true);

  return err;
}

int MEMP initParPanCompact(cPanel* pan, tCoord lf)
{
  int  err = 0;
  int x = 70;
  int r = 2;
  err |= pan->addTextItem(1100,                  1,      r   * lf,   x, lf);
  err |= pan->addEnumItem(&devPars.lang,         x,      r++ * lf,  50, lf, true, languageFunc);
  err |= pan->addTextItem(1155,                  1,      r   * lf,   x, lf);
  err |= pan->addEnumItem(&devPars.displayMode,  x,      r++ * lf,  50, lf, true, displayModeFunc);
  err |= pan->addTextItem(1148,                  1,      r   * lf,   x, lf);
  err |= pan->addNumItem(&devPars.backLightTime, x + 30, r++ * lf,  25, lf, true);
  err |= pan->addTextItem(1380,                  1,      r   * lf,   x, lf);
  err |= pan->addNumItem(&devPars.debugLevel,    x,      r++ * lf,  48, lf, true);
  err |= pan->addTextItem(1385,                  1,      r   * lf,   x, lf);
  err |= pan->addEnumItem(&devPars.gpsBaudRate,  x,      r++ * lf,  48, lf, true);
  err |= pan->addBtnItem(1390,                   1,  5 + r   * lf,  60, lf + 2, displayTestFunc);
  err |= pan->addBtnItem(5,                      64, 5 + r   * lf,  60, lf + 2, f2FormatFunc);

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

int MEMP initDateTimePanCompact(cPanel* pan, tCoord lf)
{
  int r = 1;
  int x = 60;
  int err = pan->addTextItem(1142,               1, 20 + r   * lf, 50, lf);
  err |= pan->addNumItem(&devStatus.day,         x, 20 + r   * lf, 13, lf, true);
  err |= pan->addTextItem(1345,                 74, 20 + r   * lf,  3, lf);
  err |= pan->addNumItem(&devStatus.month,      78, 20 + r   * lf, 13, lf, true);
  err |= pan->addTextItem(1345,                 91, 20 + r   * lf,  3, lf);
  err |= pan->addNumItem(&devStatus.year,       95, 20 + r++ * lf, 25, lf, true);
  err |= pan->addTextItem(1143,                  1, 20 + r   * lf, 13, lf);
  err |= pan->addNumItem(&devStatus.hour,        x, 20 + r   * lf, 13, lf, true);
  err |= pan->addTextItem(1182,                 74, 20 + r   * lf,  3, lf);
  err |= pan->addNumItem(&devStatus.minute,     78, 20 + r++ * lf, 13, lf, true);
  err |= pan->addTextItem(1720,                  1, 20 + r   * lf,  x, lf);
  err |= pan->addNumItem(&devPars.timeZone,      x, 20 + r++ * lf, 20, lf, true);
  err |= pan->addTextItem(1730,                  1, 20 + r   * lf,  x, lf);
  err |= pan->addEnumItem(&devPars.daylightSav,  x, 20 + r++ * lf, 60, lf, true);
  r++;
  err |= pan->addBtnItem(1032,                  40, 20 + r   * lf, 50, lf + 3, setTimeFunc);
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


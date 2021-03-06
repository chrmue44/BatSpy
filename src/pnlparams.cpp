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
  float fact = calcVoltageFactor(devStatus.voltage.get());
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


int MEMP initParRec(cPanel* pan, tCoord lf)
{
  int  err = 0;
  err |= pan->addTextItem(1110,                  15, 20          ,  80, lf);
  err |= pan->addEnumItem(&devPars.sampleRate,  190, 20          ,  30, lf, true);
  err |= pan->addTextItem(1120,                  15, 20 +  1 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.recTime,      190, 20 +  1 * lf,  30, lf, true);
  err |= pan->addTextItem(1140,                  15, 20 +  2 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.recThreshhold,190, 20 +  2 * lf,  30, lf, true);
  err |= pan->addTextItem(1170,                  15, 20 +  3 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.filtFreq,     190, 20 +  3 * lf,  30, lf, true);
  err |= pan->addTextItem(1175,                  15, 20 +  4 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.filtType,    190, 20 +  4 * lf,  70, lf, true);
  err |= pan->addTextItem(1330,                  15, 20 +  5 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.preTrigger,   190, 20 +  5 * lf,  80, lf, true);
  err |= pan->addTextItem(1144,                  15, 20 +  6 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.deafTime,     190, 20 +  6 * lf,  20, lf, true);
  err |= pan->addTextItem(1145,                  15, 20 +  7 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.recFmt,      190, 20 +  7 * lf,  40, lf, true);
  err |= pan->addTextItem(1190,                  15, 20 +  8 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.projectType, 190, 20 +  8 * lf,  80, lf, true);
  err |= pan->addTextItem(1180,                  15, 20 +  9 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.startH,       190, 20 +  9 * lf,  15, lf, true);
  err |= pan->addTextItem(1182,                 206, 20 +  9 * lf,   5, lf);
  err |= pan->addNumItem(&devPars.startMin,     215, 20 +  9 * lf,  15, lf, true);
  err |= pan->addTextItem(1181,                  15, 20 + 10 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.stopH,        190, 20 + 10 * lf,  15, lf, true);
  err |= pan->addTextItem(1182,                 206, 20 + 10 * lf,   5, lf);
  err |= pan->addNumItem(&devPars.stopMin,      215, 20 + 10 * lf,  15, lf, true);
  err |= pan->addTextItem(25,                    15, 20 + 11 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.recAuto,     190, 20 + 11 * lf,  80, lf, true, fuCalcSunrise);
  return err;
}


int MEMP initParPan(cPanel* pan, tCoord lf)
{
  int  err = 0;
  err |= pan->addTextItem(1100,                  15, 20,            80, lf);
  err |= pan->addEnumItem(&devPars.lang,        170, 20,           100, lf, true, languageFunc);
  err |= pan->addTextItem(1148,                  15, 20 +  1 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.backLightTime,170, 20 +  1 * lf,  20, lf, true);
  err |= pan->addTextItem(1320,                  15, 20 +  2 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.preAmpType,  170, 20 +  2 * lf,  80, lf, true);
  err |= pan->addTextItem(1325,                  15, 20 +  3 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.preAmpGain,  170, 20 +  3 * lf,  80, lf, true);
  err |= pan->addTextItem(1150,                  15, 20 +  4 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.knobRotation,170, 20 +  4 * lf, 100, lf, true);
  err |= pan->addTextItem(1160,                  15, 20 +  5 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.dispOrient,  170, 20 +  5 * lf,  80, lf, true);
  err |= pan->addTextItem(1165,                  15, 20 +  6 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.voltage,    170, 20 +  6 * lf,  80, lf, true, voltageFunc);
  err |= pan->addTextItem(1167,                  15, 20 +  7 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.sendDelay,    170, 20 +  7 * lf,  80, lf, true);

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
  int err = pan->addEnumItem(&devStatus.latSign, 120, 20 + 4 * lf,  10, lf, true, setPosFunc);
  err |= pan->addNumItem(&devStatus.latDeg,      130, 20 + 4 * lf,  17, lf, true, setPosFunc);
  err |= pan->addTextItem(1340,                  155, 20 + 4 * lf,  10, lf);
  err |= pan->addNumItem(&devStatus.latMin,      165, 20 + 4 * lf,  17, lf, true, setPosFunc);
  err |= pan->addTextItem(1345,                  180, 20 + 4 * lf,  10, lf);
  err |= pan->addNumItem(&devStatus.latSec,      190, 20 + 4 * lf,  25, lf, true, setPosFunc);
  err |= pan->addEnumItem(&devStatus.lonSign,    120, 20 + 5 * lf,  10, lf, true, setPosFunc);
  err |= pan->addNumItem(&devStatus.lonDeg,      130, 20 + 5 * lf,  25, lf, true, setPosFunc);
  err |= pan->addTextItem(1340,                  155, 20 + 5 * lf,  10, lf);
  err |= pan->addNumItem(&devStatus.lonMin,      165, 20 + 5 * lf,  17, lf, true, setPosFunc);
  err |= pan->addTextItem(1345,                  180, 20 + 5 * lf,  10, lf);
  err |= pan->addNumItem(&devStatus.lonSec,      190, 20 + 5 * lf,  25, lf, true, setPosFunc);

  return err;
}


#include "pnlparams.h"
#include "cRtc.h"

extern cRtc rtc;

void languageFunc(cMenuesystem* pThis, enKey key) {
  switch(devPars.lang.get()) {
    case 0:
      Txt::setLang(LANG_GER);
      break;
    case 1:
      Txt::setLang(LANG_EN);
      break;
  }
}

int initParPan(cPanel* pan, tCoord lf) {
  int  err = pan->addTextItem(1100,                  15, 20,            80, lf);
  err |= pan->addEnumItem(&devPars.lang,        170, 20,           100, lf, true, languageFunc);
  err |= pan->addTextItem(1110,                  15, 20 +      lf,  80, lf);
  err |= pan->addEnumItem(&devPars.sampleRate,  170, 20 +      lf,  30, lf, true);
  err |= pan->addTextItem(1120,                  15, 20 +  2 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.recTime,      170, 20 +  2 * lf,  30, lf, true);
  err |= pan->addTextItem(1140,                  15, 20 +  3 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.recThreshhold,170, 20 +  3 * lf,  30, lf, true);
  err |= pan->addTextItem(1330,                  15, 20 +  4 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.preTrigger,   170, 20 +  4 * lf,  80, lf, true);
  err |= pan->addTextItem(1144,                  15, 20 +  5 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.deafTime,     170, 20 +  5 * lf,  20, lf, true);
  err |= pan->addTextItem(1145,                  15, 20 +  6 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.recFmt,      170, 20 +  6 * lf,  20, lf, true);
  err |= pan->addTextItem(1148,                  15, 20 +  7 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.backLightTime,170, 20 +  7 * lf,  20, lf, true);
  err |= pan->addTextItem(1320,                  15, 20 +  8 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.preAmpType,  170, 20 +  8 * lf,  80, lf, true);
  err |= pan->addTextItem(1325,                  15, 20 +  9 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.preAmpGain,  170, 20 +  9 * lf,  80, lf, true);
  err |= pan->addTextItem(1150,                  15, 20 + 10 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.knobRotation,170, 20 + 10 * lf, 100, lf, true);
  err |= pan->addTextItem(1160,                  15, 20 + 11 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.dispOrient,  170, 20 + 11 * lf,  80, lf, true);
  return err;
}



void setTimeFunc(cMenuesystem* pThis, enKey key) {
#ifndef SIMU_DISPLAY
    rtc.setTime(
    devStatus.year.get(), devStatus.month.get(), devStatus.day.get(),
    devStatus.hour.get(), devStatus.minute.get(), 0
    );
#endif
}

int initDateTimePan(cPanel* pan, tCoord lf) {
  int err = pan->addTextItem(1142,                 120, 20 + 4 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.day,        155, 20 + 4 * lf,  20, lf, true);
  err |= pan->addNumItem(&devStatus.month,      180, 20 + 4 * lf,  20, lf, true);
  err |= pan->addNumItem(&devStatus.year,       205, 20 + 4 * lf,  30, lf, true);
  err |= pan->addTextItem(1143,                 120, 20 + 5 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.hour,       155, 20 + 5 * lf,  20, lf, true);
  err |= pan->addNumItem(&devStatus.minute,     180, 20 + 5 * lf,  20, lf, true);
  err |= pan->addBtnItem(devStatus.btnSetTime,  120, 20 + 7 * lf,  50, lf, setTimeFunc);
  return err;
}

void setPosFunc(cMenuesystem* pThis, enKey key) {
    float lat = devStatus.latDeg.get() + devStatus.latMin.get()/60.0 + devStatus.latSec.get()/60000.0;
    if(devStatus.latSign.get() == 1)
      lat = -lat;
    devStatus.geoPos.setLat(lat);
    float lon = devStatus.lonDeg.get() + devStatus.lonMin.get()/60.0 + devStatus.lonSec.get()/60000.0;
    if(devStatus.lonSign.get() == 1)
      lon = -lon;
    devStatus.geoPos.setLon(lon);
}

int initPositionPan(cPanel* pan, tCoord lf) {
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


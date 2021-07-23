#include "pnlparams.h"
#include "cRtc.h"
#include "debug.h"

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

void voltageFunc(cMenuesystem* pThis, enKey key) {
  float fact;
  int digits = analogRead(PIN_SUPPLY_VOLT);
  fact = (devStatus.voltage.get() - U_DIODE) / digits;
  DPRINTF2("digits: %i, voltage: %f, factor: %f\n", digits, devStatus.voltage.get(), fact);
  devPars.voltFactor.set(fact);
}

int initParRec(cPanel* pan, tCoord lf) {
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
  err |= pan->addTextItem(1180,                  15, 20 +  8 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.startH,       190, 20 +  8 * lf,  15, lf, true);
  err |= pan->addTextItem(1182,                 206, 20 +  8 * lf,   5, lf);
  err |= pan->addNumItem(&devPars.startMin,     215, 20 +  8 * lf,  15, lf, true);
  err |= pan->addTextItem(1181,                  15, 20 +  9 * lf,  80, lf);
  err |= pan->addNumItem(&devPars.stopH,        190, 20 +  9 * lf,  15, lf, true);
  err |= pan->addTextItem(1182,                 206, 20 +  9 * lf,   5, lf);
  err |= pan->addNumItem(&devPars.stopMin,      215, 20 +  9 * lf,  15, lf, true);
  err |= pan->addTextItem(25,                    15, 20 + 10 * lf,  80, lf);
  err |= pan->addEnumItem(&devPars.recAuto,     190, 20 + 10 * lf,  25, lf, true);
  return err;
}

int initParPan(cPanel* pan, tCoord lf) {
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


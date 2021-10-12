/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "pnlinfo.h"


void btnAudioFunc(cMenuesystem* pThis, enKey key, cParBase* pItem) {
  AudioProcessorUsageMaxReset();
}

int initInfoPan(cPanel* pan, tCoord lf) {
  int  err = pan->addTextItem(400,                 3,  30,            80, lf);
  err |= pan->addNumItem(&devStatus.cpuAudioAvg, 145,  30,            35, lf, false);
  err |= pan->addTextItem(401,                   185,  30,            80, lf);
  err |= pan->addTextItem(405,                     3,  30      + lf,  80, lf);
  err |= pan->addNumItem(&devStatus.cpuAudioMax, 145,  30      + lf,  35, lf, false);
  err |= pan->addTextItem(401,                   185,  30      + lf,  80, lf);
  err |= pan->addTextItem(410,                     3,  30 +  2 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.audioMem,    145,  30 +  2 * lf,  80, lf, false);
  err |= pan->addTextItem(415,                     3,  30 +  3 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.peakVal,     145,  30 +  3 * lf,  35, lf, false);
  err |= pan->addTextItem(401,                   185,  30 +  3 * lf,  80, lf);
  err |= pan->addBtnItem(309,                    145,  30 +  5 * lf,  90, lf + 3, btnAudioFunc);
  err |= pan->addTextItem(430,                     3,  30 +  7 * lf,  80, lf);
  err |= pan->addStrItem(&devStatus.version,     145,  30 +  7 * lf,  80, lf);
  err |= pan->addTextItem(440,                     3,  30 +  8 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.freeSpace,   145,  30 +  8 * lf,  80, lf, false);
  err |= pan->addTextItem(450,                     3,  30 +  9 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.voltage,     145,  30 +  9 * lf,  40, lf, false);
  err |= pan->addNumItem(&devStatus.digits,      245,  30 +  9 * lf,  40, lf, false);
  err |= pan->addTextItem(455,                     3,  30 + 10 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.temperature, 145,  30 + 10 * lf,  40, lf, false);

  return err;
}

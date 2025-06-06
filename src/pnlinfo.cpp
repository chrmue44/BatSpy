/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "pnlinfo.h"
#include "globals.h"

void btnAudioFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  AudioProcessorUsageMaxReset();
}

int MEMP initInfoPanExpert(cPanel* pan, tCoord lf)
{
  int  err = pan->addTextItem(400,                 3,  30,            80, lf);
  err |= pan->addNumItem(&devStatus.cpuAudioAvg, 145,  30,            35, lf, false);
  err |= pan->addTextItem(302,                   185,  30,            80, lf);
  err |= pan->addTextItem(405,                     3,  30      + lf,  80, lf);
  err |= pan->addNumItem(&devStatus.cpuAudioMax, 145,  30      + lf,  35, lf, false);
  err |= pan->addTextItem(302,                   185,  30      + lf,  80, lf);
  err |= pan->addTextItem(410,                     3,  30 +  2 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.audioMem,    145,  30 +  2 * lf,  80, lf, false);
  err |= pan->addTextItem(415,                     3,  30 +  3 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.peakVal,     145,  30 +  3 * lf,  35, lf, false);
  err |= pan->addTextItem(302,                   185,  30 +  3 * lf,  80, lf);
  err |= pan->addBtnItem(309,                    145,  30 +  5 * lf,  90, lf + 3, btnAudioFunc);
  err |= pan->addTextItem(435,                     3,  30 +  8 * lf,  80, lf);
  err |= pan->addStrItem(&devStatus.hwVersion,    145, 30 +  8 * lf,  80, lf);
  err |= pan->addTextItem(440,                     3,  30 +  9 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.freeSpace,   145,  30 +  9 * lf,  80, lf, false);
  err |= pan->addTextItem(450,                     3,  30 + 10 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.voltage,     145,  30 + 10 * lf,  40, lf, false);
  err |= pan->addNumItem(&devStatus.digits,      245,  30 + 10 * lf,  40, lf, false);
  err |= pan->addTextItem(455,                     3,  30 + 11 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.temperature, 145,  30 + 11 * lf,  40, lf, false);

  return err;
}

int MEMP initInfoPanCompact(cPanel* pan, tCoord lf)
{
  int r = 2;
  int x = 80;
  int err = 0;
  err |= pan->addTextItem(130,                   1,      r   * lf - 7, 45, lf);
  err |= pan->addStrItem(&devStatus.micId,      45,      r++ * lf - 7, 83, lf);
  err |= pan->addTextItem(131,                   1,      r   * lf - 7, 50, lf);
  err |= pan->addStrItem(&devStatus.micType,    45,      r++ * lf - 7, 83, lf);
  err |= pan->addTextItem(415,                   1,      r   * lf,  x, lf);
  err |= pan->addNumItem(&devStatus.peakVal,     x,      r   * lf, 20, lf, false);
  err |= pan->addTextItem(302,                   x + 30, r++ * lf, 15, lf);
  //  err |= pan->addTextItem(435,                   1,      r++ * lf,  x, lf);
//  err |= pan->addStrItem(&devStatus.hwVersion,   1,      r++ * lf,  x, lf);
  err |= pan->addTextItem(455,                   1,      r   * lf,  x, lf);
  err |= pan->addNumItem(&devStatus.temperature, x,      r   * lf, 20, lf, false);
  err |= pan->addTextItem(303,                   x + 30, r++ * lf, 15, lf);
  err |= pan->addTextItem(456,                   1,      r   * lf,  x, lf);
  err |= pan->addNumItem(&devStatus.humidity,    x,      r   * lf, 30, lf, false);
  err |= pan->addTextItem(302,                   x + 30, r++ * lf, 15, lf);
  err |= pan->addTextItem(405,                   1,      r   * lf, 80, lf);
  err |= pan->addNumItem(&devStatus.cpuAudioMax, x,      r++ * lf, 35, lf, false);
  err |= pan->addTextItem(30,                    1,      r * lf, 70, lf);
  err |= pan->addEnumItem(&devStatus.gpsStatus,  x,      r++ * lf, 35, lf, false);
  err |= pan->addTextItem(461,                   1,      r * lf, 23, lf);
  err |= pan->addNumItem(&devStatus.satCount,    x,      r++ * lf, 15, lf, false);
  err |= pan->addBtnItem(309,                   20,      r * lf, 90, lf + 3, btnAudioFunc);


  return err;
}

int MEMP initInfoPanRecorder(cPanel* pan, tCoord lf)
{
  int err = pan->addTextItem(430, 3,                   30 + 4 * lf, 80, lf);
  err |= pan->addStrItem(&devStatus.version, 145,   30 + 4 * lf, 80, lf);
  err |= pan->addTextItem(435, 3,                   30 + 5 * lf, 80, lf);
  err |= pan->addStrItem(&devStatus.hwVersion, 145, 30 + 5 * lf, 80, lf);
  err |= pan->addTextItem(440, 3,                   30 + 6 * lf, 80, lf);
  err |= pan->addNumItem(&devStatus.freeSpace, 145, 30 + 6 * lf, 80, lf, false);
  return err;
}

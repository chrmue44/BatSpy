#include "pnlinfo.h"


void btnAudioFunc(cMenuesystem* pThis, enKey key) {
#ifndef SIMU_DISPLAY
  AudioProcessorUsageMaxReset();
#endif
}

int initInfoPan(cPanel* pan, tCoord lf) {
  int  err = pan->addTextItem(400,                 3,  30,           80, lf);
  err |= pan->addNumItem(&devStatus.cpuAudioAvg, 140,  30,           20, lf, false);
  err |= pan->addTextItem(401,                   180,  30,           80, lf);
  err |= pan->addTextItem(405,                     3,  30     + lf,  80, lf);
  err |= pan->addNumItem(&devStatus.cpuAudioMax, 140,  30     + lf,  20, lf, false);
  err |= pan->addTextItem(401,                   180,  30     + lf,  80, lf);
  err |= pan->addTextItem(410,                     3,  30 + 2 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.audioMem,    140,  30 + 2 * lf,  80, lf, false);
  err |= pan->addTextItem(415,                     3,  30 + 3 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.peakVal,     140,  30 + 3 * lf,  30, lf, false);
  err |= pan->addTextItem(401,                   180,  30 + 3 * lf,  80, lf);
  err |= pan->addBtnItem(devStatus.btnAudio,     140,  30 + 5 * lf,  90, lf, btnAudioFunc);
  err |= pan->addTextItem(430,                     3,  30 + 7 * lf,  80, lf);
  err |= pan->addStrItem(&devStatus.version,     140,  30 + 7 * lf,  80, lf);
  err |= pan->addTextItem(440,                     3,  30 + 8 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.freeSpace,   140,  30 + 8 * lf,  80, lf, false);
  err |= pan->addTextItem(450,                     3,  30 + 9 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.voltage,     140,  30 + 9 * lf,  40, lf, false);
  err |= pan->addNumItem(&devStatus.digits,      240,  30 + 9 * lf,  40, lf, false);
  return err;
}

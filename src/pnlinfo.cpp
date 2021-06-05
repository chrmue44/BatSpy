#include "pnlinfo.h"


void btnAudioFunc(cMenuesystem* pThis, enKey key) {
#ifndef SIMU_DISPLAY
  AudioProcessorUsageMaxReset();
#endif
}

int initInfoPan(cPanel* pan, tCoord lf) {
  int  err = pan->addTextItem(400,                 3,  30,           80, lf);
  err |= pan->addNumItem(&devStatus.cpuAudioAvg, 140,  30,           20, lf, false);
  err |= pan->addTextItem(401,                   172,  30,           80, lf);
  err |= pan->addTextItem(405,                     3,  30     + lf,  80, lf);
  err |= pan->addNumItem(&devStatus.cpuAudioMax, 140,  30     + lf,  20, lf, false);
  err |= pan->addTextItem(401,                   172,  30     + lf,  80, lf);
  err |= pan->addTextItem(410,                     3,  30 + 2 * lf,  80, lf);
  err |= pan->addNumItem(&devStatus.audioMem,    140,  30 + 2 * lf,  80, lf, false);
  err |= pan->addBtnItem(devStatus.btnAudio,     140,  30 + 4 * lf,  90, lf, btnAudioFunc);
  return err;
}

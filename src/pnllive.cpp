/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "pnllive.h"
#include "pnlmain.h"
#include "globals.h"

cParGraph* getLiveFft()
{
 return reinterpret_cast<cParGraph*>(menue.getPan(pnlLive)->itemList[4].p); 
}


void MEMP setSampleRateLivePan(size_t parSet)
{
  cParGraph* g = getLiveFft();
  enSampleRate idx = (enSampleRate)devPars.sampleRate[parSet].get();
  g->setSampleRate(cAudio::getSampleRateHz(idx));
  devStatus.freqMax.set(g->getMaxFreq(cAudio::getFftOutputSize()) / 1000);
  devStatus.freq1Tick.set(g->getMaxFreq(cAudio::getFftOutputSize())/ 1000 / Y_TICK_CNT);
}

void MEMP funcFmax(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  cParGraph* g = getLiveFft();
  int sq = g->getSqueeze();
  size_t parSet = PARS_BAT; //TODO @@@
  switch (key) {
     case enKey::UP:
        g->setSqueeze(sq + 1, cAudio::getFftOutputSize());
        setSampleRateLivePan(parSet);
        break;

      case enKey::DOWN:
        g->setSqueeze(sq - 1, cAudio::getFftOutputSize());
        setSampleRateLivePan(parSet);
        break;

      default:
        break;
  }

}

int MEMP initLivePanHandheld(cPanel* pan, tCoord lf)
{
  int h = 128;
  int p1 = 32;
  int p2 = 18;
  int err = pan->addNumItem(&devStatus.freqMax,     5,  p2,                25, lf, true, funcFmax);
     err |= pan->addTextItem(300,                   5,  p2 + 1 * lf,       25, lf);
     err |= pan->addNumItem(&devStatus.freqMin,     5,  p2 + 128,          25, lf, false);
     err |= pan->addNumItem(&devStatus.freq1Tick,   5,  p2 + 128 - h/Y_TICK_CNT, 30, lf, false);
     err |= pan->addGraphItem(&devStatus.grafLive, 38,  p2 + 5,           280, h);
     err |= pan->addListItem(&devStatus.liveMsPerDiv, 250,  p2 + h + lf - 3,   65, lf, true);
     err |= pan->addTextItem(475,                 190,  p1 + h + 1 * lf,   80, lf);
     err |= pan->addNumItem(&devPars.liveAmplitude,280, p1 + h + 1 * lf,   25, lf, true);
     err |= pan->addTextItem(203,                   5,  p1 + h + 2 * lf,   80, lf);
     err |= pan->addNumItem(&devPars.mixFreq,     125,  p1 + h + 2 * lf,   15, lf, true);
     err |= pan->addTextItem(204,                 190,  p1 + h + 2 * lf,   80, lf);
     err |= pan->addNumItem(&devPars.volume,      280,  p1 + h + 2 * lf,   20, lf, true);
     err |= pan->addTextItem(1325,                  5,  p1 + h + 3 * lf,   80, lf);
     err |= pan->addEnumItem(&devPars.preAmpGain, 125,  p1 + h + 3 * lf,   33, lf, true);
     err |= pan->addTextItem(1338,                190,  p1 + h + 4 * lf,   80, lf);
     err |= pan->addNumItem(&devStatus.lastCallF, 280,  p1 + h + 4 * lf,   40, lf, false);

  return err;
}


int MEMP initLivePanRecorder(cPanel* pan, tCoord lf)
{
  int h = 128;
  int p1 = 32;
  int p2 = 18;
  int err = pan->addNumItem(&devStatus.freqMax,      5, p2, 25, lf, true, funcFmax);
  err |= pan->addTextItem(300,                       5, p2 + 1 * lf, 25, lf);
  err |= pan->addNumItem(&devStatus.freqMin,         5, p2 + 128, 25, lf, false);
  err |= pan->addNumItem(&devStatus.freq1Tick,       5, p2 + 128 - h / Y_TICK_CNT, 30, lf, false);
  err |= pan->addGraphItem(&devStatus.grafLive,     38, p2 + 5, 280, h);
  err |= pan->addListItem(&devStatus.liveMsPerDiv, 250, p2 + h + lf - 3, 65, lf, true);

  err |= pan->addTextItem(475,                       5, p1 + h + 1 * lf, 80, lf);
  err |= pan->addNumItem(&devPars.liveAmplitude,   125, p1 + h + 1 * lf, 25, lf, true);
  err |= pan->addTextItem(1325,                      5, p1 + h + 2 * lf, 80, lf);
  err |= pan->addEnumItem(&devPars.preAmpGain,     125, p1 + h + 2 * lf, 33, lf, true);
  err |= pan->addTextItem(1338,                    190, p1 + h + 4 * lf, 80, lf);
  err |= pan->addNumItem(&devStatus.lastCallF,     280, p1 + h + 4 * lf, 40, lf, false);

  return err;
}

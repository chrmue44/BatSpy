#include "pnllive.h"
#include "pnlmain.h"
#include "cAudio.h"

cParGraph* getLiveFft()
{
 return reinterpret_cast<cParGraph*>(menue.getPan(pnlLive)->itemList[4].p); 
}


void setSampleRateLivePan()
{
  cParGraph* g = getLiveFft();
  enSampleRate idx = (enSampleRate)devPars.sampleRate.get();
  g->setSampleRate(cAudio::getSampleRateHz(idx));
  devStatus.freqMax.set(g->getMaxFreq(cAudio::getFftOutputSize()) / 1000);
  devStatus.freq1Tick.set(g->getMaxFreq(cAudio::getFftOutputSize())/ 1000 / Y_TICK_CNT);
}

void funcFmax(cMenuesystem* pThis, enKey key)
{
  cParGraph* g = getLiveFft();
  int sq = g->getSqueeze();
  switch (key) {
     case enKey::UP:
        g->setSqueeze(sq + 1, cAudio::getFftOutputSize());
        setSampleRateLivePan();
        break;

      case enKey::DOWN:
        g->setSqueeze(sq - 1, cAudio::getFftOutputSize());
        setSampleRateLivePan();
        break;

      default:
        break;
  }

}

int initLivePan(cPanel* pan, tCoord lf)
{
  int h = 128;
  int p = 30;
  int err = pan->addNumItem(&devStatus.freqMax,     5,  20,                25, lf, true, funcFmax);
     err |= pan->addTextItem(300,                   5,  20 + 1 * lf,       25, lf);
     err |= pan->addNumItem(&devStatus.freqMin,     5, 148,                25, lf, false);
     err |= pan->addNumItem(&devStatus.freq1Tick,   5, 148 - h/Y_TICK_CNT, 30, lf, false);
     err |= pan->addGraphItem(&devStatus.grafLive, 38,  25,               280, h);

     err |= pan->addTextItem(470,                   5,  p + h + 1 * lf,   80, lf);
     err |= pan->addNumItem(&devPars.sweepSpeed,  125,  p + h + 1 * lf,   25, lf, true);
     err |= pan->addTextItem(475,                 190,  p + h + 1 * lf,   80, lf);
     err |= pan->addNumItem(&devPars.liveAmplitude,280, p + h + 1 * lf,   25, lf, true);
     err |= pan->addTextItem(202,                   5,  p + h + 2 * lf,   80, lf);
     err |= pan->addEnumItem(&devStatus.opMode,   125,  p + h + 2 * lf,  140, lf, true);
     err |= pan->addTextItem(203,                   5,  p + h + 3 * lf,   80, lf);
     err |= pan->addNumItem(&devPars.mixFreq,     125,  p + h + 3 * lf,   15, lf, true);
     err |= pan->addTextItem(204,                 190,  p + h + 3 * lf,   80, lf);
     err |= pan->addNumItem(&devPars.volume,      280,  p + h + 3 * lf,   20, lf, true);
     err |= pan->addTextItem(1320,                  5,  p + h + 4 * lf,   80, lf);
     err |= pan->addEnumItem(&devPars.preAmpType, 125,  p + h + 4 * lf,   60, lf, true);
     err |= pan->addTextItem(1325,                190,  p + h + 4 * lf,   80, lf);
     err |= pan->addEnumItem(&devPars.preAmpGain, 280,  p + h + 4 * lf,   33, lf, true);

  return err;
}

#include "pnlmain.h"
#include "cutils.h"

void f2WaterFunc(cMenuesystem* pThis, enKey key) {
  if(devStatus.fileIndex > 0)
    devStatus.fileIndex--;
  while(devStatus.fileIndex >= 0 )
  {
    char ext[8];
    if(!devStatus.dir[devStatus.fileIndex].isDir)
    {
      char* pName = devStatus.dir[devStatus.fileIndex].name;
      cUtils::getExtension(pName, ext, sizeof(ext));
      if((strcmp(ext,"RAW") == 0) || (strcmp(ext,"raw") == 0))  {
        char buf[FILENAME_LEN];
        strncpy(buf, cSdCard::inst().getActDir(), sizeof(buf));
        strcat(buf,"/");
        strcat(buf, pName);
        devPars.fileName.set(buf);

        setFileToDisplay(buf);

        devStatus.graph.initPlot(true);
        devStatus.waterf.initPlot(true);
        pThis->refreshMainPanel();
        pThis->refreshHdrPanel();
        break;
      }
    }
    if(devStatus.fileIndex > 0)
      devStatus.fileIndex--;
    else
      break;
  }
}

void f3WaterFunc(cMenuesystem* pThis, enKey key) {
  devStatus.fileIndex++;
  while(devStatus.fileIndex < devStatus.dir.size())
  {
    char ext[8];
    if(!devStatus.dir[devStatus.fileIndex].isDir)
    {
      char* pName = devStatus.dir[devStatus.fileIndex].name;
      cUtils::getExtension(pName, ext, sizeof(ext));
      if((strcmp(ext,"RAW") == 0) || (strcmp(ext,"raw") == 0))  {
        char buf[FILENAME_LEN];
        strncpy(buf, cSdCard::inst().getActDir(), sizeof(buf));
        strcat(buf,"/");
        strcat(buf, pName);
        devPars.fileName.set(buf);

        setFileToDisplay(buf);

        devStatus.graph.initPlot(true);
        devStatus.waterf.initPlot(true);
        pThis->refreshMainPanel();
        pThis->refreshHdrPanel();
        break;
      }
    }
    devStatus.fileIndex++;
  }
}


int initFkeysWaterPan(cPanel* pan, tCoord lf) {
  int err  = pan->addTextItem(  1,   3, 229, 75, lf - 1, true, f1Func);
  err |= pan->addTextItem(310,  83, 229, 75, lf - 1, true, f2WaterFunc);
  err |= pan->addTextItem(311, 163, 229, 75, lf - 1, true, f3WaterFunc);
  err |= pan->addTextItem(  4, 243, 229, 75, lf - 1, true, f4Func);
  return err;
}


void fftLevelFunc(cMenuesystem* pThis, enKey key) {
  devStatus.waterf.setFftLevels(devPars.fftLevelMin.get(), devPars.fftLevelMax.get());
  devStatus.waterf.initPlot(true);
}

void panWaterZoomFunc(cMenuesystem* pThis, enKey key) {
  switch (key) {
    case enKey::UP:
    case enKey::DOWN:
      devStatus.timMax.set(devStatus.timMin.get() + GRAPH_DIVX_COUNT * devStatus.timStep.get());
      if (devStatus.timMax.get() > devStatus.timMax.getMax())
        devStatus.timMax.set(devStatus.timMax.getMax());
      devStatus.graph.setPlotBorders(devStatus.timMin.get(), devStatus.timMax.get());
      devStatus.graph.initPlot(true);
      devStatus.waterf.setPlotBorders(devStatus.timMin.get(), devStatus.timMax.get());
      devStatus.waterf.initPlot(true);
      pThis->refreshMainPanel();
      break;
     
    default:
      break;

  }
}

const float stepTab[] = {0.00001, 0.00002, 0.00005, 0.0001, 0.0002, 0.0005, 0.001, 0.002, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1, 2};

void timeStepFunc(cMenuesystem* pThis, enKey key) {
  float step = devStatus.timStep.get();
  size_t i;
  for(i = 0; i < (sizeof(stepTab)/sizeof(stepTab[0])); i++) {
    if(step == stepTab[i])
      break;
  }

  switch (key) {
    case enKey::UP:
      if (i > 0) {
        float val = stepTab[i - 1];
        devStatus.timStep.set(val);
      }
      break;

    case enKey::DOWN:
      if (i < (sizeof(stepTab)/sizeof(stepTab[0]) - 1))
        devStatus.timStep.set(stepTab[i + 1]);
      break;

    default:
      break;
  }

  devStatus.timMin.init(0, 20, 2 * devStatus.timStep.get(), DEC_TIM_MIN);
  devStatus.timMax.set(devStatus.timMin.get() + devStatus.timStep.get() * GRAPH_DIVX_COUNT);
  devStatus.graph.setPlotBorders(devStatus.timMin.get(), devStatus.timMax.get());
  devStatus.waterf.setPlotBorders(devStatus.timMin.get(), devStatus.timMax.get());
  devStatus.graph.initPlot(true);
  devStatus.waterf.initPlot(true);

  pThis->refreshMainPanel();
}



int initWaterPan(cPanel* pan, tCoord lf) {
  int height = 128;
  int err = pan->addTextItem(300,               15, 80,            25, lf);
  err |= pan->addNumItem(&devStatus.freqMax,    10, 20,            25, lf, false);
  err |= pan->addNumItem(&devStatus.freqMin,    10, 148,           25, lf, false);
  err |= pan->addNumItem(&devPars.fftLevelMin,  50,153+COLMAP_DIST,35,lf, true, fftLevelFunc);
  err |= pan->addNumItem(&devPars.fftLevelMax, 270,153+COLMAP_DIST,35, lf, true, fftLevelFunc);
  err |= pan->addNumItem(&devStatus.timMin,     30, 213,           53, lf, true, panWaterZoomFunc);
  err |= pan->addTextItem(301,                  85, 213,           25, lf, false);
  err |= pan->addNumItem(&devStatus.timStep,   128, 213,           52, lf, true, timeStepFunc);
  err |= pan->addTextItem(305,                 182, 213,           35, lf, false);
  err |= pan->addNumItem(&devStatus.timMax,    255, 213,           53, lf, false);
  err |= pan->addTextItem(301,                 310, 213,           25, lf, false);
  err |= pan->addNumItem(&devStatus.freq1Tick,  10, 148 - height/Y_TICK_CNT, 25, lf, false);
  err |= pan->addGraphItem(&devStatus.waterf,   40,  25,          261, height);
  return err;
}


const float ampStepTab[] = {1, 2, 5, 10, 20, 50, 100};

void amplitudeFunc(cMenuesystem* pThis, enKey key) {
  float ampl = devStatus.amplMax.get();
  size_t i;
  for(i = 0; i < (sizeof(ampStepTab)/sizeof(ampStepTab[0])); i++) {
    if(ampl == ampStepTab[i])
      break;
  }

  switch (key) {
    case enKey::UP:
      if (i > 0) {
        float val = ampStepTab[i - 1];
        devStatus.amplMax.set(val);
      }
      break;

    case enKey::DOWN:
      if (i < (sizeof(ampStepTab)/sizeof(ampStepTab[0]) - 1))
        devStatus.amplMax.set(ampStepTab[i + 1]);
      break;

    default:
      break;
  }
  devStatus.amplMin.set(devStatus.amplMax.get() * -1.0);
  devStatus.graph.initPlot(true);
  devStatus.graph.setAmplitude(devStatus.amplMax.get());
  pThis->refreshMainPanel();
}

void btnMeasFunc(cMenuesystem* pThis, enKey key) {
  int32_t level = devPars.threshHold.get() * MAX_ADC / 100;
  if(devStatus.graph.isMinMax()) {
    float t = devStatus.graph.measure(level) * 1000;
    devStatus.pulseWidth.set(t);
    devStatus.measUnit.set("ms");
  }
  else {
    float f = devStatus.graph.measure(level) / 1000;
    devStatus.pulseWidth.set(f);
    devStatus.measUnit.set("kHz");
  }
}

int initTimePan(cPanel* pan, tCoord lf) {
  int err = pan->addNumItem(&devStatus.amplMax,       5,  20,           20, lf, true, amplitudeFunc);
  err |= pan->addTextItem(302,                    25,  20,            5, lf);
  err |= pan->addTextItem(306,                     1,  20 +  2 * lf, 23, lf);
  err |= pan->addNumItem(&devPars.threshHold,      1,  20 +  3 * lf, 18, lf, true);
  err |= pan->addTextItem(307,                    19,  20 +  3 * lf,  8, lf);
  err |= pan->addBtnItem(devStatus.btnMeas,        1,  20 +  5 * lf, 30, lf, btnMeasFunc);
  err |= pan->addNumItem(&devStatus.pulseWidth,    1,  20 +  7 * lf, 30, lf, false);
  err |= pan->addStrItem(&devStatus.measUnit,      1,  20 +  8 * lf, 30, lf, false);
  err |= pan->addNumItem(&devStatus.amplMin,       1, 205,           24, lf, false);
  err |= pan->addTextItem(302,                    25, 205,            5, lf);
  err |= pan->addNumItem(&devStatus.timMin,       30, 213,           53, lf, true, panWaterZoomFunc);
  err |= pan->addTextItem(301,                    85, 213,           25, lf, false);
  err |= pan->addNumItem(&devStatus.timStep,     128, 213,           52, lf, true, timeStepFunc);
  err |= pan->addTextItem(305,                   182, 213,           35, lf, false);
  err |= pan->addNumItem(&devStatus.timMax,      255, 213,           53, lf, false);
  err |= pan->addTextItem(301,                   310, 213,           25, lf, false);
  err |= pan->addGraphItem(&devStatus.graph,      33,  25, 285, 185);
  return err;
}

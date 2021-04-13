#include <cstdlib>
#include <cstring>
#include "cmenue.h"
#include "ctext.h"
#include "cutils.h"
#include <ILI9341_t3.h>
#include "cRtc.h"
#include "debug.h"
#ifndef SIMU_DISPLAY
#include "cSdCard.h"
#include <EEPROM.h>
#include <Time.h>
#else
#include "simulation/cSdCard.h"
typedef std::size_t size_t;
#endif
#pragma GCC diagnostic ignored "-Wunused-parameter" //disable because fuFocus functions may not use the parameters

#define DEBUG_LEVEL  1

stStatus devStatus;          ///< status of the device
stParams devPars;            ///< parameters of the device
extern cRtc rtc;

// ***************************************************
// waterfall panel
thPanel fkeyWaterPan;   ///< f-key panel for waterfall screen
thPanel panWaterfall;
thPanel hdrPanWaterfall;

void f2WaterFunc(cMenuesystem* pThis, tKey key) {
  devStatus.fileIndex--;
  while(devStatus.fileIndex >= 0 )
  {
    char ext[8];
    if(!(*devStatus.pDir)[devStatus.fileIndex].isDir)
    {
      char* pName = (*devStatus.pDir)[devStatus.fileIndex].name;
      cUtils::getExtension(pName, ext, sizeof(ext));
      if((strcmp(ext,"RAW") == 0) || (strcmp(ext,"raw") == 0))  {
        char buf[128];
        strncpy(buf, cSdCard::inst().getActDir(), sizeof(buf));
        strcat(buf,"/");
        strcat(buf, pName);
        devPars.fileName.set(buf);
        devStatus.graph.setPlotFile(devPars.fileName.get(), devPars.sampleRate.get() * 1000);
        devStatus.waterf.setPlotFile(devPars.fileName.get(), devPars.sampleRate.get() * 1000);
        devStatus.graph.initPlot(true);
        devStatus.waterf.initPlot(true);
        pThis->refreshMainPanel();
        pThis->refreshHdrPanel();
        break;
      }
    }
    devStatus.fileIndex--;
  }
}

void f3WaterFunc(cMenuesystem* pThis, tKey key) {
  devStatus.fileIndex++;
  while(devStatus.fileIndex < devStatus.pDir->size())
  {
    char ext[8];
    if(!(*devStatus.pDir)[devStatus.fileIndex].isDir)
    {
      char* pName = (*devStatus.pDir)[devStatus.fileIndex].name;
      cUtils::getExtension(pName, ext, sizeof(ext));
      if((strcmp(ext,"RAW") == 0) || (strcmp(ext,"raw") == 0))  {
        char buf[128];
        strncpy(buf, cSdCard::inst().getActDir(), sizeof(buf));
        strcat(buf,"/");
        strcat(buf, pName);
        devPars.fileName.set(buf);
        devStatus.graph.setPlotFile(devPars.fileName.get(), devPars.sampleRate.get() * 1000);
        devStatus.waterf.setPlotFile(devPars.fileName.get(), devPars.sampleRate.get() * 1000);
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

const float ampStepTab[] = {1, 2, 5, 10, 20, 50, 100};

void amplitudeFunc(cMenuesystem* pThis, tKey key) {
  float ampl = devStatus.amplMax.get();
  size_t i;
  for(i = 0; i < (sizeof(ampStepTab)/sizeof(ampStepTab[0])); i++) {
    if(ampl == ampStepTab[i])
      break;
  }

  switch (key) {
    case DEV_KEY_UP:
      if (i > 0) {
        float val = ampStepTab[i - 1];
        devStatus.amplMax.set(val);
      }
      break;

    case DEV_KEY_DOWN:
      if (i < (sizeof(ampStepTab)/sizeof(ampStepTab[0]) - 1))
        devStatus.amplMax.set(ampStepTab[i + 1]);
      break;
  }
  devStatus.amplMin.set(devStatus.amplMax.get() * -1.0);
  devStatus.graph.initPlot(true);
  devStatus.graph.setAmplitude(devStatus.amplMax.get());
  pThis->refreshMainPanel();
}

const float stepTab[] = {0.00001, 0.00002, 0.00005, 0.0001, 0.0002, 0.0005, 0.001, 0.002, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1, 2};

void timeStepFunc(cMenuesystem* pThis, tKey key) {
  float step = devStatus.timStep.get();
  size_t i;
  for(i = 0; i < (sizeof(stepTab)/sizeof(stepTab[0])); i++) {
    if(step == stepTab[i])
      break;
  }

  switch (key) {
    case DEV_KEY_UP:
      if (i > 0) {
        float val = stepTab[i - 1];
        devStatus.timStep.set(val);
      }
      break;

    case DEV_KEY_DOWN:
      if (i < (sizeof(stepTab)/sizeof(stepTab[0]) - 1))
        devStatus.timStep.set(stepTab[i + 1]);
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

void panWaterZoomFunc(cMenuesystem* pThis, tKey key) {
  switch (key) {
    case DEV_KEY_UP:
    case DEV_KEY_DOWN:
      devStatus.timMax.set(devStatus.timMin.get() + GRAPH_DIVX_COUNT * devStatus.timStep.get());
      if (devStatus.timMax.get() > devStatus.timMax.getMax())
        devStatus.timMax.set(devStatus.timMax.getMax());
      devStatus.graph.setPlotBorders(devStatus.timMin.get(), devStatus.timMax.get());
      devStatus.graph.initPlot(true);
      devStatus.waterf.setPlotBorders(devStatus.timMin.get(), devStatus.timMax.get());
      devStatus.waterf.initPlot(true);
      pThis->refreshMainPanel();
      break;

  }
}

void dirFunc(cMenuesystem* pThis, tKey key) {
  enSdRes rc = OK;
  cSdCard& sd = cSdCard::inst();
  enFocusState state = pThis->getFocusState();
  // leaving dropdown
  if (pThis->isDropDownInFocus()) {
    DPRINTF2("change dir: %s\n", devPars.dirSel.getActText());
    sd.chdir(devPars.dirSel.getActText());
    devPars.fileName.set(sd.getActDir());
    devStatus.fileIndex = 1;
  }
  // before opening dropdown
  else {
    if ((state == FST_SELECT) && (key = DEV_KEY_OK)) {
      tDirInfo* p;
      DPRINTLN1("dirFunc"); //@@@
      rc = sd.dir(p);
      devPars.dirSel.clear();
      if (p && (rc == OK)) {
        tDirInfo& dir = *p;
        for (size_t i = 0; i < dir.size(); i++) {
          if (dir[i].isDir) {
            devPars.dirSel.addItem(dir[i].name);
            DPRINTF2("dir name: %s\n", dir[i].name);
          }
        }
      }
      DPRINTF2("devPars.dirSel.size: %u\n", devPars.dirSel.size());
    }
  }
}

void dispModeFunc(cMenuesystem* pThis, tKey key) {
  thPanel i = pThis->getMainPanel();
  cPanel* p = pThis->getPan(i);
  if(devStatus.opMode.get() == OPMODE_REC_AUTO)
    p->itemList[2].isVisible = true;
  else
    p->itemList[2].isVisible = false;
}

void fileFunc(cMenuesystem* pThis, tKey key) {
  enSdRes rc = OK;
  cSdCard& sd = cSdCard::inst();
  enFocusState state = pThis->getFocusState();
  if (pThis->isDropDownInFocus()) {
    char buf[FILENAME_LEN];
    strncpy(buf, sd.getActDir(), FILENAME_LEN);
    size_t len = strlen(buf);
    if (buf[len - 1] != '/')
      strcat(buf, "/");
    strcat(buf, devPars.fileSel.getActText());
    devPars.fileName.set(buf);
  }
  //
  else {
    if ((state == FST_SELECT) && (key = DEV_KEY_OK)) {
      tDirInfo* p;
      DPRINTLN1("fileFunc\n");

      rc = sd.dir(p);
      devPars.fileSel.clear();
      if (rc == 0) {
        tDirInfo& dir = *p;
        for (size_t i = 0; i < dir.size(); i++) {
          if (!dir[i].isDir)
            devPars.fileSel.addItem(dir[i].name);
        }
      }
    }
  }
  devStatus.graph.setPlotFile(devPars.fileName.get(), devPars.sampleRate.get() * 1000);
  devStatus.waterf.setPlotFile(devPars.fileName.get(), devPars.sampleRate.get() * 1000);
}

void batFunc(cMenuesystem* pThis, tKey key) {
  char line[512];
  char line2[512];
  size_t byteCount;
  cSdCard& sd = cSdCard::inst();
  tFILE file;
  enSdRes res = sd.openFile("/info/bat_info.tsv", file, READ);
  bool found = false;
  if(res == 0)
  {
    for(;;) {
      res = sd.readLine(file, line, sizeof(line), byteCount);
      line[byteCount] = 0;
      cUtils::replaceUTF8withInternalCoding(line, line2, sizeof(line2));   
      if(res == 0) {
        if(strstr(line2, devStatus.bats.name.getActText()) != NULL) {
          found = true;
          break;
        }
      }
      else
        break;
    }
    sd.closeFile(file);
    if(found)
    {
      char* token = strtok(line2, "\t");  //name
      token = strtok(NULL, "\t");
      devStatus.bats.nameLat.set(token);
      token = strtok(NULL, "\t");   //Kürzel
      token = strtok(NULL, "\t");   //vorkommen
      devStatus.bats.occurrence.set(token);
      token = strtok(NULL, "\t");   //Hauptfrequenz
      devStatus.bats.freq.set(token);
      token = strtok(NULL, "\t");   //Ruflaenge
      devStatus.bats.callLen.set(token);
      token = strtok(NULL, "\t");   //Rufabstand
      devStatus.bats.callDist.set(token);
      token = strtok(NULL, "\t");   //Characteristik
      devStatus.bats.characteristic.set(token);
      token = strtok(NULL, "\t");   //Comments
      devStatus.bats.comment.set(token);
      token = strtok(NULL, "\t");   //Skiba
      token = strtok(NULL, "\t");   //Bild
      token = strtok(NULL, "\t");   //Groesse
      devStatus.bats.size.set(token);
      token = strtok(NULL, "\t");   //Spannweite
      devStatus.bats.wingSpan.set(token);
      token = strtok(NULL, "\t");   //Gewicht
      devStatus.bats.weight.set(token);
      token = strtok(NULL, "\t");   //Lebensraum
      devStatus.bats.habitat.set(token);
    }
  }
}



thPanel panFont;
thPanel panTime;        ///< panel to display time diagram
thPanel panHisto;
thPanel panGeo;
thPanel panInfo;

// ***************************************************
// main panel
thPanel fkeyMainPan;    ///< f-key panel for main screen
thPanel f2pan;
thPanel f3pan;
thPanel f4MainPan;
thPanel hdrMainPanel;
cParEnum f1MainItems(0);
cParEnum f4MainItems(0);
thPanel panParams;      ///< panel for parameter settings
thPanel panBats;        ///< panel for bat infos
thPanel panDateTime;    ///< panel to set time and date

// *******************************************
// drop down panel F1 for main panel

void f1DropFunc(cMenuesystem* pThis, tKey key) {
  switch (pThis->getFocusItem()) {
    case 0:
      pThis->setMainPanel(panGeo);
      pThis->setHdrPanel(hdrMainPanel);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
    case 1:
      devStatus.graph.initPlot(true);
      pThis->setMainPanel(panTime);
      pThis->setHdrPanel(hdrPanWaterfall);;
      pThis->setFkeyPanel(fkeyWaterPan);
      break;
    case 2:
      devStatus.waterf.initPlot(true);
      pThis->setMainPanel(panWaterfall);
      pThis->setHdrPanel(hdrPanWaterfall);
      pThis->setFkeyPanel(fkeyWaterPan);
      break;
    case 3:
      pThis->setMainPanel(panHisto);
      pThis->setHdrPanel(hdrPanWaterfall);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
    case 4:
      pThis->setMainPanel(panBats);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
    case 5:
      pThis->setMainPanel(panFont);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
    case 6:
      pThis->setMainPanel(panInfo);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
  }
}

void f4LoadFunc(cMenuesystem* pThis, tKey key) {
  switch (key) {
    case DEV_KEY_YES:
      pThis->load();
      break;
    case DEV_KEY_NO:
      break;
  }
}

void f4SaveFunc(cMenuesystem* pThis, tKey key) {
  switch (key) {
    case DEV_KEY_YES:
      pThis->save();
      break;
    case DEV_KEY_NO:
      break;
  }
}

void f4DropFunc(cMenuesystem* pThis, tKey key) {
  switch (pThis->getFocusItem()) {
    case 0:
      pThis->showMsg(MSG_YESNO, f4LoadFunc, Txt::get(1005), Txt::get(1006));
      break;
    case 1:
      pThis->showMsg(MSG_YESNO, f4SaveFunc, Txt::get(1005), Txt::get(1025));
      break;
    case 2:
      pThis->setMainPanel(panParams);
      pThis->setHdrPanel(hdrMainPanel);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
    case 3:
      devStatus.year.set(devStatus.date.getYear());
      devStatus.month.set(devStatus.date.getMonth());
      devStatus.day.set(devStatus.date.getDay());
      devStatus.hour.set(devStatus.time.getHour());
      devStatus.minute.set(devStatus.time.getMin());
      pThis->setMainPanel(panDateTime);
      pThis->setHdrPanel(hdrMainPanel);
      pThis->setFkeyPanel(fkeyMainPan);
      break;
  }
}

void f1Func(cMenuesystem* pThis, tKey key) {
  stPanelItem item;
  item.type = ITEM_ENUM;
  item.p = &f1MainItems;
  item.f = NULL;
  pThis->createDropDown(item, 1, DISP_HEIGHT - f1MainItems.size() * LINE_HEIGHT - 15, 120, f1DropFunc);

}

void f2Func(cMenuesystem* pThis, tKey key) {
  if ((devStatus.opMode.get() == OPMODE_HEAR_HET) ||
      (devStatus.opMode.get() == OPMODE_REC_AUTO)) {
    if (devStatus.playStatus.get() == 0)
      devStatus.playStatus.set(2);
    else
      devStatus.playStatus.set(0);
  }
  else {
    if (devStatus.playStatus.get() == 0)
      devStatus.playStatus.set(1);
    else
      devStatus.playStatus.set(0);
  }
}

void f4Func(cMenuesystem* pThis, tKey key) {
  stPanelItem item;
  item.type = ITEM_ENUM;
  item.p = &f4MainItems;
  pThis->createDropDown(item, DISP_WIDTH - 120 - 1, DISP_HEIGHT - f4MainItems.size() * LINE_HEIGHT - 15, 120, f4DropFunc);
}


void graphFunc(cMenuesystem* pThis, tKey key) {
}


void languageFunc(cMenuesystem* pThis, tKey key) {
  switch(devPars.lang.get()) {
    case 0:
      Txt::setLang(LANG_GER);
      break;
    case 1:
      Txt::setLang(LANG_EN);
      break;
  }
}

void btnMeasFunc(cMenuesystem* pThis, tKey key) {
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


void btnAudioFunc(cMenuesystem* pThis, tKey key) {
#ifndef SIMU_DISPLAY
  AudioProcessorUsageMaxReset();
#endif
}

void fftLevelFunc(cMenuesystem* pThis, tKey key) {
  devStatus.waterf.setFftLevels(devPars.fftLevelMin.get(), devPars.fftLevelMax.get());
  devStatus.waterf.initPlot(true);
}

void setTimeFunc(cMenuesystem* pThis, tKey key) {
#ifndef SIMU_DISPLAY
    rtc.setTime(
    devStatus.year.get(), devStatus.month.get(), devStatus.day.get(),
    devStatus.hour.get(), devStatus.minute.get(), 0
    );
#endif
}

// *********************************************

cMenue::cMenue(int width, int height, ILI9341_t3* pDisplay) :
  cMenuesystem(width, height, pDisplay) {
}


cMenue::~cMenue() {
}




void cMenue::initPars() {

  devPars.preAmpGain.addItem(1326);
  devPars.preAmpGain.addItem(1327);
  devPars.preAmpGain.addItem(1328);

  devPars.preAmpType.addItem(1321);
  devPars.preAmpType.addItem(1322);
  
  devPars.lang.clear();
  devPars.lang.addItem(1101);
  devPars.lang.addItem(1102);
  loadLanguage();
  devStatus.opMode.clear();
  devStatus.opMode.addItem(20);
  devStatus.opMode.addItem(21);
  devStatus.opMode.addItem(22);
  devStatus.opMode.addItem(23);
  devStatus.opMode.addItem(24);
  devStatus.opMode.addItem(25);

  devPars.mixFreq.init(1,20, 1, 0);

  devPars.volume.init(-30,15, 3, 0);
  devPars.volume.set(-18);

  devStatus.playStatus.clear();
  devStatus.playStatus.addItem(320);
  devStatus.playStatus.addItem(325);
  devStatus.playStatus.addItem(335);
  devStatus.playStatus.addItem(337);

  devStatus.timStep.init(0, 10, 0.0, DEC_TIM_MIN);
  devStatus.timMin.init(0, 10, devStatus.timStep.get() * 2, DEC_TIM_MIN);
  devStatus.timMax.init(0, 12, 0.4, DEC_TIM_MIN);

  devStatus.amplMax.init(1,100,0.0,0);

  initBats();

  f1MainItems.addItem(100);
  f1MainItems.addItem(101);
  f1MainItems.addItem(102);
  f1MainItems.addItem(103);
  f1MainItems.addItem(104);
  f1MainItems.addItem(105);
  f1MainItems.addItem(106);

  f4MainItems.addItem(1001);
  f4MainItems.addItem(1021);
  f4MainItems.addItem(1030);
  f4MainItems.addItem(1031);


  for(int t = 1300; t <= 1316; t++)
    devPars.sampleRate.addItem(t);
  devPars.sampleRate.set(SR_352K);  
  devPars.recTime.init(3, 30, 1, 0);

  devPars.fftLevelMin.init(0, 10000, 200, 0);
  devPars.fftLevelMax.init(0, 300000, 5000, 0);
  devStatus.waterf.setFftLevels(devPars.fftLevelMin.get(), devPars.fftLevelMax.get());

  devPars.threshHold.init(2.0, 80.0, 1.0, 0);
  devStatus.graph.setAmplitudeRaw(MAX_ADC);
  devStatus.graph.setPlotBorders(0.0, 2.5);

  devStatus.btnMeas = new cParBtn(Txt::get(308));
  devStatus.pulseWidth.init(0, 9999, 0, 1);

  devPars.recThreshhold.init(-24,0,1,0);
  devPars.recThreshhold.set(-12);

  devStatus.recCount.set(0);
  devStatus.recCount.init(0,99999,1,0);

  devStatus.btnSetTime = new cParBtn(Txt::get(1032));
  devStatus.year.init(2020, 2030, 1, 0);
  devStatus.month.init(1,12,1,0);
  devStatus.day.init(1,31,1,0);
  devStatus.hour.init(0,23,1,0);
  devStatus.minute.init(0,59,1,0);

  devPars.deafTime.set(0);
  devPars.deafTime.init(0,30,1,0);
  devPars.backLightTime.set(120);
  devPars.backLightTime.init(5,300,1,0);

  devPars.knobRotation.clear();
  devPars.knobRotation.addItem(1151);
  devPars.knobRotation.addItem(1152);

  devPars.dispOrient.clear();
  devPars.dispOrient.addItem(1161);
  devPars.dispOrient.addItem(1162);

  devPars.preTrigger.init(0.0, 50.0, 1.0, 0);

  devStatus.btnAudio = new cParBtn(Txt::get(309));
  load();
#ifndef SIMU_DISPLAY
  devStatus.time.set(rtc.getTime());
  devStatus.date.set(rtc.getTime());
#endif
}

void cMenue::initDialogs() {
  refreshFkeyPanel();
  tCoord lf = LINE_HEIGHT;     ///< distance between two lines of text
  int err;
  // F-KEYs for main panel
  fkeyMainPan = createPanel(PNL_FKEYS, 0, 226, DISP_WIDTH, FKEYPAN_HEIGHT);
  // Serial.println("initDialogs2");
  setFkeyPanel(fkeyMainPan);
  err  = getPan(fkeyMainPan)->addTextItem(1,   0, 227, 80, lf, true, f1Func);
  err |= getPan(fkeyMainPan)->addTextItem(2,  81, 227, 79, lf, true, f2Func);
  err |= getPan(fkeyMainPan)->addTextItem(3, 161, 227, 79, lf, false);
  err |= getPan(fkeyMainPan)->addTextItem(4, 241, 227, 79, lf, true, f4Func);

  // Header for main panel
  setHdrPanel(createPanel(PNL_HEADER, 0, 0, DISP_WIDTH, HDR_HEIGHT));
  hdrMainPanel = getHdrPanel();
  err |= getPan(hdrMainPanel)->addTextItem(150, lf, 1, 180, LINE_HEIGHT);

  // main panel
  panGeo = createPanel(PNL_MAIN, 0, HDR_HEIGHT, DISP_WIDTH,   DISP_HEIGHT - FKEYPAN_HEIGHT - HDR_HEIGHT + 1);
  err |= getPan(panGeo)->addTextItem(202,                     3, 30,           80, lf);
  err |= getPan(panGeo)->addEnumItem(&devStatus.opMode,     100, 30,          140, lf, true, dispModeFunc);
  err |= getPan(panGeo)->addNumItem(&devStatus.recCount,    260, 30,           40, lf, false);
  err |= getPan(panGeo)->itemList[2].isVisible = false;
  err |= getPan(panGeo)->addEnumItem(&devStatus.playStatus, 100, 30 + lf,     120, lf, false);
  err |= getPan(panGeo)->addTextItem(206,                     3, 40 + 2 * lf,  80, lf);
  err |= getPan(panGeo)->addListItem(&devPars.dirSel,       100, 40 + 2 * lf, 210, lf, true, dirFunc);
  err |= getPan(panGeo)->addTextItem(205,                     3, 40 + 3 * lf,  80, lf);
  err |= getPan(panGeo)->addListItem(&devPars.fileSel,      100, 40 + 3 * lf, 210, lf, true, fileFunc);
  err |= getPan(panGeo)->addTextItem(208,                     3, 40 + 4 * lf,  80, lf);
  err |= getPan(panGeo)->addStrItem(&devPars.fileName,      100, 40 + 4 * lf, 210, lf);
  err |= getPan(panGeo)->addTextItem(203,                     3, 50 + 5 * lf,  80, lf);
  err |= getPan(panGeo)->addNumItem(&devPars.mixFreq,       100, 50 + 5 * lf,  15, lf, true);
  err |= getPan(panGeo)->addTextItem(300,                   115, 50 + 5 * lf,  30, lf);
  err |= getPan(panGeo)->addTextItem(204,                     3, 50 + 6 * lf,  80, lf);
  err |= getPan(panGeo)->addNumItem(&devPars.volume,        100, 50 + 6 * lf,  20, lf, true);
  err |= getPan(panGeo)->addTextItem(1320,                    3, 50 + 7 * lf,  80, lf);
  err |= getPan(panGeo)->addEnumItem(&devPars.preAmpType,   100, 50 + 7 * lf, 120, lf, true);
  err |= getPan(panGeo)->addTextItem(1325,                    3, 50 + 8 * lf,  80, lf);
  err |= getPan(panGeo)->addEnumItem(&devPars.preAmpGain,   100, 50 + 8 * lf, 120, lf, true);

  //err |= getPan(panGeo)->addTextItem(200,                     3, 200,          80, lf);
  //err |= getPan(panGeo)->addGeoItem(&devStatus.geoPos,      100, 200,         150, lf);
  err |= getPan(panGeo)->addTextItem(201,                     3, 200 + lf,     70, lf);
  err |= getPan(panGeo)->addDateItem(&devStatus.date,       100, 200 + lf,     70, lf);
  err |= getPan(panGeo)->addTimeItem(&devStatus.time,       180, 200 + lf,     70, lf);
  setMainPanel(panGeo);

  // F-KEYs for waterfall panel
  fkeyWaterPan = createPanel(PNL_FKEYS, 0, 226, DISP_WIDTH, FKEYPAN_HEIGHT);
  err  = getPan(fkeyWaterPan)->addTextItem(  1,   3, 229, 75, lf - 1, true, f1Func);
  err |= getPan(fkeyWaterPan)->addTextItem(310,  83, 229, 75, lf - 1, true, f2WaterFunc);
  err |= getPan(fkeyWaterPan)->addTextItem(311, 163, 229, 75, lf - 1, true, f3WaterFunc);
  err |= getPan(fkeyWaterPan)->addTextItem(  4, 243, 229, 75, lf - 1, true, f4Func);

  hdrPanWaterfall = createPanel(PNL_HEADER,  0, 0, DISP_WIDTH, lf + 1);
  err |= getPan(hdrPanWaterfall)->addTextItem(205, 3, 1, 35, lf);
  err |= getPan(hdrPanWaterfall)->addStrItem(&devPars.fileName, 38, 1, 310, lf);

  panWaterfall = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= getPan(panWaterfall)->addTextItem(300,                15, 80,            25, lf);
  err |= getPan(panWaterfall)->addNumItem(&devPars.freqMax,    20, 20,            25, lf, false);
  err |= getPan(panWaterfall)->addNumItem(&devPars.freqMin,    20, 148,           25, lf, false);
  err |= getPan(panWaterfall)->addNumItem(&devPars.fftLevelMin,50,153+COLMAP_DIST,35,lf, true, fftLevelFunc);
  err |= getPan(panWaterfall)->addNumItem(&devPars.fftLevelMax,270,153+COLMAP_DIST,35, lf, true, fftLevelFunc);
  err |= getPan(panWaterfall)->addNumItem(&devStatus.timMin,  30, 213,           53, lf, true, panWaterZoomFunc);
  err |= getPan(panWaterfall)->addTextItem(301,               85, 213,           25, lf, false);
  err |= getPan(panWaterfall)->addNumItem(&devStatus.timStep,128, 213,           52, lf, true, timeStepFunc);
  err |= getPan(panWaterfall)->addTextItem(305,              182, 213,           35, lf, false);
  err |= getPan(panWaterfall)->addNumItem(&devStatus.timMax, 255, 213,           53, lf, false);
  err |= getPan(panWaterfall)->addTextItem(301,              310, 213,           25, lf, false);
  err |= getPan(panWaterfall)->addGraphItem(&devStatus.waterf,40,  25,          261, 128, graphFunc);

  // x-t-diagram panel
  panTime = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1,     DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= getPan(panTime)->addNumItem(&devStatus.amplMax,       5,  20,           20, lf, true, amplitudeFunc);
  err |= getPan(panTime)->addTextItem(302,                    25,  20,            5, lf);
  err |= getPan(panTime)->addTextItem(306,                     1,  20 +  2 * lf, 23, lf);
  err |= getPan(panTime)->addNumItem(&devPars.threshHold,      1,  20 +  3 * lf, 18, lf, true);
  err |= getPan(panTime)->addTextItem(307,                    19,  20 +  3 * lf,  8, lf);
  err |= getPan(panTime)->addBtnItem(devStatus.btnMeas,        1,  20 +  5 * lf, 30, lf, btnMeasFunc);
  err |= getPan(panTime)->addNumItem(&devStatus.pulseWidth,    1,  20 +  7 * lf, 30, lf, false);
  err |= getPan(panTime)->addStrItem(&devStatus.measUnit,      1,  20 +  8 * lf, 30, lf, false);
  err |= getPan(panTime)->addNumItem(&devStatus.amplMin,       1, 205,           24, lf, false);
  err |= getPan(panTime)->addTextItem(302,                    25, 205,            5, lf);
  err |= getPan(panTime)->addNumItem(&devStatus.timMin,       30, 213,           53, lf, true, panWaterZoomFunc);
  err |= getPan(panTime)->addTextItem(301,                    85, 213,           25, lf, false);
  err |= getPan(panTime)->addNumItem(&devStatus.timStep,     128, 213,           52, lf, true, timeStepFunc);
  err |= getPan(panTime)->addTextItem(305,                   182, 213,           35, lf, false);
  err |= getPan(panTime)->addNumItem(&devStatus.timMax,      255, 213,           53, lf, false);
  err |= getPan(panTime)->addTextItem(301,                   310, 213,           25, lf, false);
  err |= getPan(panTime)->addGraphItem(&devStatus.graph,      33,  25, 285, 185, graphFunc);

  panFont = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1,     DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= getPan(panFont)->addTextItem(12000,                  15, 20,           200, lf);
  err |= getPan(panFont)->addTextItem(12001,                  15, 20      + lf, 200, lf);
  err |= getPan(panFont)->addTextItem(12002,                  15, 20 +  2 * lf, 200, lf);
  err |= getPan(panFont)->addTextItem(12003,                  15, 20 +  3 * lf, 200, lf);
  err |= getPan(panFont)->addTextItem(12004,                  15, 20 +  4 * lf, 200, lf);
  err |= getPan(panFont)->addTextItem(12005,                  15, 20 +  5 * lf, 200, lf);
  err |= getPan(panFont)->addTextItem(12006,                  15, 20 +  6 * lf, 200, lf);
  err |= getPan(panFont)->addTextItem(12007,                  15, 20 +  7 * lf, 200, lf);
  err |= getPan(panFont)->addTextItem(12010,                  15, 20 +  8 * lf, 200, 2 * lf, false, NULL, 2);

  panInfo = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= getPan(panInfo)->addTextItem(400,                     3,  30,           80, lf);
  err |= getPan(panInfo)->addNumItem(&devStatus.cpuAudioAvg, 140,  30,           20, lf, false);
  err |= getPan(panInfo)->addTextItem(401,                   162,  30,           80, lf);
  err |= getPan(panInfo)->addTextItem(405,                     3,  30     + lf,  80, lf);
  err |= getPan(panInfo)->addNumItem(&devStatus.cpuAudioMax, 140,  30     + lf,  20, lf, false);
  err |= getPan(panInfo)->addTextItem(401,                   162,  30     + lf,  80, lf);
  err |= getPan(panInfo)->addTextItem(410,                     3,  30 + 2 * lf,  80, lf);
  err |= getPan(panInfo)->addNumItem(&devStatus.audioMem,    140,  30 + 2 * lf,  80, lf, false);
  err |= getPan(panInfo)->addBtnItem(devStatus.btnAudio,     140,  30 + 4 * lf,  90, lf, btnAudioFunc);

  // parameter panel
  panParams =  createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1,  DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= getPan(panParams)->addTextItem(1100,                  15, 20,            80, lf);
  err |= getPan(panParams)->addEnumItem(&devPars.lang,        170, 20,           100, lf, true, languageFunc);
  err |= getPan(panParams)->addTextItem(1110,                  15, 20 +      lf,  80, lf);
  err |= getPan(panParams)->addEnumItem(&devPars.sampleRate,  170, 20 +      lf,  30, lf, true);
  err |= getPan(panParams)->addTextItem(1120,                  15, 20 +  2 * lf,  80, lf);
  err |= getPan(panParams)->addNumItem(&devPars.recTime,      170, 20 +  2 * lf,  30, lf, true);
  err |= getPan(panParams)->addTextItem(1140,                  15, 20 +  3 * lf,  80, lf);
  err |= getPan(panParams)->addNumItem(&devPars.recThreshhold,170, 20 +  3 * lf,  30, lf, true);
  err |= getPan(panParams)->addTextItem(1330,                  15, 20 +  4 * lf,  80, lf);
  err |= getPan(panParams)->addNumItem(&devPars.preTrigger,   170, 20 +  4 * lf,  80, lf, true);
  err |= getPan(panParams)->addTextItem(1144,                  15, 20 +  5 * lf,  80, lf);
  err |= getPan(panParams)->addNumItem(&devPars.deafTime,     170, 20 +  5 * lf,  20, lf, true);
  err |= getPan(panParams)->addTextItem(1145,                  15, 20 +  6 * lf,  80, lf);
  err |= getPan(panParams)->addNumItem(&devPars.backLightTime,170, 20 +  6 * lf,  20, lf, true);
  err |= getPan(panParams)->addTextItem(1320,                  15, 20 +  7 * lf,  80, lf);
  err |= getPan(panParams)->addEnumItem(&devPars.preAmpType,  170, 20 +  7 * lf,  80, lf, true);
  err |= getPan(panParams)->addTextItem(1325,                  15, 20 +  8 * lf,  80, lf);
  err |= getPan(panParams)->addEnumItem(&devPars.preAmpGain,  170, 20 +  8 * lf,  80, lf, true);
  err |= getPan(panParams)->addTextItem(1150,                  15, 20 +  9 * lf,  80, lf);
  err |= getPan(panParams)->addEnumItem(&devPars.knobRotation,170, 20 +  9 * lf, 100, lf, true);
  err |= getPan(panParams)->addTextItem(1160,                  15, 20 + 10 * lf,  80, lf);
  err |= getPan(panParams)->addEnumItem(&devPars.dispOrient,  170, 20 + 10 * lf,  80, lf, true);

  panBats =  createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1,  DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= getPan(panBats)->addTextItem(1200,                        5,  20,           60, lf);
  err |= getPan(panBats)->addEnumItem(&devStatus.bats.name,      120,  20,          120, lf, true, batFunc);
  err |= getPan(panBats)->addTextItem(1202,                        5,  20 + 1 * lf,  60, lf);
  err |= getPan(panBats)->addStrItem(&devStatus.bats.nameLat,    120,  20 + 1 * lf, 120, lf);
  err |= getPan(panBats)->addTextItem(1204,                        5,  20 + 2 * lf,  60, lf);
  err |= getPan(panBats)->addStrItem(&devStatus.bats.freq,       120,  20 + 2 * lf,  70, lf);
  err |= getPan(panBats)->addTextItem(1210,                        5,  20 + 3 * lf,  60, lf);
  err |= getPan(panBats)->addStrItem(&devStatus.bats.callLen,    120,  20 + 3 * lf,  70, lf);
  err |= getPan(panBats)->addTextItem(1212,                        5,  20 + 4 * lf,  60, lf);
  err |= getPan(panBats)->addStrItem(&devStatus.bats.callDist,   120,  20 + 4 * lf,  70, lf);
  err |= getPan(panBats)->addTextItem(1215,                        5,  20 + 5 * lf,  60, lf);
  err |= getPan(panBats)->addStrItem(&devStatus.bats.characteristic,120,  20 + 5 * lf, 200, lf);
  err |= getPan(panBats)->addTextItem(1217,                        5,  20 + 6 * lf,  60, lf);
  err |= getPan(panBats)->addStrItem(&devStatus.bats.occurrence, 120,  20 + 6 * lf, 200, lf);
  err |= getPan(panBats)->addTextItem(1220,                        5,  20 + 7 * lf,  60, lf);
  err |= getPan(panBats)->addStrItem(&devStatus.bats.wingSpan,   120,  20 + 7 * lf, 200, lf);
  err |= getPan(panBats)->addTextItem(1225,                        5,  20 + 8 * lf,  60, lf);
  err |= getPan(panBats)->addStrItem(&devStatus.bats.weight,     120,  20 + 8 * lf, 200, lf);
  err |= getPan(panBats)->addTextItem(1230,                        5,  20 + 9 * lf,  60, lf);
  err |= getPan(panBats)->addStrItem(&devStatus.bats.habitat,    120,  20 + 9 * lf, 200, lf);
  err |= getPan(panBats)->addTextItem(1235,                        5,  20 + 11 * lf,  60, lf);
  err |= getPan(panBats)->addStrItem(&devStatus.bats.comment,    120,  20 + 11 * lf, 200, lf);

  panDateTime = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1 );
  err |= getPan(panDateTime)->addTextItem(1142,                 120, 20 + 4 * lf,  80, lf);
  err |= getPan(panDateTime)->addNumItem(&devStatus.day,        155, 20 + 4 * lf,  20, lf, true);
  err |= getPan(panDateTime)->addNumItem(&devStatus.month,      180, 20 + 4 * lf,  20, lf, true);
  err |= getPan(panDateTime)->addNumItem(&devStatus.year,       205, 20 + 4 * lf,  30, lf, true);
  err |= getPan(panDateTime)->addTextItem(1143,                 120, 20 + 5 * lf,  80, lf);
  err |= getPan(panDateTime)->addNumItem(&devStatus.hour,       155, 20 + 5 * lf,  20, lf, true);
  err |= getPan(panDateTime)->addNumItem(&devStatus.minute,     180, 20 + 5 * lf,  20, lf, true);
  err |= getPan(panDateTime)->addBtnItem(devStatus.btnSetTime,  120, 20 + 7 * lf,  50, lf, setTimeFunc);
}

void writeFloatToEep(int32_t addr, float val) {
  union {
    float v;
    char b[4];
  } s;

  s.v = val;
#ifndef SIMU_DISPLAY
  EEPROM.write(addr++, s.b[0]);
  EEPROM.write(addr++, s.b[1]);  
  EEPROM.write(addr++, s.b[2]);  
  EEPROM.write(addr++, s.b[3]);
#endif
}

void writeInt16ToEep(int32_t addr, int16_t val) {
  union {
    int16_t v;
    unsigned char b[2];
  } s;

  s.v = val;
#ifndef SIMU_DISPLAY
  EEPROM.write(addr++, s.b[0]);
  EEPROM.write(addr++, s.b[1]);  
#endif
}

float readFloatFromEep(int addr) {
  union {
    float v;
    unsigned char b[4];
  } s;
#ifndef SIMU_DISPLAY
  s.b[0] = EEPROM.read(addr++);  
  s.b[1] = EEPROM.read(addr++);  
  s.b[2] = EEPROM.read(addr++);  
  s.b[3] = EEPROM.read(addr);
#endif
  return s.v;
}

int16_t readInt16FromEep(int32_t addr) {
  union {
    int16_t v;
    unsigned char b[2];
  } s;

#ifndef SIMU_DISPLAY
  s.b[0] = EEPROM.read(addr++);  
  s.b[1] = EEPROM.read(addr);
#endif
  return s.v;
}

unsigned char readCharFromEep(int addr) {
  unsigned char retVal = 0;
#ifndef SIMU_DISPLAY
  retVal = EEPROM.read(addr++);
#endif
  return retVal;
}

void cMenue::save() {
#ifndef SIMU_DISPLAY
  writeFloatToEep(0x0004, devPars.volume.get());
  writeFloatToEep(0x0008, devPars.mixFreq.get());
  writeFloatToEep(0x000C, devPars.freqMin.get());
  writeFloatToEep(0x0010, devPars.freqMax.get());
  writeFloatToEep(0x0014, devPars.recTime.get());
  writeInt16ToEep(0x0018, devPars.sampleRate.get());
  writeInt16ToEep(0x001A, devPars.preAmpGain.get());
  writeFloatToEep(0x001C, devPars.threshHold.get());
  writeFloatToEep(0x0020, devPars.fftLevelMin.get());
  writeFloatToEep(0x0024, devPars.fftLevelMax.get());
  writeFloatToEep(0x002C, devPars.recThreshhold.get());
  writeInt16ToEep(0x0030, devPars.knobRotation.get());
  writeInt16ToEep(0x0032, devPars.dispOrient.get());
  writeFloatToEep(0x0034, devPars.preTrigger.get());
 /*
   * some space left to fill here
   */
  writeFloatToEep(0x003A, devPars.deafTime.get());
  writeFloatToEep(0x003E, devPars.backLightTime.get());
  writeInt16ToEep(0x0042, devPars.lang.get());
  writeInt16ToEep(0x0044, devPars.preAmpType.get());
  int16_t maxAddr = 0x0045;
  writeInt16ToEep(0, maxAddr);

  int16_t chks = 0;
  for(int i = 4; i <= maxAddr; i++)
    chks += readCharFromEep(i);
  writeInt16ToEep(0x0002, chks);
  Serial.printf("  EEPROM written; max. Addr: %i; Checksum %i\n", maxAddr, chks);
 #endif //#ifndef SIMU_DISPLAY
}

void cMenue::load() {
#ifndef SIMU_DISPLAY
  if(checkCRC()) {
    devPars.volume.set(readFloatFromEep(0x0004));
    devPars.mixFreq.set(readFloatFromEep(0x0008));
    devPars.freqMin.set(readFloatFromEep(0x000C));
    devPars.freqMax.set(readFloatFromEep(0x0010));
    devPars.recTime.set(readFloatFromEep(0x0014));
    devPars.sampleRate.set(readInt16FromEep(0x0018));
    devPars.preAmpGain.set(readInt16FromEep(0x001A));
    devPars.threshHold.set(readFloatFromEep(0x001C));
    devPars.fftLevelMin.set(readFloatFromEep(0x0020));
    devPars.fftLevelMax.set(readFloatFromEep(0x0024));
    devPars.recThreshhold.set(readFloatFromEep(0x002C));
    devPars.knobRotation.set(readInt16FromEep(0x0030));
    devPars.dispOrient.set(readInt16FromEep(0x0032));
    devPars.preTrigger.set(readFloatFromEep(0x0034));
    /*
     * some space left to fill here
     */
    devPars.deafTime.set(readFloatFromEep(0x003A));
    devPars.backLightTime.set(readFloatFromEep(0x003E));
    devPars.lang.set(readInt16FromEep(0x0042));
    devPars.preAmpType.set(readInt16FromEep(0x0044));
  }
#endif //#ifndef SIMU_DISPLAY
}


bool cMenue::checkCRC() {
#ifndef SIMU_DISPLAY
  int16_t rdCks = readInt16FromEep(2);
  int16_t maxAddr = readInt16FromEep(0);
  int16_t cks = 0;
  for(int i = 4; i <= maxAddr; i++) {
    cks += readCharFromEep(i);
  }
  
  bool retVal = (rdCks == cks);
  if(!retVal)
    DPRINTF1("checksum error in EEPROM, expected %i, read %i\n", rdCks, cks);
  return retVal;
#else
  return true;
#endif
}

void cMenue::loadLanguage() {
#ifndef SIMU_DISPLAY
  if(checkCRC()) {
    devPars.lang.set(readInt16FromEep(0x0042));
    if(devPars.lang.get() == 1)
      Txt::setLang(LANG_EN);
    else
      Txt::setLang(LANG_GER);
  }
#endif //#ifndef SIMU_DISPLAY
}


void cMenue::printPars() {
#ifndef SIMU_DISPLAY
  Serial.printf("volume             [dB]: %.0f\n", devPars.volume.get());
  Serial.printf("mixer frequency   [kHz]: %.0f\n", devPars.mixFreq.get());
  Serial.printf("min freq. diagram [kHz]: %.0f\n", devPars.freqMin.get());
  Serial.printf("max freq. diagram [kHz]: %.0f\n", devPars.freqMax.get());
  Serial.printf("recording time      [s]: %.1f\n", devPars.recTime.get());
  Serial.printf("sampling rate     [kHz]: %s\n", devPars.sampleRate.getActText());
  Serial.printf("pre amp type           : %s\n", devPars.preAmpType.getActText());
  Serial.printf("pre amp gain           : %s\n", devPars.preAmpGain.getActText());
#endif //#ifndef SIMU_DISPLAY
}


void cMenue::printStatus() {
 // enCassMode cassMode = CAS_STOP; ///< mode of operation of cassette player
#ifndef SIMU_DISPLAY
  Serial.printf("avg. audio CPU usage: %.0f\n", devStatus.cpuAudioAvg.get());
  Serial.printf("max. audio CPU usage: %.0f\n", devStatus.cpuAudioMax.get());
  Serial.printf("audio memory usage:   %.0f\n", devStatus.audioMem.get());
#endif //#ifndef SIMU_DISPLAY
}

void cMenue::initBats() {
  char line[512];
  char line2[512];
  size_t byteCount;
  cSdCard& sd = cSdCard::inst();
  tFILE file;
  enSdRes res = sd.openFile("/info/bat_info.tsv", file, READ);
  if(res == 0)
  {
    devStatus.bats.name.clear();
    // read and forget first two header
    res = sd.readLine(file, line, sizeof(line), byteCount);
    if(res == 0) {
      for(;;) {
        line[0] = 0;
        res = sd.readLine(file, line, sizeof(line), byteCount);        
        if(res == 0) {
          line[byteCount] = 0;
          cUtils::replaceUTF8withInternalCoding(line, line2, sizeof(line2));             
          char* token = strtok(line2, "\t");
          DPRINTF1("token: %s", token);
          devStatus.bats.name.addItem(token);
        }
        else
          break;
      }
    }
    sd.closeFile(file);
  }
}

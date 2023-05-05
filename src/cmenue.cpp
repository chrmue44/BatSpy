/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include <cstdlib>
#include <cstring>

#include "ctext.h"
#include "cutils.h"
#include <ILI9341_t3.h>
#include "debug.h"
#include "cfileinfo.h"
#include "pnlmain.h"
#include "pnlwaterfall.h"
#include "pnlbats.h"
#include "pnlparams.h"
#include "pnlinfo.h"
#include "pnllive.h"
#include "pnlfilebrowser.h"

#include "cSdCard.h"
#include <Arduino.h>
#include <Time.h>
#include "globals.h"

#pragma GCC diagnostic ignored "-Wunused-parameter" //disable because fuFocus functions may not use the parameters

#define DEBUG_LEVEL  1


cMenue::cMenue(int width, int height, ILI9341_t3* pDisplay) :
  cMenuesystem(width, height, pDisplay) {
}

cMenue::~cMenue() {
}

void MEMP cMenue::initPars() 
{
  devPars.lang.clear();
  devPars.lang.addItem(1101);
  devPars.lang.addItem(1102);
  loadLanguage();

  devPars.mixFreq.init(1,20, 1, 0);

  devPars.volume.init(-30,15, 3, 0);
  devPars.volume.set(-18);

  initBats();

  if(hasAmpRevB())
  {
    devPars.preAmpGain.addItem(1350);
    devPars.preAmpGain.addItem(1351);
    devPars.preAmpGain.addItem(1352);
    devPars.preAmpGain.addItem(1353);
    devPars.preAmpGain.addItem(1354);
    devPars.preAmpGain.addItem(1355);
    devPars.preAmpType.addItem(1322);
  }
  else
  {
    devPars.preAmpGain.addItem(1331);
    devPars.preAmpGain.addItem(1332);
    devPars.preAmpGain.addItem(1333);
    devPars.preAmpGain.addItem(1334);
    devPars.preAmpType.addItem(1321);
    devPars.preAmpType.addItem(1322);
  }


  for(int t = 1300; t <= 1318; t++)
    devPars.sampleRate.addItem(t);
  devPars.sampleRate.set(SR_312K);  
  devPars.recTime.init(3, 30, 1, 0);

  devPars.fftLevelMin.init(0, 10000, 200, 0);
  devPars.fftLevelMax.init(0, 300000, 5000, 0);

  devPars.threshHold.init(2.0, 80.0, 1.0, 0);
  devPars.filtFreq.init(5,70, 1, 0);

  devPars.filtType.addItem(1171);
  devPars.filtType.addItem(1172);
  devPars.filtType.addItem(1173);

  devPars.recThreshhold.init(-24,0,1,0);
  devPars.recThreshhold.set(-12);

  devPars.recFmt.addItem(1146);   //enRecFmt::RAW
  devPars.recFmt.addItem(1147);   //enRecFmt::WAV

  devPars.deafTime.set(0);
  devPars.deafTime.init(0,30,1,0);
//  devPars.backLightTime.set(120);
  devPars.backLightTime.init(5,300,1,0);

  devPars.knobRotation.clear();
  devPars.knobRotation.addItem(1151);
  devPars.knobRotation.addItem(1152);

  devPars.dispOrient.clear();
  devPars.dispOrient.addItem(1161);
  devPars.dispOrient.addItem(1162);

  devPars.preTrigger.init(0.0, 150.0, 1.0, 0);
  devPars.startH.init(0, 23, 1, 0, 2);
  devPars.startMin.init(0, 59, 10, 0, 2);
  devPars.stopH.init(0, 23, 1, 0, 2);
  devPars.stopMin.init(0, 59, 10, 0, 2);
  devPars.voltFactor.init(0, 1, 0.0000001f, 5);

  devPars.recAuto.addItem(1400),
  devPars.recAuto.addItem(1401),
  devPars.recAuto.addItem(1402),
  devPars.recAuto.addItem(1403),

  devPars.projectType.addItem(1191);
  devPars.projectType.addItem(1192);

  devPars.sendDelay.init(0, 20, 1, 0);
  devPars.sweepSpeed.init(0, 10, 1, 0);
  devPars.liveAmplitude.init(10,300, 2, 0);

  devPars.srcPosition.addItem(1410);
  devPars.srcPosition.addItem(1412);

  devPars.menueType.addItem(1711),
  devPars.menueType.addItem(1712),
  devPars.menueType.addItem(1713),

  devPars.triggerType.addItem(1361);
  devPars.triggerType.addItem(1362);
  devPars.triggerType.addItem(1363);

  devPars.minEventLen.init(0.0f, 20.0f, 0.5f, 1);

  notes1.initNotes(PATH_NOTES1, 2000, 2005);
  notes1.initListPar(devStatus.notes1);
  notes2.initNotes(PATH_NOTES2, 2010, 2013);
  notes2.initListPar(devStatus.notes2);

    devStatus.opMode.clear();
  devStatus.opMode.addItem(20);
  devStatus.opMode.addItem(21);
  devStatus.opMode.addItem(22);
  devStatus.opMode.addItem(23);
  devStatus.opMode.addItem(24);

  devStatus.playStatus.clear();
  devStatus.playStatus.addItem(320);
  devStatus.playStatus.addItem(325);
  devStatus.playStatus.addItem(335);
  devStatus.playStatus.addItem(337);

  devStatus.timStep.init(0, 10, 0.0f, DEC_TIM_MIN);
  devStatus.timMin.init(0, 10, devStatus.timStep.get() * 2, DEC_TIM_MIN);
  devStatus.timMax.init(0, 12, 0.4f, DEC_TIM_MIN);

  devStatus.amplMax.init(1,100,0.0,0);

  devStatus.cpuAudioAvg.init(0,100,1,2);
  devStatus.cpuAudioMax.init(0,100,1,2);
  devStatus.audioMem.init(0,10000,1,0);
  devStatus.dirFilter.addItem(221);
  devStatus.dirFilter.addItem(222);
  devStatus.dirFilter.addItem(223);
  devStatus.dirFilter.addItem(224);
  devStatus.dirFilter.addItem(225);

  for(int i=0; i < DIR_PAN_SIZE; i++)
    devStatus.dirFiles[i].set("--");

  devStatus.waterf.setFftLevels(devPars.fftLevelMin.get(), devPars.fftLevelMax.get());
  devStatus.graph.setAmplitudeRaw(MAX_ADC);
  devStatus.graph.setPlotBorders(0.0, 2.5);

  devStatus.pulseWidth.init(0, 9999, 0, 1);

  devStatus.recCount.set(0);
  devStatus.recCount.init(0,99999,1,0);

  devStatus.year.init(2020, 2030, 1, 0);
  devStatus.month.init(1,12,1,0);
  devStatus.day.init(1,31,1,0);
  devStatus.hour.init(0,23,1,0);
  devStatus.minute.init(0,59,1,0);

  devStatus.freqMax.init(1, 300, 1, 0);
  devStatus.freqMin.init(1, 300, 1, 0);

  devStatus.latSign.addItem(1341);
  devStatus.latSign.addItem(1342);
  devStatus.latDeg.init(0,59,1,0,2);
  devStatus.latMin.init(0, 59, 1, 0,2);
  devStatus.latSec.init(0,999,5,0,3);
  devStatus.lonSign.addItem(1343);
  devStatus.lonSign.addItem(1344);
  devStatus.lonDeg.init(0,59,1,0,3);
  devStatus.lonMin.init(0, 59, 1, 0,2);
  devStatus.lonSec.init(0,999,5,0,3);
  devStatus.geoPos.set(49.1234, 8.2345f);
  
  devStatus.peakVal.init(0, 100, 0.1f, 2);
  devStatus.freeSpace.init(0, 256000, 1, 0);
  devStatus.voltage.init(0, 20, 0.05f, 2);
  devStatus.digits.init(0, 10000, 1, 0);
  devStatus.temperature.init(-50, 100, 0.1f, 1);

  devStatus.freq1Tick.init(0, 300,0.1f, 1);
  devStatus.msPerDiv.init(0, 10000, 1, 0);
  devStatus.satCount.init(0, 24, 1, 0);
  devStatus.height.init(-100, 10000, 5, 0);
  devStatus.posValid.addItem(16);
  devStatus.posValid.addItem(15);
  devStatus.lastCallF.init(0.0f, 200.0f, 0.1f, 1);

  load();
  setPosFunc(this, enKey::NO, nullptr);

  if (devPars.recAuto.get() == enRecAuto::TWILIGHT)
    calcSunrise();
  devStatus.time.set(rtc.getTime());
  devStatus.date.set(rtc.getTime());
}

void MEMP cMenue::initDialogs() 
{
  clearPanelList();
  refreshFkeyPanel();
  tCoord lf = LINE_HEIGHT;     ///< distance between two lines of text
  // Header for main panel
  setHdrPanel(createPanel(PNL_HEADER, 0, 0, DISP_WIDTH, HDR_HEIGHT));
  hdrMainPanel = getHdrPanel();
  getPan(hdrMainPanel)->addTextItem(150, lf, 1, 180, LINE_HEIGHT);

  // F-KEYs for main panel
  fkeyMainPan = createPanel(PNL_FKEYS, 0, 226, DISP_WIDTH, FKEYPAN_HEIGHT);
  setFkeyPanel(fkeyMainPan);

  switch (devPars.menueType.get())
  {
    default:
    case enMenueType::EXPERT:
      initFunctionItemsExpert();
      initExpertPanels(lf);
      break;
    case enMenueType::RECORDER:
      initFunctionItemsRecorder();
      initRecorderPanels(lf);
      break;
    case enMenueType::HANDHELD:
      initFunctionItemsHandheld();
      initHandheldPanels(lf);
      break;
  }
}

int MEMP cMenue::initExpertPanels(tCoord lf)
{
  int err = initFkeyPanel(getPan(fkeyMainPan), lf);

  // main panel
  panGeo = createPanel(PNL_MAIN, 0, HDR_HEIGHT, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT - HDR_HEIGHT + 1);
  err |= initMainPanelExpert(getPan(panGeo), lf);

  // F-KEYs for waterfall panel
  fkeyWaterPan = createPanel(PNL_FKEYS, 0, 226, DISP_WIDTH, FKEYPAN_HEIGHT);
  err |= initFkeysWaterPan(getPan(fkeyWaterPan), lf);

  hdrPanWaterfall = createPanel(PNL_HEADER, 0, 0, DISP_WIDTH, HDR_HEIGHT);
  err |= getPan(hdrPanWaterfall)->addTextItem(205, 3, 1, 35, lf);
  err |= getPan(hdrPanWaterfall)->addStrItem(&devPars.fileName, 38, 1, 310, lf);

  panWaterfall = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initWaterPan(getPan(panWaterfall), lf);

  // x-t-diagram panel
  panTime = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initTimePan(getPan(panTime), lf);

  pnlLive = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initLivePan(getPan(pnlLive), lf);

  panFont = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= getPan(panFont)->addTextItem(12000, 15, 20, 200, lf);
  err |= getPan(panFont)->addTextItem(12001, 15, 20 + lf, 200, lf);
  err |= getPan(panFont)->addTextItem(12002, 15, 20 + 2 * lf, 200, lf);
  err |= getPan(panFont)->addTextItem(12003, 15, 20 + 3 * lf, 200, lf);
  err |= getPan(panFont)->addTextItem(12004, 15, 20 + 4 * lf, 200, lf);
  err |= getPan(panFont)->addTextItem(12005, 15, 20 + 5 * lf, 200, lf);
  err |= getPan(panFont)->addTextItem(12006, 15, 20 + 6 * lf, 200, lf);
  err |= getPan(panFont)->addTextItem(12007, 15, 20 + 7 * lf, 200, lf);
  err |= getPan(panFont)->addTextItem(12010, 15, 20 + 8 * lf, 200, 2 * lf, false, NULL, 2);

  hdrBatInfo = createPanel(PNL_HEADER, 0, 0, DISP_WIDTH, HDR_HEIGHT);
  err |= getPan(hdrBatInfo)->addTextItem(1500, 3, 1, 80, lf);
  err |= getPan(hdrBatInfo)->addStrItem(&devStatus.bats.nameLat, 95, 1, 225, lf);


  panInfo = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initInfoPanExpert(getPan(panInfo), lf);

  hdrParams = createPanel(PNL_HEADER, 0, 0, DISP_WIDTH, HDR_HEIGHT);
  err |= getPan(hdrParams)->addTextItem(1510, 3, 1, 180, lf);

  // parameter panel
  panParams = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initParPan(getPan(panParams), lf);

  panParRec = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initParRec(getPan(panParRec), lf);

  panPosition = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initPositionPan(getPan(panPosition), lf);

  panBats = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initBatPan(getPan(panBats), lf);

  panDateTime = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initDateTimePan(getPan(panDateTime), lf);

  fkeyFilePan = createPanel(PNL_FKEYS, 0, 226, DISP_WIDTH, FKEYPAN_HEIGHT);
  err |= initFkeyFilePanel(getPan(fkeyFilePan), lf);

  panFileBrowser = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initFileBrowserPan(getPan(panFileBrowser), lf);
  initFileBrowser(getPan(panFileBrowser), "/");

  setMainPanel(panGeo);

  return err;
}

int MEMP cMenue::initHandheldPanels(tCoord lf)
{
  int err = initFkeyPanel(getPan(fkeyMainPan), lf);

  // main panel
  panGeo = createPanel(PNL_MAIN, 0, HDR_HEIGHT, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT - HDR_HEIGHT + 1);
  err |= initMainPanelExpert(getPan(panGeo), lf);

  // F-KEYs for waterfall panel
  fkeyWaterPan = createPanel(PNL_FKEYS, 0, 226, DISP_WIDTH, FKEYPAN_HEIGHT);
  err |= initFkeysWaterPan(getPan(fkeyWaterPan), lf);

  hdrPanWaterfall = createPanel(PNL_HEADER, 0, 0, DISP_WIDTH, HDR_HEIGHT);
  err |= getPan(hdrPanWaterfall)->addTextItem(205, 3, 1, 35, lf);
  err |= getPan(hdrPanWaterfall)->addStrItem(&devPars.fileName, 38, 1, 310, lf);

  panWaterfall = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initWaterPan(getPan(panWaterfall), lf);

  // x-t-diagram panel
  panTime = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initTimePan(getPan(panTime), lf);

  pnlLive = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initLivePan(getPan(pnlLive), lf);

  hdrBatInfo = createPanel(PNL_HEADER, 0, 0, DISP_WIDTH, HDR_HEIGHT);
  err |= getPan(hdrBatInfo)->addTextItem(1500, 3, 1, 80, lf);
  err |= getPan(hdrBatInfo)->addStrItem(&devStatus.bats.nameLat, 95, 1, 225, lf);


  panInfo = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initInfoPanRecorder(getPan(panInfo), lf);

  hdrParams = createPanel(PNL_HEADER, 0, 0, DISP_WIDTH, HDR_HEIGHT);
  err |= getPan(hdrParams)->addTextItem(1510, 3, 1, 180, lf);

  // parameter panel
  panParams = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initParPan(getPan(panParams), lf);

  panParRec = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initParRec(getPan(panParRec), lf);

  panPosition = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initPositionPan(getPan(panPosition), lf);

  panBats = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initBatPan(getPan(panBats), lf);

  panDateTime = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initDateTimePan(getPan(panDateTime), lf);

  fkeyFilePan = createPanel(PNL_FKEYS, 0, 226, DISP_WIDTH, FKEYPAN_HEIGHT);
  err |= initFkeyFilePanel(getPan(fkeyFilePan), lf);

  panFileBrowser = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initFileBrowserPan(getPan(panFileBrowser), lf);
  initFileBrowser(getPan(panFileBrowser), "/");

  setMainPanel(panGeo);

  return err;
}


int MEMP cMenue::initRecorderPanels(tCoord lf)
{
  int err = initFkeyPanel(getPan(fkeyMainPan), lf);

  // main panel
  panGeo = createPanel(PNL_MAIN, 0, HDR_HEIGHT, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT - HDR_HEIGHT + 1);
  err |= initMainPanelRecorder(getPan(panGeo), lf);

  // F-KEYs for waterfall panel
  fkeyWaterPan = createPanel(PNL_FKEYS, 0, 226, DISP_WIDTH, FKEYPAN_HEIGHT);
  err |= initFkeysWaterPan(getPan(fkeyWaterPan), lf);

  pnlLive = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initLivePan(getPan(pnlLive), lf);
  hdrParams = createPanel(PNL_HEADER, 0, 0, DISP_WIDTH, HDR_HEIGHT);
  err |= getPan(hdrParams)->addTextItem(1510, 3, 1, 180, lf);

  panInfo = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initInfoPanRecorder(getPan(panInfo), lf);

  // parameter panel
  panParams = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initParPan(getPan(panParams), lf);

  panParRec = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initParRec(getPan(panParRec), lf);

  panPosition = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initPositionPan(getPan(panPosition), lf);

  panDateTime = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initDateTimePan(getPan(panDateTime), lf);

  fkeyFilePan = createPanel(PNL_FKEYS, 0, 226, DISP_WIDTH, FKEYPAN_HEIGHT);
  err |= initFkeyFilePanel(getPan(fkeyFilePan), lf);

  panFileBrowser = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initFileBrowserPan(getPan(panFileBrowser), lf);
  initFileBrowser(getPan(panFileBrowser), "/");

  setMainPanel(panGeo);

  return err;
}

void MEMP writeFloatToEep(int32_t addr, float val) 
{
  union 
  {
    float v;
    char b[4];
  } s;

  s.v = val;
  EEPROM.write(addr++, s.b[0]);
  EEPROM.write(addr++, s.b[1]);  
  EEPROM.write(addr++, s.b[2]);  
  EEPROM.write(addr++, s.b[3]);
}

void MEMP writeInt16ToEep(int32_t addr, int16_t val) 
{
  union  
  {
    int16_t v;
    unsigned char b[2];
  } s;

  s.v = val;
  EEPROM.write(addr++, s.b[0]);
  EEPROM.write(addr++, s.b[1]);  
}

float MEMP readFloatFromEep(int addr) 
{
  union 
  {
    float v;
    unsigned char b[4];
  } s;
  s.b[0] = EEPROM.read(addr++);  
  s.b[1] = EEPROM.read(addr++);  
  s.b[2] = EEPROM.read(addr++);  
  s.b[3] = EEPROM.read(addr);
  return s.v;
}

int16_t MEMP cMenue::readInt16FromEep(int32_t addr) 
{
  union 
  {
    int16_t v;
    unsigned char b[2];
  } s;

  s.b[0] = EEPROM.read(addr++);  
  s.b[1] = EEPROM.read(addr);
  return s.v;
}

unsigned char MEMP readCharFromEep(int addr) 
{
  unsigned char retVal = 0;
  retVal = EEPROM.read(addr++);
  return retVal;
}

void MEMP cMenue::save() 
{
  writeFloatToEep(0x0004, devPars.volume.get());
  writeFloatToEep(0x0008, devPars.mixFreq.get());
  writeInt16ToEep(0x000C, devPars.recAuto.get());
  writeInt16ToEep(0x000E, (int16_t)devPars.sendDelay.get());
  writeInt16ToEep(0x0010, devPars.srcPosition.get());
  writeInt16ToEep(0x0012, devPars.menueType.get());
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
  writeInt16ToEep(0x0038, devPars.recFmt.get());
  writeFloatToEep(0x003A, devPars.deafTime.get());
  writeFloatToEep(0x003E, devPars.backLightTime.get());
  writeInt16ToEep(0x0042, devPars.lang.get());
  writeInt16ToEep(0x0044, devPars.preAmpType.get());
  writeFloatToEep(0x0046, devStatus.geoPos.getLat());
  writeFloatToEep(0x004A, devStatus.geoPos.getLon());
  writeFloatToEep(0x004E, devPars.filtFreq.get());
  writeInt16ToEep(0x0052, devPars.filtType.get());
  writeInt16ToEep(0x0054, (int16_t)devPars.startH.get());
  writeInt16ToEep(0x0056, (int16_t)devPars.startMin.get());
  writeInt16ToEep(0x0058, (int16_t)devPars.stopH.get());
  writeInt16ToEep(0x005A, (int16_t)devPars.stopMin.get());
  writeFloatToEep(0x005C, (int16_t)devPars.voltFactor.get());
  writeInt16ToEep(0x0060, (int16_t)devPars.sweepSpeed.get());
  writeInt16ToEep(0x0062, (int16_t)devPars.liveAmplitude.get());
  writeInt16ToEep(0x0064, (int16_t)devPars.projectType.get());
  writeFloatToEep(0x0066, devStatus.height.get());
  writeInt16ToEep(0x006A, (int16_t)devPars.triggerType.get());
  writeFloatToEep(0x006C, devPars.minEventLen.get());
  writeInt16ToEep(0x0070, 0);
  writeInt16ToEep(0x0072, 0);
  writeInt16ToEep(0x0074, 0);
  writeInt16ToEep(0x0076, 0);
  writeInt16ToEep(0x0078, 0);
  writeInt16ToEep(0x007A, 0);
  writeInt16ToEep(0x007C, 0);
  writeInt16ToEep(0x007E, 0);
  writeInt16ToEep(0x0080, 0);
  writeInt16ToEep(0x0082, 0);
  writeInt16ToEep(0x0084, 0);
  writeInt16ToEep(0x0086, 0);
  writeInt16ToEep(0x0088, 0);
  writeInt16ToEep(0x008A, 0);
  writeInt16ToEep(0x008C, 0);
  writeInt16ToEep(0x008E, 0);
  int16_t maxAddr = 0x008F;
  writeInt16ToEep(0, maxAddr);

  int16_t chks = 0;
  for(int i = 4; i <= maxAddr; i++)
    chks += readCharFromEep(i);
  writeInt16ToEep(0x0002, chks);
  Serial.printf("  EEPROM written; max. Addr: %i; Checksum %i\n", maxAddr, chks);
}

void MEMP cMenue::load()
{
  if(checkCRC())
  {
    devPars.volume.set(readFloatFromEep(0x0004));
    devPars.mixFreq.set(readFloatFromEep(0x0008));
    devPars.recAuto.set(readInt16FromEep(0x000C));
    devPars.sendDelay.set(readInt16FromEep(0x000E));
    devPars.srcPosition.set(readInt16FromEep(0x0010));
    devPars.menueType.set(readInt16FromEep(0x0012));
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
    devPars.recFmt.set(readInt16FromEep(0x0038));
    devPars.deafTime.set(readFloatFromEep(0x003A));
    devPars.backLightTime.set(readFloatFromEep(0x003E));
    devPars.lang.set(readInt16FromEep(0x0042));
    devPars.preAmpType.set(readInt16FromEep(0x0044));
    devStatus.geoPos.setLat(readFloatFromEep(0x0046));
    devStatus.geoPos.setLon(readFloatFromEep(0x004A));
    devPars.filtFreq.set(readFloatFromEep(0x004E));
    devPars.filtType.set(readInt16FromEep(0x0052));
    devPars.startH.set(readInt16FromEep(0x0054));   //if addr changes see also pnlparams.cpp
    devPars.startMin.set(readInt16FromEep(0x0056)); //if addr changes see also pnlparams.cpp
    devPars.stopH.set(readInt16FromEep(0x0058));    //if addr changes see also pnlparams.cpp
    devPars.stopMin.set(readInt16FromEep(0x005A));  //if addr changes see also pnlparams.cpp
    devPars.voltFactor.set(readFloatFromEep(0x005C));
    devPars.sweepSpeed.set(readInt16FromEep(0x0060));
    devPars.liveAmplitude.set(readInt16FromEep(0x0062));
    devPars.projectType.set(readInt16FromEep(0x0064));
    devStatus.height.set(readFloatFromEep(0x0066));
    devPars.triggerType.set(readInt16FromEep(0x006A));
    devPars.minEventLen.set(readFloatFromEep(0x006C));
  }
}


bool MEMP cMenue::checkCRC()
{
  int16_t rdCks = readInt16FromEep(2);
  int16_t maxAddr = readInt16FromEep(0);
  int16_t cks = 0;
  if (maxAddr == 0)
    return false;
  for(int i = 4; i <= maxAddr; i++)
  {
    cks += readCharFromEep(i);
  }
  
  bool retVal = (rdCks == cks);
  if(!retVal)
    DPRINTF1("checksum error in EEPROM, expected %i, read %i\n", rdCks, cks);
  return retVal;
}

void MEMP cMenue::loadLanguage()
{
  if(checkCRC())
  {
    devPars.lang.set(readInt16FromEep(0x0042));
    if(devPars.lang.get() == 1)
      Txt::setLang(LANG_EN);
    else
      Txt::setLang(LANG_GER);
  }
}


void MEMP cMenue::printPars()
{
  Serial.printf("volume             [dB]: %.0f\n", devPars.volume.get());
  Serial.printf("mixer frequency   [kHz]: %.0f\n", devPars.mixFreq.get());
  Serial.printf("recording time      [s]: %.1f\n", devPars.recTime.get());
  Serial.printf("sampling rate     [kHz]: %s\n", devPars.sampleRate.getActText());
  Serial.printf("pre trigger        [ms]: %.0f\n", devPars.preTrigger.get());
  Serial.printf("trigger level       [%]: %.3f\n", devPars.recThreshhold.get() * 100);
  Serial.printf("pre amp type           : %s\n", devPars.preAmpType.getActText());
  Serial.printf("pre amp gain           : %s\n", devPars.preAmpGain.getActText());
}


void MEMP cMenue::printStatus()
{
 // enCassMode cassMode = STOP; ///< mode of operation of cassette player
  Serial.printf("avg. audio CPU usage: %.2f\n", devStatus.cpuAudioAvg.get());
  Serial.printf("max. audio CPU usage: %.2f\n", devStatus.cpuAudioMax.get());
  Serial.printf("audio memory usage:   %.0f\n", devStatus.audioMem.get());
}


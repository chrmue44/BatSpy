#include <cstdlib>
#include <cstring>
#include "cmenue.h"
#include "ctext.h"
#include "cutils.h"
#include <ILI9341_t3.h>
#include "cRtc.h"
#include "debug.h"
#include "cAudio.h"
#include "cfileinfo.h"
#include "pnlmain.h"
#include "pnlwaterfall.h"
#include "pnlbats.h"
#include "pnlparams.h"
#include "pnlinfo.h"

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

cMenue::cMenue(int width, int height, ILI9341_t3* pDisplay) :
  cMenuesystem(width, height, pDisplay) {
}

cMenue::~cMenue() {
}

void cMenue::initPars() {
#ifdef AMP_REV1
  devPars.preAmpGain.addItem(1326);
  devPars.preAmpGain.addItem(1327);
  devPars.preAmpGain.addItem(1328);
#endif
#ifdef AMP_REV2
  devPars.preAmpGain.addItem(1331);
  devPars.preAmpGain.addItem(1332);
  devPars.preAmpGain.addItem(1333);
#endif
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

  devStatus.cpuAudioAvg.init(0,100,1,2);
  devStatus.cpuAudioMax.init(0,100,1,2);
  devStatus.audioMem.init(0,10000,1,0);
  initBats();

  initFunctionItems();


  for(int t = 1300; t <= 1317; t++)
    devPars.sampleRate.addItem(t);
  devPars.sampleRate.set(SR_352K);  
  devPars.recTime.init(3, 30, 1, 0);

  devPars.fftLevelMin.init(0, 10000, 200, 0);
  devPars.fftLevelMax.init(0, 300000, 5000, 0);
  devStatus.waterf.setFftLevels(devPars.fftLevelMin.get(), devPars.fftLevelMax.get());

  devPars.threshHold.init(2.0, 80.0, 1.0, 0);
  devStatus.graph.setAmplitudeRaw(MAX_ADC);
  devStatus.graph.setPlotBorders(0.0, 2.5);

  devPars.filtFreq.init(5,70, 1, 0);

  devPars.filtType.addItem(1171);
  devPars.filtType.addItem(1172);
  devPars.filtType.addItem(1173);

  devStatus.btnMeas = new cParBtn(Txt::get(308));
  devStatus.pulseWidth.init(0, 9999, 0, 1);

  devPars.recThreshhold.init(-24,0,1,0);
  devPars.recThreshhold.set(-12);

  devPars.recFmt.addItem(1146);   //enRecFmt::RAW
  devPars.recFmt.addItem(1147);   //enRecFmt::WAV

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
  devStatus.geoPos.setLat(49.1234);
  devStatus.geoPos.setLon(8.2345);
  devStatus.btnAudio = new cParBtn(Txt::get(309));
  
  devStatus.peakVal.init(0, 20, 0.1, 2);

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
  setFkeyPanel(fkeyMainPan);
  err = initFkeyPanel(getPan(fkeyMainPan), lf);

  // Header for main panel
  setHdrPanel(createPanel(PNL_HEADER, 0, 0, DISP_WIDTH, HDR_HEIGHT));
  hdrMainPanel = getHdrPanel();
  err |= getPan(hdrMainPanel)->addTextItem(150, lf, 1, 180, LINE_HEIGHT);

  // main panel
  panGeo = createPanel(PNL_MAIN, 0, HDR_HEIGHT, DISP_WIDTH,   DISP_HEIGHT - FKEYPAN_HEIGHT - HDR_HEIGHT + 1);
  err |= initMainPanel(getPan(panGeo), lf);
  setMainPanel(panGeo);

  // F-KEYs for waterfall panel
  fkeyWaterPan = createPanel(PNL_FKEYS, 0, 226, DISP_WIDTH, FKEYPAN_HEIGHT);
  err |= initFkeysWaterPan(getPan(fkeyWaterPan), lf);

  hdrPanWaterfall = createPanel(PNL_HEADER,  0, 0, DISP_WIDTH, lf + 1);
  err |= getPan(hdrPanWaterfall)->addTextItem(205, 3, 1, 35, lf);
  err |= getPan(hdrPanWaterfall)->addStrItem(&devPars.fileName, 38, 1, 310, lf);

  panWaterfall = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initWaterPan(getPan(panWaterfall), lf);
  // x-t-diagram panel
  panTime = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1,     DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initTimePan(getPan(panTime), lf);

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
  err |= initInfoPan(getPan(panInfo), lf);

  // parameter panel
  panParams =  createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1,  DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initParPan(getPan(panParams), lf);

  panPosition = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1,  DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initPositionPan(getPan(panPosition), lf);

  panBats =  createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1,  DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1);
  err |= initBatPan(getPan(panBats), lf);

  panDateTime = createPanel(PNL_MAIN, 0, FKEYPAN_HEIGHT + 1, DISP_WIDTH, DISP_HEIGHT - FKEYPAN_HEIGHT * 2 - 1 );
  err |= initDateTimePan(getPan(panDateTime), lf);
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
  // 4 bytes free here
  // 4 bytes free here
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
  writeInt16ToEep(0x0054, 0);
  writeInt16ToEep(0x0056, 0);
  writeInt16ToEep(0x0058, 0);
  writeInt16ToEep(0x005A, 0);
  writeInt16ToEep(0x005C, 0);
  writeInt16ToEep(0x005E, 0);
  int16_t maxAddr = 0x005F;
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
    // 4 bytes free here
    // 4 bytes free here 
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
  Serial.printf("recording time      [s]: %.1f\n", devPars.recTime.get());
  Serial.printf("sampling rate     [kHz]: %s\n", devPars.sampleRate.getActText());
  Serial.printf("pre trigger        [ms]: %.0f\n", devPars.preTrigger.get());
  Serial.printf("trigger level       [%]: %.3f\n", devPars.recThreshhold.get() * 100);
  Serial.printf("pre amp type           : %s\n", devPars.preAmpType.getActText());
  Serial.printf("pre amp gain           : %s\n", devPars.preAmpGain.getActText());
#endif //#ifndef SIMU_DISPLAY
}


void cMenue::printStatus() {
 // enCassMode cassMode = STOP; ///< mode of operation of cassette player
#ifndef SIMU_DISPLAY
  Serial.printf("avg. audio CPU usage: %.2f\n", devStatus.cpuAudioAvg.get());
  Serial.printf("max. audio CPU usage: %.2f\n", devStatus.cpuAudioMax.get());
  Serial.printf("audio memory usage:   %.0f\n", devStatus.audioMem.get());
#endif //#ifndef SIMU_DISPLAY
}


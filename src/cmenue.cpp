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
//#include <ILI9341_t3.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1327.h"
#include "debug.h"
#include "cfileinfo.h"
#include "pnlmain.h"
#include "pnlwaterfall.h"
#include "pnlbats.h"
#include "pnlparams.h"
#include "pnlinfo.h"
#include "pnllive.h"
#include "pnlfilebrowser.h"
#include "cEeprom.h"

#include "cSdCard.h"
#include <Arduino.h>
#include <Time.h>
#include "globals.h"

#pragma GCC diagnostic ignored "-Wunused-parameter" //disable because fuFocus functions may not use the parameters

//#define DEBUG_LEVEL  1



cMenue::cMenue(int width, int height, Adafruit_GFX* pDisplay) :
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


  devPars.fftLevelMin.init(0, 10000, 200, 0);
  devPars.fftLevelMax.init(0, 300000, 5000, 0);

  devPars.threshHold.init(2.0, 80.0, 1.0, 0);

  for(int i = PARS_BAT; i <= PARS_BIRD; i++)
  {
    devPars.preAmpGain[i].clear();
#ifdef ARDUINO_TEENSY41
    devPars.preAmpType.clear();
    if (hasAmpRevB())
    {
      devPars.preAmpGain[i].addItem(1350);
      devPars.preAmpGain[i].addItem(1351);
      devPars.preAmpGain[i].addItem(1352);
      devPars.preAmpGain[i].addItem(1353);
      devPars.preAmpGain[i].addItem(1354);
      devPars.preAmpGain[i].addItem(1355);
      devPars.preAmpType.addItem(1322);
    }
    else
    {
      devPars.preAmpGain[i].addItem(1331);
      devPars.preAmpGain[i].addItem(1332);
      devPars.preAmpGain[i].addItem(1333);
      devPars.preAmpGain[i].addItem(1334);
      devPars.preAmpType.addItem(1321);
      devPars.preAmpType.addItem(1322);
    }
#endif
#ifdef ARDUINO_TEENSY40
    devPars.preAmpGain[i].addItem(1353);
    devPars.preAmpGain[i].addItem(1355);
#endif

    devPars.recTime[i].init(PAR_RECTIM_MIN, PAR_RECTIM_MAX, 1, 0);
  
    for(int t = 1300; t <= 1308; t++)
      devPars.sampleRate[i].addItem(t);

    devPars.trigFiltFreq[i].init(PAR_TRIGFILTFREQ_MIN, PAR_TRIGFILTFREQ_MAX, 1.0, 0);
    devPars.recFiltFreq[i].init(0, PAR_TRIGFILTFREQ_MAX, 1.0, 0);
    devPars.trigFiltType[i].clear();
    devPars.trigFiltType[i].addItem(1171);
    devPars.trigFiltType[i].addItem(1172);
    devPars.trigFiltType[i].addItem(1173);

    devPars.recFiltType[i].clear();
    devPars.recFiltType[i].addItem(1171);
    devPars.recFiltType[i].addItem(1172);
    devPars.recFiltType[i].addItem(1173);

    devPars.recThreshhold[i].init(PAR_RECTHRESH_MIN, PAR_RECTHRESH_MAX, 1, 0);
    devPars.recThreshhold[i].set(-12);

    devPars.triggerType[i].clear();
    devPars.triggerType[i].addItem(1361);
    devPars.triggerType[i].addItem(1362);
    devPars.triggerType[i].addItem(1363);

    devPars.minEventLen[i].init(PAR_TRIGEVENT_MIN, PAR_TRIGEVENT_MAX, 0.5f, 1);
    devPars.deadTime[i].set(PAR_DEADTIM_MIN);
    devPars.deadTime[i].init(PAR_DEADTIM_MIN, PAR_DEADTIM_MAX, 0.5f, 1);

  }

  devPars.sampleRate[PARS_BAT].set(SR_312K);
  devPars.sampleRate[PARS_BIRD].set(SR_44K);

//  devPars.backLightTime.set(120);
  devPars.backLightTime.init(PAR_BACKLIGHT_MIN, PAR_BACKLIGHT_MAX, 1, 0);
  devPars.knobRotation.clear();
  devPars.knobRotation.addItem(1151);
  devPars.knobRotation.addItem(1152);

  devPars.dispOrient.clear();
  devPars.dispOrient.addItem(1161);
  devPars.dispOrient.addItem(1162);

  devPars.preTrigger.init(PAR_PRETRIG_MIN, PAR_PRETRIG_MAX, 1.0, 0);
  devPars.startH.init(0, 23, 1, 0, 2);
  devPars.startMin.init(0, 59, 1, 0, 2);
  devPars.stopH.init(0, 23, 1, 0, 2);
  devPars.stopMin.init(0, 59, 1, 0, 2);
  devPars.voltFactor.init(0, 1, 0.0000001f, 5);

  devPars.recAuto.clear();
  devPars.recAuto.addItem(1400),
  devPars.recAuto.addItem(1401),
  devPars.recAuto.addItem(1402),
  devPars.recAuto.addItem(1403),
  devPars.recAuto.addItem(1404),
  devPars.recAuto.addItem(1405),
  devPars.recAuto.addItem(1406),
  devPars.recAuto.addItem(1407),
  devPars.recAuto.addItem(1408),

  devPars.sendDelay.init(0, 20, 1, 0);
  devPars.liveAmplitude.init(10,300, 2, 0);

  devPars.srcPosition.clear();
  devPars.srcPosition.addItem(1410);
  devPars.srcPosition.addItem(1412);
  devPars.srcPosition.addItem(1413);

  devPars.menueType.clear();
  devPars.menueType.addItem(1714);
  devPars.menueType.addItem(1713);
 
  devPars.ShutoffVoltage.init(PAR_SHUTOFF_MIN, PAR_SHUTOFF_MAX, 0.05, 2);

  devPars.debugLevel.init(0, 63, 1, 0);
  devPars.timeZone.init(-12.0, +12, 1.0, 0);

  devPars.daylightSav.clear();
  devPars.daylightSav.addItem(16);
  devPars.daylightSav.addItem(15);
  devPars.daylightSav.addItem(1740);

  devPars.displayMode.clear();
  devPars.displayMode.addItem(1156);
  devPars.displayMode.addItem(1157);

  devPars.gpsBaudRate.clear();
  devPars.gpsBaudRate.addItem(1386);
  devPars.gpsBaudRate.addItem(1387);

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
  devStatus.month.init(1,12,1,0, 2);
  devStatus.day.init(1,31,1,0, 2);
  devStatus.hour.init(0,23,1,0, 2);
  devStatus.minute.init(0,59,1,0, 2);

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

  devStatus.peakVal.init(0, 100, 0.1f, 1);
  devStatus.freeSpace.init(0, 256000, 1, 0);
  devStatus.voltage.init(0, 20, 0.01f, 2);
  devStatus.setVoltage.init(0,10, 0.05f, 2);
  devStatus.digits.init(0, 10000, 1, 0);
  devStatus.temperature.init(-50, 100, 0.1f, 1);
  devStatus.humidity.init(0, 100, 1, 0);
  devStatus.mainLoop.init(0, 1e10, 1, 0);

  devStatus.freq1Tick.init(0, 300,0.1f, 1);
  devStatus.satCount.init(0, 24, 1, 0);
  devStatus.height.init(-100, 10000, 5, 0);
  devStatus.posValid.addItem(16);
  devStatus.posValid.addItem(15);
  devStatus.lastCallF.init(0.0f, 200.0f, 0.1f, 1);

  if(hasDisplay() == enDisplayType::OLED_128)
  {
    devStatus.gpsStatus.addItem(1430); //enGpsStatus::GPS_STATUS_OFF
    devStatus.gpsStatus.addItem(1434); //enGpsStatus::GPS_SEARCHING
    devStatus.gpsStatus.addItem(1435); //enGpsStatus::GPS_FIXED
    devStatus.gpsStatus.addItem(1436); //enGpsStatus::GPS_FIXED_OFF
  }
  else
  {
    devStatus.gpsStatus.addItem(1430); //enGpsStatus::GPS_STATUS_OFF
    devStatus.gpsStatus.addItem(1431); //enGpsStatus::GPS_SEARCHING
    devStatus.gpsStatus.addItem(1432); //enGpsStatus::GPS_FIXED
    devStatus.gpsStatus.addItem(1433); //enGpsStatus::GPS_FIXED_OFF
  }
  devStatus.chargeLevel.init(0,99,1,0);
  devStatus.chargeLevel.init(0.0, 99.0, 0.1, 0);
  load();
  setPosFunc(this, enKey::NO, nullptr);

  if (checkTwilight((enRecAuto) devPars.recAuto.get()))
    calcSunrise();
  devStatus.time.set();
  devStatus.date.set();
}

extern Adafruit_SSD1327 oled;
void cMenue::refreshDisplay()
{
  if((hasDisplay() == enDisplayType::OLED_128))
    oled.display();
}

void cMenue::initFileRelatedParams()
{
  initBats();
  notes1.initNotes(PATH_NOTES1, 2000, 2005);
  notes1.initListPar(devStatus.notes1);
  notes2.initNotes(PATH_NOTES2, 2010, 2013);
  notes2.initListPar(devStatus.notes2);
}

void cMenue::setFactoryDefaults(enMode mode)
{
  devPars.volume.set(6.0f);              ///< volume setting
  devPars.mixFreq.set(40.0f);              ///< mixer frequency
  devPars.recAuto.set(0);
  devPars.sendDelay.set(2.0);
  devPars.srcPosition.set(static_cast<uint32_t>(enPositionMode::FIX));
  devPars.menueType.set(enMenueType::COMPACT);
  devPars.knobRotation.set(enKnobRot::CLOCKWISE);
  devPars.threshHold.set(10.0f);           ///< threshhold level graph, waterfall
  devPars.fftLevelMin.set(3500.0f);        ///< low (threshhold) level for FFT display
  devPars.fftLevelMax.set(70000.0f);       ///< high level for FFT display
  devPars.dispOrient.set(enDispOrient::RIGHT_HAND); ///< display orientation
  devPars.preTrigger.set(20.0f);           ///< pre trigger time [ms]
  devPars.displayMode.set(static_cast<uint32_t>(enDispMode::NORMAL));
  devPars.backLightTime.set(120.0);       ///< time for backlight
  devPars.lang.set(enLang::LANG_GER);        ///< display language
  devPars.debugLevel.set(0);
  devStatus.geoPos.setLat(49.5);
  devStatus.geoPos.setLon(8.3);
  devPars.startH.set(21);               ///< hour of start time
  devPars.startMin.set(0);              ///< minute of start time
  devPars.stopH.set(6);                 ///< hour of start time
  devPars.stopMin.set(0);               ///< minute of start time
  devPars.timeZone.set(1);
  devPars.daylightSav.set(static_cast<uint32_t>(enDlSaving::AUTO));
  devPars.liveAmplitude.set(50);        ///< max. amplitude for live display
  devStatus.height.set(100);
  devPars.recTime[PARS_BAT].set(3.0f);               ///< recording time
  devPars.sampleRate[PARS_BAT].set(enSampleRate::SR_384K);    ///< sample rate
  devPars.recThreshhold[PARS_BAT].set(-18.0f);       ///< auto recording threshhold
  devPars.deadTime[PARS_BAT].set(3.0);              ///< timeout after one recording
  devPars.trigFiltFreq[PARS_BAT].set(16.0);             ///< hight pass freq for recording trigger
  devPars.trigFiltType[PARS_BAT].set(enFiltType::HIGHPASS);             ///< filter type for recording trigger
  devPars.recFiltFreq[PARS_BAT].set(1.0);             ///< hight pass freq for recording trigger
  devPars.recFiltType[PARS_BAT].set(enFiltType::HIGHPASS);             ///< filter type for recording trigger
  devPars.triggerType[PARS_BAT].set(enTrigType::LEVEL);   ///< trigger type for recording
  devPars.minEventLen[PARS_BAT].set(1.0f);         ///< minimal event length for trigger
  devPars.preAmpGain[PARS_BAT].set(static_cast<uint32_t>(enGainRevC::GAINC_58DB));
  devPars.recTime[PARS_BIRD].set(5.0f);               ///< recording time
  devPars.sampleRate[PARS_BIRD].set(enSampleRate::SR_44K);    ///< sample rate
  devPars.recThreshhold[PARS_BIRD].set(-18.0f);       ///< auto recording threshhold
  devPars.deadTime[PARS_BIRD].set(3.0);              ///< timeout after one recording
  devPars.trigFiltFreq[PARS_BIRD].set(5.0);             ///< hight pass freq for recording trigger
  devPars.trigFiltType[PARS_BIRD].set(enFiltType::LOWPASS);             ///< filter type for recording trigger
  devPars.recFiltFreq[PARS_BIRD].set(20.0);             ///< hight pass freq for recording trigger
  devPars.recFiltType[PARS_BIRD].set(enFiltType::LOWPASS);             ///< filter type for recording trigger
  devPars.triggerType[PARS_BIRD].set(enTrigType::LEVEL);   ///< trigger type for recording
  devPars.minEventLen[PARS_BIRD].set(10.0f);         ///< minimal event length for trigger
  devPars.preAmpGain[PARS_BIRD].set(static_cast<uint32_t>(enGainRevC::GAINC_58DB));

  devPars.ShutoffVoltage.set(5.8f);
//  devPars.voltFactor.set(1);
  devPars.gpsBaudRate.set(static_cast<uint32_t>(enGpsBaudRate::BD_9600));
}

void MEMP cMenue::initDialogs()
{
  if(hasDisplay() == enDisplayType::NO_DISPLAY)
    return;
  clearPanelList();
  refreshFkeyPanel();
  tCoord lf = LINE_HEIGHT_TFT;     ///< distance between two lines of text
  switch (hasDisplay())
  {
    break;
    case enDisplayType::OLED_128:
      lf = LINE_HEIGHT_OLED;
      break;
    case enDisplayType::TFT_320:
      lf = LINE_HEIGHT_TFT;
      break;
    case enDisplayType::NO_DISPLAY:
      break;

  }
  // Header for main panel
  setHdrPanel(createPanel(PNL_HEADER, 0, 0, getWidth(), getHdrHeight()));
  setHeaderPanelText(this, 100);


  switch (devPars.menueType.get())
  {
    default:
    case enMenueType::COMPACT:
      initFunctionsCompact();
      initCompactPanels(lf);
      break;
    case enMenueType::HANDHELD:
      initFunctionItemsHandheld();
      initHandheldPanels(lf);
      break;
  }
}


int MEMP cMenue::initHandheldPanels(tCoord lf)
{
  fkeyMainPan = createPanel(PNL_FKEYS, 0, 226, DISP_WIDTH_TFT, getFkeypanHeight());
  setFkeyPanel(fkeyMainPan);
  int err = initFkeyPanel(getPan(fkeyMainPan), lf);

  // main panel
  panGeo = createPanel(PNL_MAIN, 0, getHdrHeight(), DISP_WIDTH_TFT, DISP_HEIGHT_TFT - getFkeypanHeight() - getHdrHeight() + 1);
  err |= initMainPanelExpert(getPan(panGeo), lf);

  // F-KEYs for waterfall panel
  fkeyWaterPan = createPanel(PNL_FKEYS, 0, 226, DISP_WIDTH_TFT, getFkeypanHeight());
  err |= initFkeysWaterPan(getPan(fkeyWaterPan), lf);

  hdrPanWaterfall = createPanel(PNL_HEADER, 0, 0, DISP_WIDTH_TFT, getHdrHeight());
  err |= getPan(hdrPanWaterfall)->addTextItem(205, 3, 1, 35, lf);
  err |= getPan(hdrPanWaterfall)->addStrItem(&devPars.fileName, 38, 1, 310, lf);

  panWaterfall = createPanel(PNL_MAIN, 0, getFkeypanHeight() + 1, DISP_WIDTH_TFT, DISP_HEIGHT_TFT - getFkeypanHeight() * 2 - 1);
  err |= initWaterPan(getPan(panWaterfall), lf);

  // x-t-diagram panel
  panTime = createPanel(PNL_MAIN, 0, getFkeypanHeight() + 1, DISP_WIDTH_TFT, DISP_HEIGHT_TFT - getFkeypanHeight() * 2 - 1);
  err |= initTimePan(getPan(panTime), lf);

  pnlLive = createPanel(PNL_MAIN, 0, getFkeypanHeight() + 1, DISP_WIDTH_TFT, DISP_HEIGHT_TFT - getFkeypanHeight() * 2 - 1);
  err |= initLivePanHandheld(getPan(pnlLive), lf);

  hdrBatInfo = createPanel(PNL_HEADER, 0, 0, DISP_WIDTH_TFT, getHdrHeight());
  err |= getPan(hdrBatInfo)->addTextItem(1500, 3, 1, 80, lf);
  err |= getPan(hdrBatInfo)->addStrItem(&devStatus.bats.nameLat, 95, 1, 225, lf);


  panInfo = createPanel(PNL_MAIN, 0, getFkeypanHeight() + 1, DISP_WIDTH_TFT, DISP_HEIGHT_TFT - getFkeypanHeight() * 2 - 1);
  err |= initInfoPanRecorder(getPan(panInfo), lf);

  // parameter panel
  panParams = createPanel(PNL_MAIN, 0, getFkeypanHeight() + 1, DISP_WIDTH_TFT, DISP_HEIGHT_TFT - getFkeypanHeight() * 2 - 1);
  err |= initParPan(getPan(panParams), lf);

  panParRecNight = createPanel(PNL_MAIN, 0, getFkeypanHeight() + 1, DISP_WIDTH_TFT, DISP_HEIGHT_TFT - getFkeypanHeight() * 2 - 1);
  err |= initParRec(getPan(panParRecNight), lf);

  panPosition = createPanel(PNL_MAIN, 0, getFkeypanHeight() + 1, DISP_WIDTH_TFT, DISP_HEIGHT_TFT - getFkeypanHeight() * 2 - 1);
  err |= initPositionPan(getPan(panPosition), lf);

  panBats = createPanel(PNL_MAIN, 0, getFkeypanHeight() + 1, DISP_WIDTH_TFT, DISP_HEIGHT_TFT - getFkeypanHeight() * 2 - 1);
  err |= initBatPan(getPan(panBats), lf);

  panDateTime = createPanel(PNL_MAIN, 0, getFkeypanHeight() + 1, DISP_WIDTH_TFT, DISP_HEIGHT_TFT - getFkeypanHeight() * 2 - 1);
  err |= initDateTimePan(getPan(panDateTime), lf);

  fkeyFilePan = createPanel(PNL_FKEYS, 0, 226, DISP_WIDTH_TFT, getFkeypanHeight());
  err |= initFkeyFilePanel(getPan(fkeyFilePan), lf);

  panFileBrowser = createPanel(PNL_MAIN, 0, getFkeypanHeight() + 1, DISP_WIDTH_TFT, DISP_HEIGHT_TFT - getFkeypanHeight() * 2 - 1);
  err |= initFileBrowserPan(getPan(panFileBrowser), lf);
  initFileBrowser(getPan(panFileBrowser), "/");

  setMainPanel(panGeo);

  return err;
}

int MEMP cMenue::initCompactPanels(tCoord lf)
{
  int err = 0;
  panInfo = createPanel(PNL_MAIN, 0, getHdrHeight(), getWidth(), getHeight() - getFkeypanHeight() - getHdrHeight());
  err |= initInfoPanCompact(getPan(panInfo), lf);

  // F-KEYs for main panel
  fkeyMainPan = createPanel(PNL_FKEYS, 0, getHeight() - getFkeypanHeight(), DISP_WIDTH_OLED, getFkeypanHeight());
  err |= initCompactFkeyPanel(getPan(fkeyMainPan), lf);

  // main panel
  panGeo = createPanel(PNL_MAIN, 0, getHdrHeight(), getWidth(), getHeight() - getFkeypanHeight() - getHdrHeight());
  err |= initMainPanelCompact(getPan(panGeo), lf);
  setMainPanel(panGeo);
  setFkeyPanel(fkeyMainPan);
  enableEditPosition(this, devPars.srcPosition.get() == static_cast<uint32_t>(enPositionMode::FIX));
  enableEditTimes(this, (devPars.recAuto.get() != enRecAuto::OFF) && (devPars.recAuto.get() != enRecAuto::ON_BIRD) && 
                  (devPars.recAuto.get() != enRecAuto::ON_BAT));

  panParams = createPanel(PNL_MAIN, 0, getHdrHeight(), getWidth(), getHeight() - getFkeypanHeight() - getHdrHeight());
  err |= initParPanCompact(getPan(panParams), lf);

  panParRecNight = createPanel(PNL_MAIN, 0, getHdrHeight(), getWidth(), getHeight() - getFkeypanHeight() - getHdrHeight());
  err |= initParRecCompact(getPan(panParRecNight), lf, PARS_BAT);

  panParTrigNight = createPanel(PNL_MAIN, 0, getHdrHeight(), getWidth(), getHeight() - getFkeypanHeight() - getHdrHeight());
  err |= initParTriggerCompact(getPan(panParTrigNight), lf, PARS_BAT);

  panParRecDay = createPanel(PNL_MAIN, 0, getHdrHeight(), getWidth(), getHeight() - getFkeypanHeight() - getHdrHeight());
  err |= initParRecCompact(getPan(panParRecDay), lf, PARS_BIRD);

  panParTrigDay = createPanel(PNL_MAIN, 0, getHdrHeight(), getWidth(), getHeight() - getFkeypanHeight() - getHdrHeight());
  err |= initParTriggerCompact(getPan(panParTrigDay), lf, PARS_BIRD);

  panDateTime = createPanel(PNL_MAIN, 0, getHdrHeight(), getWidth(), getHeight() - getFkeypanHeight() - getHdrHeight());
  err |= initDateTimePanCompact(getPan(panDateTime), lf);

  return err;
}


void cMenue::load()
{
  loadParsFromEep();
}

void cMenue::save()
{
  saveParsToEep();
}

void MEMP cMenue::printPars(size_t parSet)
{
  Serial.printf("volume             [dB]: %.0f\n", devPars.volume.get());
  Serial.printf("mixer frequency   [kHz]: %.0f\n", devPars.mixFreq.get());
  Serial.printf("recording time      [s]: %.1f\n", devPars.recTime[parSet].get());
  Serial.printf("sampling rate     [kHz]: %s\n", devPars.sampleRate[parSet].getActText());
  Serial.printf("pre trigger        [ms]: %.0f\n", devPars.preTrigger.get());
  Serial.printf("trigger level       [%]: %.3f\n", devPars.recThreshhold[parSet].get() * 100);
  Serial.printf("pre amp gain           : %s\n", devPars.preAmpGain[parSet].getActText());
}


void MEMP cMenue::printStatus()
{
 // enCassMode cassMode = STOP; ///< mode of operation of cassette player
  Serial.printf("avg. audio CPU usage: %.2f\n", devStatus.cpuAudioAvg.get());
  Serial.printf("max. audio CPU usage: %.2f\n", devStatus.cpuAudioMax.get());
  Serial.printf("audio memory usage:   %.0f\n", devStatus.audioMem.get());
}


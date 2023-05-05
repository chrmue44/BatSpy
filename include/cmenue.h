/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * The complete menue system needed to operate BatSpy based
 * on the library MenueSystem
 * ***********************************************************/
#ifndef CMENUE_H
#define CMENUE_H
#include <Arduino.h>
#include <EEPROM.h>
#include <MenueSystem.h>
#include "types.h"
#include "ctext.h"
#include "cCassette.h"
#include "cAudio.h"
#include "cRtc.h"

#define FREQ_MIN   0
#define FREQ_MAX   150
#define MAX_ADC             0x7FFF
#define DEC_TIM_MIN   5   ///< nr of decimals for left border of graph

#define DIR_PAN_SIZE  10

/**
 * @brief operation modes of the bat detector
 */

/**
 * @brief The stBatInfo struct
 */
struct stBatInfo
{
  stBatInfo() {}
  cParList name = 0;
  cParStr nameLat = "";
  cParStr freq = "";
  cParStr callLen = "";
  cParStr callDist = "";
  cParStr characteristic = "";
  cParStr wingSpan = "";
  cParStr weight = "";
  cParStr size = "";
  cParStr habitat = "";
  cParStr comment = "";
  cParStr occurrence = "";
};


/**
 * @brief device status
 */
struct stStatus
{
  stStatus() :
  graph(enGraphMode::GRAPH_XT),
  waterf(enGraphMode::GRAPH_FFT),
  grafLive(enGraphMode::LIVE_FFT)
  {  
  }

  cParEnum opMode = enOpMode::HEAR_HET; ///< display mode
  cParNum recCount = 0;           ///< nr of recordings
  cParGeoPos geoPos;              ///< actual geographic position
  cParNum satCount = 0;           ///< number of satellites
  cParNum height = 0;             ///< height [m]
  cParEnum posValid = 0;
  cParEnum playStatus = 0;        ///< status Aufnahme/Abspielen
  cParNum timMin = 0;             ///< left border of time-, waterfall-diagram
  cParNum timMax = 2.5;           ///< right border of time-, waterfall-diagram
  cParNum timStep = 0.5;          ///< time step for cursor in time-, waterfall-diagram
  cParNum cpuAudioAvg = 0;        ///< average audio CPU usage
  cParNum cpuAudioMax = 0;        ///< maximum audio CPU usage
  cParNum audioMem = 0;           ///< audio memory usage
  cParTime time;                  ///< actual time
  cParDate date;                  ///< actual date
  cParGraph graph;                ///< x-t-diagram
  cParNum pulseWidth = 0;         ///< measured pulse width
  cParStr measUnit = "ms";
  cParGraph waterf;               ///< waterfall diagram
  int forceDisplay = 0;           ///< force display activity
  cParNum amplMax = 100;          ///< max. amplitude for td display
  cParNum amplMin = -100;         ///< min. amplitude for td display
  stBatInfo bats;                 ///< struct to store bat informations
  tDirInfo dir;                   ///< content of current dir
  size_t fileIndex = 0;           ///< index of file in dir
  size_t startIndex = 0;          ///< start index of directory
  cParNum year = 2021;
  cParNum month = 7;
  cParNum day = 13;
  cParNum hour = 22;
  cParNum minute = 0;
  cParEnum lonSign = 0;
  cParNum lonDeg = 8;
  cParNum lonMin = 37;
  cParNum lonSec = 925;
  cParEnum latSign = 0;
  cParNum latDeg = 49;
  cParNum latMin = 45;
  cParNum latSec = 236;
  cParNum freqMin = FREQ_MIN;     ///< min freq for waterfall diagram
  cParNum freq1Tick = (FREQ_MAX - FREQ_MIN)/Y_TICK_CNT;
  cParNum freqMax = FREQ_MAX;     ///< max freq for waterfall diagram
  cParNum peakVal = 0;
  cParStr version = "BatSpy " __DATE__  "/"  __TIME__;
  cParStr hwVersion = "";
  cParNum freeSpace = 0;
  cParNum voltage = 0;
  cParNum digits = 0;
  cParNum temperature = 0;
  cParGraph grafLive;
  cParEnum dirFilter = 0;
  cParStr dirFiles[DIR_PAN_SIZE];
  cParList notes1 = 0;
  cParList notes2 = 0;
  cParNum lastCallF = 0.0;
  cParList liveMsPerDiv = 0;
};


/**
 * @brief device parameters
 */
struct stParams
{
  cParNum volume =   6;              ///< volume setting
  cParNum mixFreq = 40;              ///< mixer frequency
  cParNum recTime = 3;               ///< recording time
  cParEnum sampleRate = enSampleRate::SR_384K;    ///< sample rate
  cParEnum lang   = LANG_GER;        ///< display language
  cParList dirSel = 0;               ///< select directory
  cParList fileSel = 0;              ///< select file
  cParStr dir = "";                  ///< actual directory on SD card
  cParStr fileName = "";             ///< actual file name
  cParNum threshHold = 10;           ///< threshhold level graph, waterfall
  cParNum fftLevelMin = 3500;        ///< low (threshhold) level for FFT display
  cParNum fftLevelMax = 70000;       ///< high level for FFT display
  cParNum recThreshhold = -18;       ///< auto recording threshhold
  cParNum deafTime = 3;              ///< timeout after one recording
  cParNum backLightTime = 120;       ///< time for backlight
  cParEnum preAmpGain = 1;           ///< gain of pre amplifier
  cParEnum preAmpType = enPreAmp::HIGH_PASS;      ///< type of pre amplifier
  cParEnum dispOrient = enDispOrient::RIGHT_HAND; ///< display orientation
  cParEnum knobRotation = enKnobRot::CLOCKWISE;   ///< knob rotation
  cParNum preTrigger = 20;           ///< pre trigger time [ms]
  cParEnum recFmt = 0;               ///< recording format
  cParNum filtFreq = 16;             ///< hight pass freq for recording trigger
  cParEnum filtType = 0;             ///< filter type for recording trigger
  cParNum startH = 21;               ///< hour of start time
  cParNum startMin = 0;              ///< minute of start time
  cParNum stopH = 6;                 ///< hour of start time
  cParNum stopMin = 0;               ///< minute of start time
  cParNum voltFactor = 1;            ///< factor digits to voltage
  cParEnum recAuto = 0;              ///< mode for automatic recording
  bool sunriseCalc = false;          ///< true if sunrise and sunset are calculated
  cParNum sendDelay = 2;             ///< delay [ms] after sending 2048 Bytes on USB
  cParNum liveAmplitude = 50;        ///< max. amplitude for live display
  cParEnum projectType = 0;          ///< type of project for automatic recording
  cParEnum srcPosition = enSrcPosition::FIX;  ///< source of position (fixed, GPS)
  cParEnum menueType = enMenueType::EXPERT;   ///< menue type
  cParEnum triggerType = enTrigType::LEVEL;   ///< trigger type for recording
  cParNum minEventLen = 1.0;         ///< minimal event length for trigger
};


class cMenue : public cMenuesystem
{
 public:
  cMenue(int width, int height, ILI9341_t3* pDisplay);
  virtual ~cMenue();
  virtual void initDialogs();
  virtual void save();
  virtual void load();
  void printPars();
  void printStatus();
  static int16_t readInt16FromEep(int32_t addr);

protected:
  virtual void initPars();

private:
  bool checkCRC();
  void loadLanguage();
  int initExpertPanels(tCoord lf);
  int initRecorderPanels(tCoord lf);
  int initHandheldPanels(tCoord lf);

};

#endif // CMENUE_H

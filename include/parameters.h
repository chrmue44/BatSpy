/*************************************************************
 * BatSpy: Teensy 4.x based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2023 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include "MenueSystem.h"

#define FREQ_MIN   0
#define FREQ_MAX   150
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
  cParStr version = "BatSpy " __DATE__/*  "/"  __TIME__*/;
  cParStr hwVersion = "";
  cParNum freeSpace = 0;
  cParNum voltage = 0;
  cParNum setVoltage = 0;
  cParNum digits = 0;
  cParNum temperature = 0;
  cParNum humidity = 0;
  cParGraph grafLive;
  cParEnum dirFilter = 0;
  cParStr dirFiles[DIR_PAN_SIZE];
  cParList notes1 = 0;
  cParList notes2 = 0;
  cParNum lastCallF = 0.0;
  cParList liveMsPerDiv = 0;
  cParNum mainLoop = 0;
  cParEnum gpsStatus = enGpsStatus::GPS_STATUS_OFF;
  cParNum chargeLevel = 0;
  cParStr batSymbol = "\xEC\xED\xED\xED\xED\xED\xED\xED\xED\xED\xED\xEF";
  cParStr recStatus = "\xF1";
};


/**
 * @brief device parameters
 */
struct stParams
{
  cParNum volume =   6;              ///< volume setting
  cParNum mixFreq = 40;              ///< mixer frequency
  cParNum recTime = 3;               ///< recording time
#define PAR_RECTIM_MIN  1.0f
#define PAR_RECTIM_MAX  30.0f
  cParEnum sampleRate = enSampleRate::SR_384K;    ///< sample rate
#define PAR_SR_MIN  enSampleRate::SR_19K
#define PAR_SR_MAX  enSampleRate::SR_480K
  cParEnum lang   = LANG_GER;        ///< display language
  cParStr fileName = "";             ///< actual file name
  cParNum threshHold = 10;           ///< threshhold level graph, waterfall
  cParNum fftLevelMin = 3500;        ///< low (threshhold) level for FFT display
  cParNum fftLevelMax = 70000;       ///< high level for FFT display
  cParNum recThreshhold = -18;       ///< auto recording threshhold
#define PAR_RECTHRESH_MIN -24.0f
#define PAR_RECTHRESH_MAX -1.0f
  cParNum deadTime = 3;              ///< timeout after one recording
#define PAR_DEADTIM_MIN  0.3f
#define PAR_DEADTIM_MAX  30.0f
  cParNum backLightTime = 120;       ///< time for backlight
  cParEnum displayMode = enDispMode::DISP_NORMAL;   ///< display mode
#define PAR_BACKLIGHT_MIN     5.0f
#define PAR_BACKLIGHT_MAX   300.0f
  cParEnum preAmpGain = 1;           ///< gain of pre amplifier
#define PAR_GAIN_MIN   0
#define PAR_GAIN_MAX   1
  cParEnum dispOrient = enDispOrient::RIGHT_HAND; ///< display orientation
  cParEnum knobRotation = enKnobRot::CLOCKWISE;   ///< knob rotation
  cParNum preTrigger = 20;           ///< pre trigger time [ms]
#define PAR_PRETRIG_MIN 0.0
#define PAR_PRETRIG_MAX 150.0
  cParNum recFiltFreq = 16;          ///< hight pass freq for recording trigger
  cParNum trigFiltFreq = 16; 
#define PAR_TRIGFILTFREQ_MIN   1.0f
#define PAR_TRIGFILTFREQ_MAX  70.0f
  cParEnum recFiltType = 0;          ///< filter type for recording trigger
  cParEnum trigFiltType = 0;         ///< filter type for recording trigger
#define PAR_TRIGFILTTYPE_MIN   0
#define PAR_TRIGFILTTYPE_MAX   2
  cParNum startH = 21;               ///< hour of start time
  cParNum startMin = 0;              ///< minute of start time
  cParNum stopH = 6;                 ///< hour of start time
  cParNum stopMin = 0;               ///< minute of start time
  cParNum voltFactor = 1;            ///< factor digits to voltage
  cParEnum recAuto = 0;              ///< mode for automatic recording
#define PAR_AUTOMODE_MIN  0
#define PAR_AUTOMODE_MAX  3
  bool sunriseCalc = false;          ///< true if sunrise and sunset are calculated
  cParNum sendDelay = 2;             ///< delay [ms] after sending 2048 Bytes on USB
  cParNum liveAmplitude = 50;        ///< max. amplitude for live display
  cParEnum srcPosition = static_cast<uint32_t>(enPositionMode::FIX);  ///< source of position (fixed, GPS)
#define PAR_LOCSRC_MIN   0
#define PAR_LOCSRC_MAX   1
  cParEnum menueType = enMenueType::EXPERT;   ///< menue type
  cParEnum triggerType = enTrigType::LEVEL;   ///< trigger type for recording
#define PAR_TRIGTYPE_MIN  0
#define PAR_TRIGTYPE_MAX  2
  cParNum minEventLen = 1.0;         ///< minimal event length for trigger
#define PAR_TRIGEVENT_MIN  0.5f
#define PAR_TRIGEVENT_MAX  50.0f
  cParNum ShutoffVoltage = 5.4f;
#define PAR_SHUTOFF_MIN  3.6f
#define PAR_SHUTOFF_MAX  8.0f
  cParEnum debugLevel = 0;
  cParNum timeZone = 0;
  cParEnum daylightSav = static_cast<uint32_t>(enDlSaving::OFF);
  cParEnum gpsBaudRate = static_cast<uint32_t>(enGpsBaudRate::BD_9600);
#define PAR_GPS_BR_MIN  static_cast<uint32_t>(enGpsBaudRate::BD_9600)
#define PAR_GPS_BR_MAX  static_cast<uint32_t>(enGpsBaudRate::BD_115200)
};

#endif //#ifndef _PARAMETERS_H_

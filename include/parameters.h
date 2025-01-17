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
#include "types.h"

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
  cParStr micId = "unknown";      ///< Microphone ID
  cParStr micType = "";           ///< Microphone Type
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
  cParStr battSymbol = "\xEC\xED\xED\xED\xED\xED\xED\xED\xED\xED\xED\xEF";
  cParStr recStatus = "\xF1";
};


#define DBG_COMMANDS 0x01
#define DBG_GPS      0x02
#define DBG_SYSTEM   0x04
#define DBG_TRIGGER  0x08
#define DBG_PERM_REC 0x10

/**
 * @brief device parameters
 */
struct stParams
{
  bool checkDebugLevel(int l)
  {
    int w = debugLevel.get();
    if (w & l)
      return true;
    else
      return false;
  }

  cParNum volume =   6;              ///< volume setting
  cParNum mixFreq = 40;              ///< mixer frequency
  cParNum recTime[2];               ///< recording time
#define PAR_RECTIM_MIN  1.0f
#define PAR_RECTIM_MAX  30.0f
  cParEnum sampleRate[2];    ///< sample rate
#define PAR_SR_MIN  enSampleRate::SR_19K
#define PAR_SR_MAX  enSampleRate::SR_480K
#define PAR_SR_DEF_BAT  enSampleRate::SR_384K
#define PAR_SR_DEF_BIRD  enSampleRate::SR_44K
  cParEnum lang   = LANG_GER;        ///< display language
  cParStr fileName = "";             ///< actual file name
  cParNum threshHold = 10;           ///< threshhold level graph, waterfall
  cParNum fftLevelMin = 3500;        ///< low (threshhold) level for FFT display
  cParNum fftLevelMax = 70000;       ///< high level for FFT display
  cParNum recThreshhold[2];       ///< auto recording threshhold
#define PAR_RECTHRESH_MIN -24.0f
#define PAR_RECTHRESH_MAX -1.0f
#define PAR_RECTHRESH_DEF -18.0f
  cParNum deadTime[2];              ///< timeout after one recording
#define PAR_DEADTIM_MIN  0.5f
#define PAR_DEADTIM_MAX  30.0f
#define PAR_DEADTIM_DEF  3.0f
  cParNum backLightTime = 120;       ///< time for backlight
  cParEnum displayMode = static_cast<uint32_t>(enDispMode::NORMAL);   ///< display mode
#define PAR_BACKLIGHT_MIN     5.0f
#define PAR_BACKLIGHT_MAX   300.0f
  cParEnum preAmpGain[2];           ///< gain of pre amplifier
#define PAR_GAIN_MIN   0
#define PAR_GAIN_MAX   1
  cParEnum dispOrient = enDispOrient::RIGHT_HAND; ///< display orientation
  cParEnum knobRotation = enKnobRot::CLOCKWISE;   ///< knob rotation
  cParNum preTrigger = 20;           ///< pre trigger time [ms]
#define PAR_PRETRIG_MIN 0.0
#define PAR_PRETRIG_MAX 150.0
  cParNum recFiltFreq[2];          ///< hight pass freq for recording trigger
  cParNum trigFiltFreq[2]; 
#define PAR_TRIGFILTFREQ_MIN   1.0f
#define PAR_TRIGFILTFREQ_MAX  70.0f
#define PAR_TRIGFILTFREQ_DEF_BAT  16.0f
#define PAR_TRIGFILTFREQ_DEF_BIRD  5.0f
  cParEnum recFiltType[2];          ///< filter type for recording trigger
  cParEnum trigFiltType[2];         ///< filter type for recording trigger
#define PAR_TRIGFILTTYPE_MIN   0
#define PAR_TRIGFILTTYPE_MAX   2
#define PAR_TRIGFILTTYPE_DEF   0
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
  cParEnum menueType = enMenueType::COMPACT;   ///< menue type
  cParEnum triggerType[2];   ///< trigger type for recording
#define PAR_TRIGTYPE_MIN  0
#define PAR_TRIGTYPE_MAX  2
#define PAR_TRIGTYPE_DEF  enTrigType::LEVEL

  cParNum minEventLen[2];         ///< minimal event length for trigger
#define PAR_TRIGEVENT_MIN  0.5f
#define PAR_TRIGEVENT_MAX  50.0f
#define PAR_TRIGEVENT_DEF_BAT  1.0f
#define PAR_TRIGEVENT_DEF_BIRD  10.0f
  cParNum ShutoffVoltage = 5.4f;
#define PAR_SHUTOFF_MIN  3.6f
#define PAR_SHUTOFF_MAX  8.0f
  cParNum debugLevel = 0;
  cParNum timeZone = 0;
  cParEnum daylightSav = static_cast<uint32_t>(enDlSaving::OFF);
  cParEnum gpsBaudRate = static_cast<uint32_t>(enGpsBaudRate::BD_9600);
#define PAR_GPS_BR_MIN  static_cast<uint32_t>(enGpsBaudRate::BD_9600)
#define PAR_GPS_BR_MAX  static_cast<uint32_t>(enGpsBaudRate::BD_115200)

  stParams()
  {
    sampleRate[PARS_BAT].set(PAR_SR_DEF_BAT) ;    ///< sample rate
    sampleRate[PARS_BIRD].set(PAR_SR_DEF_BIRD) ;    ///< sample rate
    recThreshhold[PARS_BAT].set(PAR_RECTHRESH_DEF);       ///< auto recording threshhold
    recThreshhold[PARS_BIRD].set(PAR_RECTHRESH_DEF);       ///< auto recording threshhold
    deadTime[PARS_BAT].set(PAR_DEADTIM_DEF);              ///< timeout after one recording
    deadTime[PARS_BIRD].set(PAR_DEADTIM_DEF);              ///< timeout after one recording
    recFiltFreq[PARS_BAT].set(PAR_TRIGFILTFREQ_DEF_BAT);          ///< hight pass freq for recording trigger
    recFiltFreq[PARS_BIRD].set(PAR_TRIGFILTFREQ_DEF_BIRD);          ///< hight pass freq for recording trigger
    trigFiltFreq[PARS_BAT].set(PAR_TRIGFILTFREQ_DEF_BAT); 
    trigFiltFreq[PARS_BIRD].set(PAR_TRIGFILTFREQ_DEF_BIRD); 
    triggerType[PARS_BAT].set(PAR_TRIGFILTTYPE_DEF);   ///< trigger type for recording
    triggerType[PARS_BIRD].set(PAR_TRIGFILTTYPE_DEF);   ///< trigger type for recording
    minEventLen[PARS_BAT].set(PAR_TRIGEVENT_DEF_BAT);         ///< minimal event length for trigger
    minEventLen[PARS_BIRD].set(PAR_TRIGEVENT_DEF_BIRD);         ///< minimal event length for trigger
    preAmpGain[PARS_BAT].set(1);
    preAmpGain[PARS_BIRD].set(1);
  }

};

#endif //#ifndef _PARAMETERS_H_

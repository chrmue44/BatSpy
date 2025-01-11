/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * 
 * ***********************************************************/
#ifndef _TYPES_H
#define _TYPES_H

// operation modes of recorder
enum enCassMode
{
  STOP = 1,
  PLAY = 2,
  REC  = 3
};

// selection of available sample rates
enum enSampleRate 
{
//  SR_17K =      0,
  SR_19K =      0,
//  SR_23K =      0,
//  SR_28K =      3,
  SR_32K =      1,
//  SR_35K =      5,
  SR_38K =      2,
  SR_44K =      3,
  SR_48K =      4,
//  SR_88K =      9,
//  SR_96K =     10,
//  SR_176K =    11,
  SR_192K =    5,
//  SR_234K =    6,
//  SR_281K =    14,
  SR_312K =    6,
//  SR_352K =    16,
  SR_384K =    7,
  SR_480K =    8
};

// available hardware gain settings for pre amp
enum enGain
{
  GAIN_27DB            = 0,
  GAIN_36DB            = 1,
  GAIN_45DB            = 2,
  GAIN_54DB            = 3,
};

// available hardware gain settings for pre amp Version 2
enum enGainRevB
{
  GAIN_30DB            = 0,
  GAIN_33DB            = 1,
  GAIN_40DB            = 2,
  GAIN_48DB            = 3,
  GAIN_53DB            = 4,
  GAIN_58DB            = 5
};

// available hardware gain settings for revision C
enum class enGainRevC
{
  GAINC_48DB            = 0,
  GAINC_58DB            = 1
};


// display orientation
enum enDispOrient
{
  LEFT_HAND              = 0,
  RIGHT_HAND             = 1
};

enum class enDispMode
{
  NORMAL            = 0,
  INVERSE           = 1
};

// knob rotation
enum enKnobRot
{
  CLOCKWISE              = 0,
  COUNTERCLOCKW          = 1
};

// available operation modes
enum enOpMode
{
  HEAR_DIRECT     = 0,
  HEAR_HET        = 1,
  PLAY_DIRECT     = 2,
  PLAY_STRETCHED  = 3,
  PLAY_HET        = 4,
};

//filter type
enum enFiltType
{
  HIGHPASS  = 0,
  LOWPASS   = 1,
  BANDPASS  = 2
};

// automatic recording
enum enRecAuto
{
  OFF            = 0,
  ON_BAT         = 1,
  ON_BIRD        = 2,
  TIME_ALL       = 3,
  TIME_BATS      = 4,
  TIME_BIRDS     = 5,
  TWILIGHT_ALL   = 6,
  TWILIGHT_BATS  = 7,
  TWILIGHT_BIRDS = 8
};


enum enRecStatus
{
  REC_OFF   = 0,
  REC_BATS = 1,
  REC_BIRDS   = 2
};

enum class enPlayStatus
{
  STOP    = 0,
  PLAY    = 1,
  REC     = 2,
  TIMEOUT = 3,
};


/// menu level
enum enMenueType
{
  COMPACT  = 0,
  HANDHELD = 1
};


/// type of recording trigger
enum enTrigType
{
  LEVEL      = 0,  ///< trigger when level exceeded
  FREQUENCY  = 1,  ///< trigger when frequency exceeded
  FREQ_LEVEL = 2   ///<trigger when frequency and level exceeded
};

/// source of position information
enum class enPositionMode
{
  FIX = 0,
  GPS_ON = 1,
  GPS_AUTO = 2,
};

enum class enDisplayType
{
  NO_DISPLAY = 0,
  OLED_128 = 1,
  TFT_320 = 2
};

enum class enDlSaving
{
  OFF  = 0,
  ON   = 1,
  AUTO = 2,
};

enum enCpuMode
{
  CPU_MODE_INIT = 0,
  RECORDING     = 1,
  POWER_SAVE    = 2
};

// states of the main state machine
enum class enMainState
{
  INIT    = 0,
  IDLE    = 1,
  RECORD  = 2,
  PLAY    = 3,
  COMMAND = 4,
};

enum class enCmd
{
	LOG,
	SHUTDOWN,
	UPDATE_INFOS,
  MEAS_TEMPERATURE,
  CHECK_AND_SET_TIME,
  BACKLIGHT_OFF,
  BACKLIGHT_ON,
  CLOSE_PROJECT,
  POWER_OFF
};

enum class enGpsBaudRate
{
  BD_9600 = 0,
  BD_152000 = 1,
};

#define PARS_BAT   0  // array index for bat recording parameters
#define PARS_BIRD  1  // array index for bird recording parameters


#endif    //#ifndef _TYPES_H

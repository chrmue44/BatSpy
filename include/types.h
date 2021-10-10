#ifndef _TYPES_H
#define _TYPES_H

#include "config.h"

// operation modes of recorder
enum enCassMode {
  STOP = 1,
  PLAY = 2,
  REC  = 3
};

// selection of available sample rates
enum enSampleRate 
{
  SR_17K =      0,
  SR_19K =      1,
  SR_23K =      2,
  SR_28K =      3,
  SR_32K =      4,
  SR_35K =      5,
  SR_38K =      6,
  SR_44K =      7,
  SR_48K =      8,
  SR_88K =      9,
  SR_96K =     10,
  SR_176K =    11,
  SR_192K =    12,
  SR_234K =    13,
  SR_281K =    14,
  SR_352K =    15,
  SR_384K =    16,
  SR_480K =    17
};

// available pre amp selections
enum enPreAmp
{
  LINEAR         = 0,
  HIGH_PASS      = 1
};

// available hardware gain settings for pre amp
enum enGain
{
  GAIN_27DB            = 0,
  GAIN_36DB            = 1,
  GAIN_45DB            = 2,
  GAIN_54DB            = 3,
};

// recording format on SD card
enum enRecFmt
{
  RAW = 0,
  WAV = 1
};

// display orientation
enum enDispOrient
{
  LEFT_HAND              = 0,
  RIGHT_HAND             = 1
};

// knob rotation
enum enKnobRot
{
  CLOCKWISE              = 0,
  COUNTERCLOCKW          = 1
};

// available operation modes
enum enOpMode {
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
  OFF = 0,
  ON = 1,
  TIME = 2,
  TWILIGHT = 3
};

#endif    //#ifndef _TYPES_H

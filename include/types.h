#ifndef _TYPES_H
#define _TYPES_H

// operation modes of recorder
enum enCassMode {
  CAS_STOP = 1,
  CAS_PLAY = 2,
  CAS_REC  = 3
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
  SR_384K =    16
};

// available pre amp selections
enum enPreAmp
{
  PRE_AMP_LINEAR         = 0,
  PRE_AMP_HIGH_PASS      = 1
};

// available hardware gain settings for pre amp
enum enGain
{
  GAIN_LO                = 0,  // 22/1.5 * 7.8/1.5  = 40
  GAIN_MED               = 1,  // 22/1.5 * 7.8/1.5  = 112
  GAIN_HI                = 2,  // 22/1.5 * 33/1.5   = 322
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
  OPMODE_HEAR_DIRECT     = 0,
  OPMODE_HEAR_HET        = 1,
  OPMODE_PLAY_DIRECT     = 2,
  OPMODE_PLAY_STRETCHED  = 3,
  OPMODE_PLAY_HET        = 4,
  OPMODE_REC_AUTO        = 5,
};


#endif    //#ifndef _TYPES_H

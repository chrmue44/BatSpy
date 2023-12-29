/*************************************************************
 * BatSpy: Teensy 4.x based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2023 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#ifndef _CEEPROM_H_
#define _CEEPROM_H_

#include <Arduino.h>

bool checkCRC();
void writeFloatToEep(int32_t addr, float val);
void writeInt16ToEep(int32_t addr, int16_t val);
float readFloatFromEep(int addr);
int16_t readInt16FromEep(int32_t addr);
unsigned char readCharFromEep(int addr);
void writeCharToEep(int32_t addr, unsigned char val);
void loadLanguage();
void saveParsToEep();
bool loadParsFromEep(); 
bool setAndCheckPassword(const char* pw);
bool isSystemLocked();
bool setSerialNr(const char* serial);
void getSerialNr(char* pBuf, size_t bufSize);
bool setVoltageFactor(const char* p);


#define UNLOCK_PW  "System889376"

#define EEPADDR_MAX_ADDR      0x0000   // highest address for checksum calculation
#define EEPADDR_CHKS_PAR      0x0002   // checksum parameter area
#define EEPADDR_VOLUME        0x0004   // devPar.Volume,         float
#define EEPADDR_MIXFREQ       0x0008   // devPars.mixFreq,       float
#define EEPADDR_RECAUTO       0x000C   // devPars.recAuto,       INT16
#define EEPADDR_SEND_DELAY    0x000E   // devPars.sendDelay,     INT16
#define EEPADDR_SRC_POSITION  0x0010   // devPars.srcPosition,   INT16
#define EEPADDR_MENU_TYPE     0x0012   // devPars.menueType,     INT16
#define EEPADDR_REC_TIME      0x0014   // devPars.recTime,       float
#define EEPADDR_SAMPLE_RATE   0x0018   // devPars.sampleRate     INT16
#define EEPADDR_PRE_AMP_GAIN  0x001A   // devPars.preAmpGain,    INT16
#define EEPADDR_TRESHHOLD     0x001C   // devPars.threshHold,    float
#define EEPADDR_FFT_LEV_MIN   0x0020   // devPars.fftLevelMin,   float
#define EEPADDR_FFT_LEV_MAX   0x0024   // devPars.fftLevelMax,   float
#define EEPADDR_REC_TRESH     0x002C   // devPars.recThreshhold, float
#define EEPADDR_KNOB_ROT      0x0030   // devPars.knobRotation,  INT16
#define EEPADDR_DISP_ORIENT   0x0032   // devPars.dispOrient,    INT16
#define EEPADDR_PRE_TRIGGER   0x0034   // devPars.preTrigger,    float
#define EEPADDR_FREE_INT      0x0038   
#define EEPADDR_DEAD_TIME     0x003A   // devPars.deadTime,      float
#define EEPADDR_BACKLIGHT     0x003E   // devPars.backLightTime  float
#define EEPADDR_LANGUAGE      0x0042   // devPars.lang,          INT16
#define EEPADDR_PRE_AMPTYPE   0x0044   // devPars.preAmpType,    INT16
#define EEPADDR_LAT           0x0046   // devStatus.geoPos.getLat, float
#define EEPADDR_LON           0x004A   // devStatus.geoPos.getLon, float
#define EEPADDR_FILT_FREQ     0x004E   // devPars.filtFreq,        float
#define EEPADDR_FILT_TYPE     0x0052   // devPars.filtType,        INT16
#define EEPADDR_START_H       0x0054   // devPars.startH,          INT16
#define EEPADDR_START_MIN     0x0056   // devPars.startMin,        INT16
#define EEPADDR_STOP_H        0x0058   // devPars.stopH,           INT16
#define EEPADDR_STOP_MIN      0x005A   // devPars.stopMin,         INT16
#define EEPADDR_FREE_INT16_0  0x005C   
#define EEPADDR_FREE_INT16_1  0x005E
#define EEPADDR_FREE_INT16_2  0x0060
#define EEPADDR_LIVE_AMPL     0x0062   // devPars.liveAmplitude,   INT16
#define EEPADDR_FREE_INT16_3  0x0064   
#define EEPADDR_ALTITUDE      0x0066   // devStatus.height,        float
#define EEPADDR_TRIG_TYPE     0x006A   // devPars.triggerType,     INT16
#define EEPADDR_MIN_EV_LEN    0x006C   // devPars.minEventLen,     float
#define EEPADDR_SHUTOFF_V     0x0070   // devPars.ShutoffVoltage,  float
#define EEPADDR_FIRSTFREE     0x0074

#define EEPADDR_SERIAL        0x0300   // serial Nr: 12 char string  
#define EEP_SERIAL_SIZE       12       // size of the string
#define EEPADDR_VOLT_FACT     0x0310   // devPars.voltFactor       float



#endif  //#ifndef _CEEPROM_H_


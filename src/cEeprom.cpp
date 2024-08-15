/*************************************************************
 * BatSpy: Teensy 4.x based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2023 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "cEeprom.h"
#include <EEPROM.h>
#include "debug.h"
#include "config.h"
#include "globals.h"

bool MEMP checkCRC()
{
  int16_t rdCks = readInt16FromEep(EEPADDR_CHKS_PAR);
  int16_t maxAddr = readInt16FromEep(EEPADDR_MAX_ADDR);
  int16_t cks = 0;
  if (maxAddr == 0)
    return false;
  for(int i = 4; i <= maxAddr; i++)
  {
    cks += readCharFromEep(i);
  }
 // Serial.printf("EEPROM checksum; max. Addr: %i; Checksum read:%i  calc: %i\n", maxAddr, rdCks, cks);
  
  bool retVal = (rdCks == cks);
  if(!retVal)
    DPRINTF1("checksum error in EEPROM, expected %i, read %i\n", rdCks, cks);
  return retVal;
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

int16_t MEMP readInt16FromEep(int32_t addr) 
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
  retVal = EEPROM.read(addr);
  return retVal;
}

void MEMP writeCharToEep(int32_t addr, unsigned char val) 
{
  EEPROM.write(addr, val);
}


void MEMP saveParsToEep() 
{
  writeFloatToEep(EEPADDR_VOLUME,       devPars.volume.get());
  writeFloatToEep(EEPADDR_MIXFREQ,      devPars.mixFreq.get());
  writeInt16ToEep(EEPADDR_RECAUTO,      devPars.recAuto.get());
  writeInt16ToEep(EEPADDR_SEND_DELAY,   (int16_t)devPars.sendDelay.get());
  writeInt16ToEep(EEPADDR_SRC_POSITION, devPars.srcPosition.get());
  writeInt16ToEep(EEPADDR_MENU_TYPE,    devPars.menueType.get());
  writeFloatToEep(EEPADDR_REC_TIME,     devPars.recTime.get());
  writeInt16ToEep(EEPADDR_SAMPLE_RATE,  devPars.sampleRate.get());
  writeInt16ToEep(EEPADDR_PRE_AMP_GAIN, devPars.preAmpGain.get());
  writeFloatToEep(EEPADDR_TRESHHOLD,    devPars.threshHold.get());
  writeFloatToEep(EEPADDR_FFT_LEV_MIN,  devPars.fftLevelMin.get());
  writeFloatToEep(EEPADDR_FFT_LEV_MAX,  devPars.fftLevelMax.get());
  writeFloatToEep(EEPADDR_REC_TRESH,    devPars.recThreshhold.get());
  writeInt16ToEep(EEPADDR_KNOB_ROT,     devPars.knobRotation.get());
  writeInt16ToEep(EEPADDR_DISP_ORIENT,  devPars.dispOrient.get());
  writeFloatToEep(EEPADDR_PRE_TRIGGER,  devPars.preTrigger.get());
  writeInt16ToEep(EEPADDR_DISP_MODE,    devPars.displayMode.get());
  writeFloatToEep(EEPADDR_DEAD_TIME,    devPars.deadTime.get());
  writeFloatToEep(EEPADDR_BACKLIGHT,    devPars.backLightTime.get());
  writeInt16ToEep(EEPADDR_LANGUAGE,     devPars.lang.get());
  writeFloatToEep(EEPADDR_DEBUG_LEVEL , devPars.debugLevel.get());
  writeFloatToEep(EEPADDR_LAT,          devStatus.geoPos.getLat());
  writeFloatToEep(EEPADDR_LON,          devStatus.geoPos.getLon());
  writeFloatToEep(EEPADDR_TFILT_FREQ,    devPars.trigFiltFreq.get());
  writeInt16ToEep(EEPADDR_TFILT_TYPE,    devPars.trigFiltType.get());
  writeInt16ToEep(EEPADDR_START_H,      (int16_t)devPars.startH.get());
  writeInt16ToEep(EEPADDR_START_MIN,    (int16_t)devPars.startMin.get());
  writeInt16ToEep(EEPADDR_STOP_H,       (int16_t)devPars.stopH.get());
  writeInt16ToEep(EEPADDR_STOP_MIN,     (int16_t)devPars.stopMin.get());
  writeInt16ToEep(EEPADDR_TIMEZONE,     (int16_t)devPars.timeZone.get());
  writeInt16ToEep(EEPADDR_DAYLIGHT_SAV, (int16_t)devPars.daylightSav.get());
  writeInt16ToEep(EEPADDR_LIVE_AMPL,    (int16_t)devPars.liveAmplitude.get());
  writeFloatToEep(EEPADDR_ALTITUDE,     devStatus.height.get());
  writeInt16ToEep(EEPADDR_TRIG_TYPE,    (int16_t)devPars.triggerType.get());
  writeFloatToEep(EEPADDR_MIN_EV_LEN,   devPars.minEventLen.get());
  writeFloatToEep(EEPADDR_SHUTOFF_V,    devPars.ShutoffVoltage.get());
  writeFloatToEep(EEPADDR_RFILT_FREQ,   devPars.recFiltFreq.get());
  writeInt16ToEep(EEPADDR_RFILT_TYPE,   devPars.recFiltType.get());
  writeInt16ToEep(EEPADDR_BAUD_GPS,     devPars.gpsBaudRate.get());

  int addr = EEPADDR_FIRSTFREE;
  for(int i = 0; i < 20; i++)
  {
    writeInt16ToEep(addr, 0);
    addr += 2;
  }
  writeInt16ToEep(EEPADDR_MAX_ADDR, addr - 2);

  int16_t chks = 0;
  for(int i = 4; i <= addr - 2; i++)
    chks += readCharFromEep(i);
  writeInt16ToEep(EEPADDR_CHKS_PAR, chks);
 // Serial.printf("EEPROM written; max. Addr: %i; Checksum %i\n", addr - 2, chks);
}

bool MEMP loadParsFromEep()
{
  bool retVal = false;
  if (checkCRC())
  {
    devPars.volume.set(readFloatFromEep(EEPADDR_VOLUME));
    devPars.mixFreq.set(readFloatFromEep(EEPADDR_MIXFREQ));
    devPars.recAuto.set(readInt16FromEep(EEPADDR_RECAUTO));
    devPars.sendDelay.set(readInt16FromEep(EEPADDR_SEND_DELAY));
    devPars.srcPosition.set(readInt16FromEep(EEPADDR_SRC_POSITION));
    if (hasDisplay() == enDisplayType::OLED_128)
    {
      devPars.menueType.set(enMenueType::COMPACT);
      devPars.knobRotation.set(enKnobRot::CLOCKWISE);
    }
    else
    {
      devPars.menueType.set(readInt16FromEep(EEPADDR_MENU_TYPE));
      devPars.knobRotation.set(readInt16FromEep(EEPADDR_KNOB_ROT));
    }
    devPars.recTime.set(readFloatFromEep(EEPADDR_REC_TIME));
    devPars.sampleRate.set(readInt16FromEep(EEPADDR_SAMPLE_RATE));
    devPars.preAmpGain.set(readInt16FromEep(EEPADDR_PRE_AMP_GAIN));
    devPars.threshHold.set(readFloatFromEep(EEPADDR_TRESHHOLD));
    devPars.fftLevelMin.set(readFloatFromEep(EEPADDR_FFT_LEV_MIN));
    devPars.fftLevelMax.set(readFloatFromEep(EEPADDR_FFT_LEV_MAX));
    devPars.recThreshhold.set(readFloatFromEep(EEPADDR_REC_TRESH));
    devPars.dispOrient.set(readInt16FromEep(EEPADDR_DISP_ORIENT));
    devPars.preTrigger.set(readFloatFromEep(EEPADDR_PRE_TRIGGER));
    devPars.displayMode.set(readInt16FromEep(EEPADDR_DISP_MODE));
    //    devPars.recFmt.set(readInt16FromEep(EEPADDR_REC_FMT));
    devPars.deadTime.set(readFloatFromEep(EEPADDR_DEAD_TIME));
    devPars.backLightTime.set(readFloatFromEep(EEPADDR_BACKLIGHT));
    devPars.lang.set(readInt16FromEep(EEPADDR_LANGUAGE));
    devPars.debugLevel.set(readFloatFromEep(EEPADDR_DEBUG_LEVEL));
    devStatus.geoPos.setLat(readFloatFromEep(EEPADDR_LAT));
    devStatus.geoPos.setLon(readFloatFromEep(EEPADDR_LON));
    devPars.trigFiltFreq.set(readFloatFromEep(EEPADDR_TFILT_FREQ));
    devPars.trigFiltType.set(readInt16FromEep(EEPADDR_TFILT_TYPE));
    devPars.startH.set(readInt16FromEep(EEPADDR_START_H));   //if addr changes see also pnlparams.cpp
    devPars.startMin.set(readInt16FromEep(EEPADDR_START_MIN)); //if addr changes see also pnlparams.cpp
    devPars.stopH.set(readInt16FromEep(EEPADDR_STOP_H));    //if addr changes see also pnlparams.cpp
    devPars.stopMin.set(readInt16FromEep(EEPADDR_STOP_MIN));  //if addr changes see also pnlparams.cpp
    devPars.timeZone.set(readInt16FromEep(EEPADDR_TIMEZONE));
    devPars.daylightSav.set(readInt16FromEep(EEPADDR_DAYLIGHT_SAV));

    devPars.liveAmplitude.set(readInt16FromEep(EEPADDR_LIVE_AMPL));
    //  devPars.projectType.set(readInt16FromEep(EEPADDR_PRJ_TYPE));
    devStatus.height.set(readFloatFromEep(EEPADDR_ALTITUDE));
    devPars.recFiltFreq.set(readFloatFromEep(EEPADDR_RFILT_FREQ));
    devPars.recFiltType.set(readInt16FromEep(EEPADDR_RFILT_TYPE));
    devPars.triggerType.set(readInt16FromEep(EEPADDR_TRIG_TYPE));
    devPars.minEventLen.set(readFloatFromEep(EEPADDR_MIN_EV_LEN));
    //devPars.ShutoffVoltage.set(readFloatFromEep(EEPADDR_SHUTOFF_V));
    devPars.ShutoffVoltage.set(5.8);
    devPars.gpsBaudRate.set(readInt16FromEep(EEPADDR_BAUD_GPS));
    devPars.voltFactor.set(readFloatFromEep(EEPADDR_VOLT_FACT));
    int digits = analogRead(PIN_SUPPLY_VOLT);
    devStatus.setVoltage.set((devPars.voltFactor.get() * (float)digits));
    retVal = true;
  }
  else
    saveParsToEep();
  return retVal;
}


void MEMP loadLanguage()
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

char passwd[16];

bool isSystemLocked()
{
  return (strlen(passwd) != strlen(UNLOCK_PW)) || strncmp(passwd, UNLOCK_PW, sizeof(UNLOCK_PW) != 0);
}

bool setVoltageFactor(const char* p)
{
  bool retVal = false;
  if(!isSystemLocked())
  {
    float voltage = atof(p);
    float fact = calcVoltageFactor(voltage);
    devPars.voltFactor.set(fact);
    writeFloatToEep(EEPADDR_VOLT_FACT, fact);
    retVal = true;
  }
  return retVal;
}

bool setAndCheckPassword(const char* pw)
{
  strncpy(passwd, pw, sizeof(passwd) - 1);
  return isSystemLocked();
}


/// @brief set a serial number
/// @param serial string containing serial format should be 11 character as follows:
///               BS40-C-0001
/// @param pwd 
bool MEMP setSerialNr(const char* serial)
{
  if(!isSystemLocked())
  {
    const char* p = serial;
    int addr = EEPADDR_SERIAL;
    for (int i = 0; i < EEP_SERIAL_SIZE; i++)
    {
      writeCharToEep(addr++, p[i]);
    }
    return true;
  }
  else 
   return false;
}

void getSerialNr(char* pBuf, size_t bufSize)
{
  int addr = 0x300;
  if(bufSize >= EEP_SERIAL_SIZE)
  {
    for (int i = 0; i < EEP_SERIAL_SIZE; i++)
      pBuf[i] = readCharFromEep(addr++);
    pBuf[EEP_SERIAL_SIZE - 1] = 0;
  } 
}

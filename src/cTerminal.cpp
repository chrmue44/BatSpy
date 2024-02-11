/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "cTerminal.h"
#include "globals.h"
#include <Arduino.h>
#include "cSdCard.h"
#include "cmenue.h"
#include "cutils.h"
#include "cEeprom.h"

extern cMenue menue;

cTerminal::cTerminal() 
{
}

bool MEMF cTerminal::parseCmdfromUSB() 
{
  bool retVal = false;
  int numBytes = Serial.available();
  if(numBytes <= 0)
    return false;
  m_onlineTime = millis();
  while(numBytes > 0) 
  {
    if(m_recIdxUSB < sizeof(m_recbufUSB)/sizeof(m_recbufUSB[0]))
      m_recbufUSB[m_recIdxUSB] = Serial.read();
    else
    {
      m_recIdxUSB = sizeof(m_recbufUSB) / sizeof(m_recbufUSB[0] - 1);
      m_recbufUSB[m_recIdxUSB] = '\n';
    }

    numBytes--;
    if(m_recbufUSB[m_recIdxUSB] == '\n')
      retVal = execCmd(m_recbufUSB, m_recIdxUSB);
    else
      m_recIdxUSB++;
  }
  return retVal;
}


bool MEMF cTerminal::execCmd(char* buf, size_t& bufIdx)
{
  bool retVal = false;
  enSdRes rc;
  buf[bufIdx] = 0;
  if(buf[bufIdx - 1] == '\r')  // for commands from Windows machines
    buf[bufIdx - 1] = 0;
  tDirInfo dirInfo;
  size_t dirIdx;
  char ret;

  switch(buf[0]) 
  {
    case 'h':
      Serial.println("");
      showCommands();
      break;

    case 'A':
      saveParsToEep();
      Serial.write('0');
      Serial.write(m_endChar);
      break;

    case 'c': 
      rc = cSdCard::inst().chdir(&buf[1]);
      if(rc != 0)
        Serial.printf("1\nerror changing directory: %i\n", rc);
      Serial.printf("0\n%s\n", cSdCard::inst().getActDir());
      Serial.write(m_endChar);
      break;

    case 'C':
      parseControlCmd(&buf[1]);
      break;
      
    case 'd':
      {
        Serial.println("");
        dirIdx = 0;
        char* p = NULL;
        if((buf[1] == '*') && (buf[2] == '.')) {
          p = &buf[3];
          buf[6] = 0;
        }
        do {
          rc = cSdCard::inst().dir(dirInfo, true, p, dirIdx);
          dirIdx += MAX_DIRENTRIES;
        } while(dirInfo.size() >= MAX_DIRENTRIES);
      
        Serial.write(m_endChar);
      }
      break;
      
    case 'D':
      if(buf[1] == '0') {
        devStatus.forceDisplay = 0;
        Serial.write('0');
      }
      else if(buf[1] == '1') {
        devStatus.forceDisplay = 1;
        Serial.write('0');
      }
      else
        Serial.write('1');      
      break;

    case 'e':
      parseDebugCmd(&buf[1]);
      Serial.write(m_endChar);
      break;

    case 'f': 
      if(buf[1] == '0')
         audio.sendFftBuffer(devPars.sendDelay.get(), 0);
      if(buf[1] == '1')
         audio.sendFftBuffer(devPars.sendDelay.get(), 1);
      Serial.write(m_endChar);
      break;

    case 'L':
      {
        bool ok = loadParsFromEep();
        if(ok)
          Serial.write('0');
        else
          Serial.write('1');   
        Serial.write(m_endChar);
      }
      break;

    case 'm':
      ret = cSdCard::inst().mkDir(&buf[1]);
      if(ret == 0)
        Serial.write('0');
      else
        Serial.write('1');
      Serial.write(m_endChar);      
      break;
    
    case 'g':
      gps.test();
      Serial.write("GPS test mode finished\n");
      break;

    case 'n':
      {
        char oldName[FILENAME_LEN];
        char newName[FILENAME_LEN];
        char* p = strchr(&buf[1], ' ');
        *p = 0;
        if(p != NULL) {
          strcpy(oldName, &buf[1]);    
          strcpy(newName, p + 1);
//          ret = cSdCard::inst().rename(oldName, newName);
          ret = 0;  //TODO
          if(ret == 0)
            Serial.write('0');
          else
            Serial.write('1');
          Serial.write(m_endChar);      
        }
      }
      break;

    case 'O':
      m_key = enKey::KEY_OK;
      Serial.println("enKey::KEY_OK");
      break;
      
    case 'p':
      if (buf[1] == 0)
        menue.printPars();
      else
        parseGetCmd(&buf[1]);
      Serial.write(m_endChar);
      break;

    case 'P':
      parseSetCmd(&buf[1]);
      retVal = true;
      break;
    
    case 'r':
      cSdCard::inst().sendFileToTerminal(&buf[1], devPars.sendDelay.get());
      break;
    

    case 's':
      parseGetStatusCmd(&buf[1]);
      break;

    case 'S':
      parseSetStatusCmd(&buf[1]);
      break;
      
    case 'u':
      m_key = enKey::UP;
      Serial.println("enKey::UP");
      break;

    case 'W':
      m_key = enKey::DOWN;
      Serial.println("enKey::DOWN");
      break;
      
    case 'w':
      cSdCard::inst().readFileFromTerminal(&buf[1]);      
      break;
    case 'v':
      Serial.write(devStatus.version.get());      
      Serial.write(m_endChar);
      break;

    case 'x':
    case 'X':
      ret = (char)cSdCard::inst().del(&buf[1]);
      ret += '0';
      if (ret > '9')
        ret += 7;
      Serial.write(ret);      
      Serial.write(m_endChar);
      break;  
    case 'Y':
      Serial.write(setAndCheckPassword(&buf[1]) ? '1':'0');
      Serial.write(m_endChar);
      break;
    case 'y':
      Serial.write(isSystemLocked() ? '1' : '0');
      Serial.write(m_endChar);
      break;
    case 'Z':
      Serial.write(setSerialNr(&buf[1]) ? '0':'1');
      Serial.write(m_endChar);
      break;
    case 'z':
      {
        char serial[EEP_SERIAL_SIZE];
        getSerialNr(serial, EEP_SERIAL_SIZE);
        Serial.print(serial);
        Serial.write(m_endChar);
      } 
      break;
    default:
      Serial.printf("unknown command: %c\n",buf[0]);
      break;
  }
  bufIdx = 0;
  return retVal;
}

bool cTerminal::isOnline()
{
  uint32_t now = millis();
  uint32_t dt;
  if(now < m_onlineTime)
    dt = 0xFFFFFFFF - m_onlineTime + now;
  else
    dt = now - m_onlineTime;
  return (dt < MAX_ONLINE_PAUSE);
}

void MEMF cTerminal::parseSetStatusCmd(const char* buf)
{
  char locBuf[64];
  strncpy(locBuf, buf, sizeof(locBuf) - 1);
  my_vector<char*, 20> token;
  int y, mo, d, h, mi, sec;
  int cnt;

  switch(buf[0])
  {
    case 'd':
      cnt = cUtils::splitStr(&locBuf[1], '.', token);
      if(cnt == 3)
      {
        rtc.getTime(y, mo, d, h, mi, sec);
        d = atoi(token[0]);
        mo = atoi(token[1]);
        y = atoi(token[2]);
        rtc.setTime(y, mo, d, h, mi, sec);
        Serial.write('0');
      }
      else
        Serial.write('?');
      break;

    case 't':
      cnt = cUtils::splitStr(&locBuf[1], ':', token);
      if(cnt == 3)
      {
        rtc.getTime(y, mo, d, h, mi, sec);
        h = atoi(token[0]);
        mi = atoi(token[1]);
        sec = atoi(token[2]);
        rtc.setTime(y, mo, d, h, mi, sec);
        Serial.write('0');
      }
      else
        Serial.write('?');
      break;
  }
  Serial.write(m_endChar);
}

void MEMF cTerminal::parseGetStatusCmd(const char* buf)
{
  char replyBuf[256];
  switch(buf[0])
  {
    case 'a':
      devStatus.audioMem.set(AudioMemoryUsage());
      getValFloat(buf + 1, devStatus.audioMem, replyBuf, sizeof(replyBuf));
      Serial.print(&replyBuf[0]);
      break;
    case 'b':
      {
        bool ok = sht.readSample();
        if(ok)
          Serial.printf("%.1f", sht.getTemperature());
        else
          Serial.println("Error reading temperature");
      }
      break;
    case 'c':
      Serial.printf("%.1f",  sht.getHumidity());
      break;
	  case 'd':
  	  snprintf(replyBuf, sizeof(replyBuf), "%02lu.%02lu.%04lu", devStatus.date.getDay(), devStatus.date.getMonth(), devStatus.date.getYear());
      Serial.print(&replyBuf[0]);
      break;
	  case 'e':
      getValEnum(buf + 1, devStatus.playStatus, replyBuf, sizeof(replyBuf));
      Serial.print(&replyBuf[0]);
      break;	
	case 'f':
      size_t freeSpace;  size_t totSpace;
      cSdCard::inst().getFreeMem(freeSpace, totSpace);
      devStatus.freeSpace.set(freeSpace / 1024);
      getValFloat(buf + 1, devStatus.freeSpace, replyBuf, sizeof(replyBuf));
      Serial.print(&replyBuf[0]);
      break;	
    case 'g':
      devStatus.cpuAudioAvg.set(AudioProcessorUsage());
      getValFloat(buf + 1, devStatus.cpuAudioAvg, replyBuf, sizeof(replyBuf));
      Serial.print(&replyBuf[0]);
      break; 
    case 'h':
      getValFloat(buf + 1, devStatus.height, replyBuf, sizeof(replyBuf));
      Serial.print(&replyBuf[0]);
      break; 
	case 'i':
      getValFloat(buf + 1, devStatus.chargeLevel, replyBuf, sizeof(replyBuf));
      Serial.print(&replyBuf[0]);
      break; 	
    case 'k':
      getValFloat(buf + 1, devStatus.peakVal, replyBuf, sizeof(replyBuf));
      Serial.print(&replyBuf[0]);
      break;
    case 'l':
	    getPosition(replyBuf, sizeof(replyBuf));
      Serial.print(&replyBuf[0]);
	    break;
    case 'm':
      getValFloat(buf + 1, devStatus.mainLoop, replyBuf, sizeof(replyBuf));
      Serial.print(&replyBuf[0]);
      break; 

    case 'p':
      devStatus.gpsStatus.set(gps.getStatus());
      getValEnum(buf + 1, devStatus.gpsStatus, replyBuf, sizeof(replyBuf));
      Serial.print(&replyBuf[0]);
      break;

    case 'r':
      getValFloat(buf + 1, devStatus.recCount, replyBuf, sizeof(replyBuf));
      Serial.print(&replyBuf[0]);
      break; 
	case 's':
      getValFloat(buf + 1, devStatus.satCount, replyBuf, sizeof(replyBuf));
      Serial.print(&replyBuf[0]);
      break;
	case 't':
	  snprintf(replyBuf, sizeof(replyBuf), "%02lu:%02lu:%02lu", devStatus.time.getHour(), devStatus.time.getMin(), devStatus.time.getSec());
      Serial.print(&replyBuf[0]);
      break;
    case 'u':
      menue.printStatus();
      break;
    case 'v':
      getValFloat(buf + 1, devStatus.voltage, replyBuf, sizeof(replyBuf));
      Serial.print(&replyBuf[0]);
      break;
    case 'x':
      devStatus.cpuAudioMax.set(AudioProcessorUsageMax());
      getValFloat(buf + 1, devStatus.cpuAudioMax, replyBuf, sizeof(replyBuf));
      Serial.print(&replyBuf[0]);
      break; 
    default:
      Serial.write('?');
  }
  Serial.write(m_endChar);
}

void MEMF cTerminal::getPosition(char* buf, size_t bufSize)
{
  int degLat = devStatus.geoPos.getDegLat();
  char latStr[2];
  char lonStr[2];
  if(degLat < 0)
	latStr[0] = 'S';
  else
    latStr[0] = 'N';
  latStr[1] = 0;
  int degLon = devStatus.geoPos.getDegLon();
  if(degLon < 0)
	lonStr[0] = 'W';
  else
	lonStr[0] = 'E';
  lonStr[1] = 0;
  char locBuf[128];
  snprintf(locBuf, sizeof(locBuf), "%02i" CH_DEG " %02.3f %s  %03i" CH_DEG " %02.3f %s",  
	       degLat, devStatus.geoPos.getMinfLat(), latStr,
	       degLon, devStatus.geoPos.getMinfLon(), lonStr);
  cUtils::replaceInternalCodingWithUTF8(locBuf, buf, bufSize);

}


void MEMF cTerminal::parseControlCmd(const char* buf) 
{
  m_key = enKey::TER;
  bool replyOk = true;
  switch(buf[0]) {
    case 'p':
      devStatus.playStatus.set(enPlayStatus::ST_PLAY);
      break;
    case 'r':
      devStatus.playStatus.set(enPlayStatus::ST_REC);
      break;
    case 's':
      devStatus.playStatus.set(enPlayStatus::ST_STOP);
      break;
    case 'f':
      devPars.fileName.set(&buf[1]);
      break;
    case 'm':
      devStatus.opMode.set(buf[1] - '0');
      Serial.println(devStatus.opMode.getActText());
      break;
    default:
      m_key = enKey::NOKEY;
      replyOk = false;
      break;
  }
  if(replyOk)
    Serial.write('0');
  else  
    Serial.write('1');
  Serial.write(m_endChar);
}


void MEMF cTerminal::parseSetCmd(const char* buf) 
{
  m_key = enKey::TER;
  bool replyOk = true;
  int val = 0;
  switch(buf[0]) 
  {
    case 'a':
      replyOk = parseAutoRecParams(&buf[1], true);
      break;
    case 'b':
     replyOk = setValFloat(buf + 1, PAR_BACKLIGHT_MIN, PAR_BACKLIGHT_MAX, devPars.backLightTime);
     break;
    case 'f':
      replyOk = setVoltageFactor(&buf[1]);
      break;
    case 'l':
      replyOk = parseLocationParams(&buf[1], true);
      break;
    case 'm':
     replyOk = setValFloat(buf + 1, devPars.mixFreq.getMin(), devPars.mixFreq.getMax(), devPars.mixFreq);
     break;
    case 'n':
      setValEnum(&buf[1], 0, 1, devPars.lang);
      break;
    case 'p':
      setValEnum(&buf[1], 0, devPars.srcPosition.size() - 1, devPars.srcPosition);
      gps.setMode((enGpsMode)devPars.srcPosition.get());
      break;
    case 'r':
      replyOk = parseRecParams(&buf[1], true);
      break;
    case 'v':
      val = atoi(buf + 1);
      if ((val > -30) && (val <= 18))
        devPars.volume.set(val);
      else
        replyOk = false;
      break;
    default:
      m_key = enKey::NOKEY;
      replyOk = false;
      break;
  }
  if(replyOk)
    Serial.write('0');
  else  
    Serial.write('1');
  Serial.write(m_endChar);
}

void MEMF cTerminal::parseGetCmd(const char* buf)
{
  m_key = enKey::TER;
  bool replyOk = true;
  int val = 0;
  char replyBuf[256];
  switch (buf[0])
  {
  case 'a':
    replyOk = parseAutoRecParams(&buf[1], false, replyBuf, sizeof(replyBuf));
    break;
  case 'b':
    getValInt(buf + 1, devPars.backLightTime, replyBuf, sizeof(replyBuf));
    break;
  case 'l':
    replyOk = parseLocationParams(&buf[1], false, replyBuf, sizeof(replyBuf));
    break;
  case 'm':
    getValInt(buf + 1, devPars.mixFreq, replyBuf, sizeof(replyBuf));
    break;
  case 'n':
    getValEnum(&buf[1], devPars.lang, replyBuf, sizeof(replyBuf));
    break;
  case 'p':
    getValEnum(&buf[1], devPars.srcPosition, replyBuf, sizeof(replyBuf));
    break;
  case 'r':
    replyOk = parseRecParams(&buf[1], false, replyBuf, sizeof(replyBuf));
    break;
  case 'v':
    val = atoi(buf + 1);
    if ((val > -30) && (val <= 18))
      devPars.volume.set(val);
    else
      replyOk = false;
    break;
  default:
    m_key = enKey::NOKEY;
    replyOk = false;
    break;
  }
  if (replyOk)
    Serial.print(&replyBuf[0]);
  else
    Serial.write('?');
}

bool MEMF cTerminal::parseAutoRecParams(const char* buf, bool write, char* reply, size_t replySize)
{
  bool replyOk = false;
  if (replySize >= 2)
  {
    reply[0] = '?';
    reply[1] = 0;
  }
  switch (buf[0])
  {
    case 'o':
      if (write)
      {
        if(buf[1] == '0')
          audio.stopRecording();
        replyOk = setValEnum(buf + 1, PAR_AUTOMODE_MIN, PAR_AUTOMODE_MAX, devPars.recAuto);
      }
      else
        getValEnum(buf + 1, devPars.recAuto, reply, replySize);
      break;
    case 'h':
      if (write)
        replyOk = setValInt(buf + 1, 0, 23, devPars.startH);
      else
        getValInt(buf + 1, devPars.startH, reply, replySize);
      break;
    case 'i':
      if (write)
        replyOk = setValInt(buf + 1, 0, 23, devPars.stopH);
      else
        getValInt(buf + 1, devPars.stopH, reply, replySize);
      break;
    case 'm':
      if (write)
        replyOk = setValInt(buf + 1, 0, 59, devPars.startMin);
      else
        getValInt(buf + 1, devPars.startMin, reply, replySize);
      break;
    case 'n':
      if (write)
        setValInt(buf + 1, 0, 59, devPars.stopMin);
      else
        getValInt(buf + 1, devPars.stopMin, reply, replySize);
      break;
  }
  if (!write && (reply[0] != '?') && (replySize >= 2))
    replyOk = true;
  return replyOk;
}


bool MEMF cTerminal::parseLocationParams(const char* buf, bool write, char* reply, size_t replySize)
{
  bool replyOk = false;
  float valf = 0.0;
  if (replySize >= 2)
  {
    reply[0] = '?';
    reply[1] = 0;
  }
  switch (buf[0])
  {
    case 's':
      if(write)
        replyOk = setValEnum(buf + 1, PAR_LOCSRC_MIN, PAR_LOCSRC_MAX, devPars.srcPosition);
      else
        getValEnum(buf + 1, devPars.srcPosition, reply, replySize);
      break;
    case 'a':
      if (write)
      {
        valf = atof(buf + 1);
        replyOk = checkValF(valf, 0, 90);
        if (replyOk)
          devStatus.geoPos.setLat(valf);
      }
      else
      {
        if(buf[1] == 'r')
          devStatus.geoPos.getRangeLat(reply, replySize);
        else
          snprintf(reply, replySize, "%f", devStatus.geoPos.getLat());
      }
      break;
    case 'o':
      if (write)
      {
        valf = atof(buf + 1);
        replyOk = checkValF(valf, -180, 180);
        if (replyOk)
          devStatus.geoPos.setLon(valf);
      }
      else
      {
        if(buf[1] == 'r')
          devStatus.geoPos.getRangeLon(reply, replySize);
        else
          snprintf(reply, replySize, "%f", devStatus.geoPos.getLon());
      }
      break;
    case 'h':
      if (write)
        replyOk = setValFloat(buf + 1, PAR_LOCSRC_MIN, PAR_LOCSRC_MAX, devStatus.height);
      else
        getValFloat(buf + 1,devStatus.height, reply, replySize);
      break;
  }
  if (!write && (reply[0] != '?') && (replySize >= 2))
    replyOk = true;
  return replyOk;
}


bool MEMF cTerminal::parseRecParams(const char* buf, bool write, char* reply, size_t replySize)
{
  bool replyOk = true;
  if (replySize >= 2)
  {
    reply[0] = '?';
    reply[1] = 0;
  }
  switch (buf[0])
  {	
    case 'd':
      if(write)
        replyOk = setValFloat(buf + 1, PAR_DEADTIM_MIN, PAR_DEADTIM_MAX, devPars.deadTime);
      else
        getValFloat(buf + 1, devPars.deadTime, reply, replySize);
      break;
	case 'g':
      if(write)
        replyOk = setValEnum(buf + 1, PAR_GAIN_MIN, PAR_GAIN_MAX, devPars.preAmpGain);
      else
        getValEnum(buf + 1, devPars.preAmpGain, reply, replySize);
      break;

    case 's':
      if (write)
        replyOk = setValEnum(buf + 1, PAR_SR_MIN, PAR_SR_MAX, devPars.sampleRate);
      else
        getValEnum(buf + 1, devPars.sampleRate, reply, replySize);
      break;
    case 't':
      if (write)
        replyOk = setValFloat(buf + 1, PAR_RECTIM_MIN, PAR_RECTIM_MAX, devPars.recTime);
      else
        getValFloat(buf + 1, devPars.recTime, reply, replySize);
      break;
    case 'h':
      if (write)
        replyOk = setValFloat(buf + 1, PAR_RECTHRESH_MIN, PAR_RECTHRESH_MAX, devPars.recThreshhold);
      else
        getValFloat(buf + 1, devPars.recThreshhold, reply, replySize);
      break;
    case 'r':
      if (write)
        replyOk = setValEnum(buf + 1, PAR_TRIGTYPE_MIN, PAR_TRIGTYPE_MAX, devPars.triggerType);
      else
        getValEnum(buf + 1, devPars.triggerType, reply, replySize);
      break;
    case 'f':
      if (write)
        replyOk = setValFloat(buf + 1, PAR_TRIGFILTFREQ_MIN, PAR_TRIGFILTFREQ_MAX, devPars.filtFreq);
      else
        getValFloat(buf + 1, devPars.filtFreq, reply, replySize);
      break;
    case 'm':
      if (write)
        replyOk = setValFloat(buf + 1, PAR_TRIGEVENT_MIN, PAR_TRIGEVENT_MAX, devPars.minEventLen);
      else
        getValFloat(buf + 1, devPars.minEventLen, reply, replySize);
      break;
    case 'y':
      if (write)
        replyOk = setValEnum(buf + 1, 0, 1, devPars.filtType);
      else
        getValEnum(buf + 1, devPars.filtType, reply, replySize);
      break;
    case 'p':
      if (write)
        replyOk = setValFloat(buf + 1, PAR_PRETRIG_MIN, PAR_PRETRIG_MAX, devPars.preTrigger);
      else
        getValFloat(buf + 1, devPars.preTrigger, reply, replySize);
      break;
  }
  if (!write && (reply[0] != '?') && (replySize >= 2))
    replyOk = true;
  return replyOk;
}

bool MEMF cTerminal::setValInt(const char* buf, int min, int max, cParNum& par)
{
  int val = atoi(buf);
  bool retVal = false;
  if ((min <= val) && (val <= max))
  {
    retVal = true;
    par.set(val);
  }
  return retVal;
}

void MEMF cTerminal::getValInt(const char* buf, cParNum& par, char* reply, size_t replySize)
{
  if(buf[0] == 'r')
    par.getRange(reply, replySize);
  else
    snprintf(reply, replySize, "%i", (int)par.get());
}

bool MEMF cTerminal::setValFloat(const char* buf, float min, float max, cParNum& par)
{
  int val = atof(buf);
  bool retVal = false;
  if ((min <= val) && (val <= max))
  {
    retVal = true;
    par.set(val);
  }
  return retVal;
}

void MEMF cTerminal::getValFloat(const char* buf, cParNum& par, char* reply, size_t replySize)
{
  if(buf[0] == 'r')
    par.getRange(reply, replySize);
  else
    snprintf(reply, replySize, "%f", par.get());
}

bool MEMF cTerminal::setValEnum(const char* buf, int min, int max, cParEnum& par)
{
  int val = atoi(buf);
  bool retVal = false;
  if ((min <= val) && (val <= max))
  {
    retVal = true;
    par.set(val);
  }
  return retVal;
}

void MEMF cTerminal::getValEnum(const char* buf, cParEnum& par, char* reply, size_t replySize)
{
  if(buf[0] == 'r')
  {
    char buf[512];
    par.getRange(buf, sizeof(buf));
    cUtils::replaceInternalCodingWithUTF8(buf, reply, replySize);
  }
  else
    snprintf(reply, replySize, "%i", (int)par.get());
}

void MEMF cTerminal::parseDebugCmd(const char* buf)
{
  int ioNr;
  int inp;
  switch(buf[0])
  {
    case '0':
      setIoDebugMode(false);
      Serial.println("IO debug OFF");
      break;

    case '1':
      setIoDebugMode(true);
      Serial.println("IO debug ON");
      break;

    case 'a':
      ioNr = atoi(&buf[1]);
      inp = analogRead(ioNr);
      Serial.printf("analog input %i: %i\n", ioNr, inp);
      break;
    
    case 'i':
      ioNr = atoi(&buf[1]);
      inp = digitalRead(ioNr);
      Serial.printf("digital input %i: %i\n", ioNr, inp);
      break;

    case 'l':
      ioNr = atoi(&buf[1]);
      sysLog.setLogLevel(ioNr);
      break;

    case 'o':
      ioNr = atoi(&buf[1]);
      Serial.printf("digital output %i OFF\n", ioNr);
      if(ioNr < 99)
        digitalWrite(ioNr,0);
      else
      {
        ioNr -= 100;
        int mask = 1 << ioNr;
        portExpSetBit(mask, 0);
      }
      break;
    case 'O':
      ioNr = atoi(&buf[1]);
      Serial.printf("digital output %i ON\n", ioNr);
      if(ioNr < 99)
        digitalWrite(ioNr,1);
      else
      {
        ioNr -= 100;
        int mask = 1 << ioNr;
        portExpSetBit(mask, 1);
      }
      break;
    default:
      Serial.print("1");
  }
} 

void MEMF cTerminal::showCommands() 
{
  Serial.println("Available commands:");
  Serial.println("A        save parameters to EEPROM");
  Serial.println("c<name>  change directory");
  Serial.println("Cf<name> set play name");  
  Serial.println("Cm<mode> set display mode(0 = HEAR_DIRECT, 1 = HEAR_HET, 2 = PLAY_DIRECT,");
  Serial.println("                          3 = PLAY_STRETCHED, 4 = PLAY_HET");
  Serial.println("Cr       record");
  Serial.println("Cp       play");
  Serial.println("Cs       stop");
  Serial.println("d        show directory");
  Serial.println("D0       Debug: force display activity off");
  Serial.println("D1       Debug: force display activity on");
  Serial.println("e0       Set debug IO off");
  Serial.println("e1       Set Debug IO on");
  Serial.println("ea<nr>   read analog value from pin (nr 0..30)");
  Serial.println("ei<nr>   read digital value from pin (nr 0..30)");
  Serial.println("eo<nr>   set digital output OFF (nr 0..30 direct IO, 100 .. 107 port extender)");
  Serial.println("eO<nr>   set digital output ON (nr 0..30 direct IO, 100 .. 107 port extender)");
  Serial.println("f        get live fft");
  Serial.println("g        GPS test cmd: Serial connected to GPS, terminate with 'q!'");
  Serial.println("L        load parameters from EEPROM");
  Serial.println("n<old> <new> rename file or directory");  
  Serial.println("o        key OK");
  Serial.println("p        print parameters");
  Serial.println("Pao<val> set auto recording mode (0 = OFF, 1 = ON, 2 = TIME, 3 = TWILIGHT)");
  Serial.println("pao      get auto recording mode");
  Serial.println("Pah<val> set auto recording start hour (0 ... 23");
  Serial.println("pah      get auto recording start hour");
  Serial.println("Pam<val> set auto recording start minute (0 ... 59");
  Serial.println("pam      get auto recording start minute");
  Serial.println("Pai<val> set auto recording stop hour (0 ... 23");
  Serial.println("pai      get auto recording stop hour");
  Serial.println("Pan<val> set auto recording stop minute (0 ... 59");
  Serial.println("pan      get auto recording stop minute");
  Serial.println("Pb<val>  set backlight time [s]");
  Serial.println("pb       get backlight time [s]");
  Serial.println("Pf       set current voltage to calc voltage fact (unlock first)");
  Serial.println("Pls<val> set location source (0 = FIX, 1 = GPS)");
  Serial.println("pls      get location source");
  Serial.println("Pla<val> set location latitude (0 ... 90");
  Serial.println("pla      get location latitude");
  Serial.println("Plo<val> set location longitude (-180 ... 180");
  Serial.println("plo      get location longitude");
  Serial.println("Plh<val> set location heigth (0 ... 10000");
  Serial.println("plh      get location heigth");
  Serial.println("Pm<val>  set mixer frequency [kHz] ( 1 .. 79)");
  Serial.println("pm       get mixer frequency [kHz]");
  Serial.println("Pn<val>  set language (0 .. 1)");
  Serial.println("pn       get language");
  Serial.println("Pv<val>  set volume [dB] (-30 .. 18 dB)");
  Serial.println("pv       get volume [dB]");
  Serial.println("Prg<val> set gain (0 .. 1)");
  Serial.println("prg      get gain");  
  Serial.println("Prs<val> set sample rate (0 .. 8)");
  Serial.println("prs      get sample rate");
  Serial.println("Prt<val> set recording time [s] (1 .. 30)");
  Serial.println("prt      get recording time [s]");
  Serial.println("Prd<val> set dead time after recording [s] (1 .. 30)");
  Serial.println("prd      get dead time after recording [s]");
  Serial.println("Prh<val> set trig threshold [dB] (-24 ... -1)");
  Serial.println("prh      get trig threshold [dB]");
  Serial.println("Prr<val> set trig type (0=LEVEL, 1=FREQ, 2=LEVEL + FREQ)");
  Serial.println("prr      get trig type");
  Serial.println("Prf<val> set trig filter frequency [kHz] (5 .. 70)");
  Serial.println("prf<val> get trig filter frequency [kHz]");
  Serial.println("Pry<val> set trig filter type (0=HIGHPASS , 1=LOWPASS, 2=BANDPASS)");
  Serial.println("pry<val> get trig filter type");
  Serial.println("sa       get audio mem usage [%]");
  Serial.println("sb       get temperature [°C]");
  Serial.println("sc       get humidity [%]");
  Serial.println("sd       get date [YY.MM:DD] of internal clock");
  Serial.println("se       get play status [0=STOP, 1=PLAY, 2=REC, 3=DEAD_TIME");
  Serial.println("sf       get free disk space [MB]");
  Serial.println("sg       get audio processur usage [%]");
  Serial.println("sh       get current altitude [m]");
  Serial.println("si       get battery charge level [%]");
  Serial.println("sk       get last peak value [%]");
  Serial.println("sl       get current position (lat, lon)");
  Serial.println("sm       get main loop count [Hz]");
  Serial.println("sp       get GPS status (0=GPS_OFF , 1=GPS_SEARCHING, 2=GPS_FIXED, 3=GPS_FIXED_OFF");
  Serial.println("sr       get current count of recordings in project");
  Serial.println("ss       get number of satellites");
  Serial.println("st       get time [hh:mm:ss] of internal clock");
  Serial.println("su       get summarized status");
  Serial.println("sv       get battery voltage [V]");
  Serial.println("sx       get max. audio processor usage [%]");
  Serial.println("r<name>  dump file <name>");
  Serial.println("u        key cursor up");
  Serial.println("w<name>  write file <name> in curr. directory"); 
  Serial.println("W        key cursor down"); 
  Serial.println("v        display software version"); 
  Serial.println("x<name>  delete file/dir <name>");
  Serial.println("Y<Pwd>   set unlock password for system functions");
  Serial.println("y<Pwd>   '1': system locked, '0': system unlocked");
  Serial.println("Z<serNr> set serial nr (unlock first)");
  Serial.println("z        get serial nr");
}

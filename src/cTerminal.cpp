/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "globals.h"
#include <Arduino.h>
#include "cSdCard.h"
#include "cmenue.h"

extern cMenue menue;


cTerminal::cTerminal() 
{
}

void MEMF cTerminal::parseCmdfromUSB() 
{
  int numBytes = Serial.available();
  if(numBytes <= 0)
    return;
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
      execCmd(m_recbufUSB, m_recIdxUSB);
    else
      m_recIdxUSB++;
  }
}


void MEMF cTerminal::execCmd(char* buf, size_t& bufIdx)
{
  enSdRes rc;
  buf[bufIdx] = 0;
  tDirInfo dirInfo;
  size_t dirIdx;
  char ret;

  switch(buf[0]) 
  {
    case 'h':
      Serial.println("");
      showCommands();
      break;

    case 'c': 
      rc = cSdCard::inst().chdir(&buf[1]);
      if(rc != 0)
        Serial.printf("1\nerror changing directory: %i\n", rc);
      Serial.printf("0\n%s\n", cSdCard::inst().getActDir());
      Serial.write(0x04);
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
      
        Serial.write(0x04);
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

    case 'L':
      menue.load();
      break;

    case 'm':
      ret = cSdCard::inst().mkDir(&buf[1]);
      if(ret == 0)
        Serial.write('0');
      else
        Serial.write('1');
      Serial.write(0x04);      
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
          Serial.write(0x04);      
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
      Serial.write(0x04);
      break;

    case 'P':
      parseSetCmd(&buf[1]);
      break;
    
    case 'r':
      cSdCard::inst().sendFileToTerminal(&buf[1], devPars.sendDelay.get());
      break;
    

    case 's':
      menue.printStatus();
      break;

    case 'S':
      menue.save();
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
      Serial.write(0x04);
      break;

    case 'x':
    case 'X':
      ret = (char)cSdCard::inst().del(&buf[1]);
      ret += '0';
      if (ret > '9')
        ret += 7;
      Serial.write(ret);      
      Serial.write(0x04);
      break;  
      
    default:
      Serial.printf("unknown command: %c\n",buf[0]);
      break;
  }
  bufIdx = 0;
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
  Serial.write(0x04);
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

    case 'l':
      replyOk = parseLocationParams(&buf[1], true);
      break;
    case 'm':
      val = atoi(buf + 1);
      if((val >= 0) && (val <= 80))
        devPars.mixFreq.set(val);
      else
        replyOk = false;
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
  Serial.write(0x04);
}

void MEMF cTerminal::parseGetCmd(const char* buf)
{
  m_key = enKey::TER;
  bool replyOk = true;
  int val = 0;
  char replyBuf[32];
  switch (buf[0])
  {
  case 'a':
    replyOk = parseAutoRecParams(&buf[1], false, replyBuf, sizeof(replyBuf));
    break;

  case 'l':
    replyOk = parseLocationParams(&buf[1], false, replyBuf, sizeof(replyBuf));
    break;
  case 'm':
    val = atoi(buf + 1);
    if ((val >= 0) && (val <= 80))
      devPars.mixFreq.set(val);
    else
      replyOk = false;
    break;
  case 'r':
    replyOk = parseRecParams(&buf[1], true, replyBuf, sizeof(replyBuf));
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
    Serial.write(replyBuf);
  else
    Serial.write('?');
  Serial.write(0x04);
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
    case 'f':
      if (write)
        replyOk = setValEnum(buf + 1, 0, 1, devPars.recFmt);
      else
        snprintf(reply, replySize, "%i", (int)devPars.recFmt.get());
      break;
    case 'o':
      if (write)
        replyOk = setValEnum(buf + 1, PAR_AUTOMODE_MIN, PAR_AUTOMODE_MAX, devPars.recAuto);
      else
        snprintf(reply, replySize, "%u", (int)devPars.recAuto.get());
      break;
    case 'h':
      if (write)
        replyOk = setValInt(buf + 1, 0, 23, devPars.startH);
      else
        snprintf(reply, replySize, "%i", (int)devPars.startH.get());
      break;
    case 'H':
      if (write)
        replyOk = setValInt(buf + 1, 0, 23, devPars.stopH);
      else
        snprintf(reply, replySize, "%i", (int)devPars.stopH.get());
      break;
    case 'm':
      if (write)
        replyOk = setValInt(buf + 1, 0, 59, devPars.startMin);
      else
        snprintf(reply, replySize, "%i", (int)devPars.startMin.get());
      break;
    case 'M':
      if (write)
        replyOk = setValInt(buf + 1, 0, 59, devPars.stopMin);
      else
        snprintf(reply, replySize, "%i", (int)devPars.stopMin.get());
      break;
    case 'p':
      if (write)
        replyOk = setValEnum(buf + 1, 0, 1, devPars.projectType);
      else
        snprintf(reply, replySize, "%i", (int)devPars.projectType.get());
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
        snprintf(reply, replySize, "%i", (int)devPars.srcPosition.get());
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
        snprintf(reply, replySize, "%f", devStatus.geoPos.getLat());
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
        snprintf(reply, replySize, "%f", devStatus.geoPos.getLon());
      break;
    case 'h':
      if (write)
        replyOk = setValFloat(buf + 1, PAR_LOCSRC_MIN, PAR_LOCSRC_MAX, devStatus.height);
      else
        snprintf(reply, replySize, "%f", devStatus.height.get());
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
        snprintf(reply, replySize, "%f", devPars.deadTime.get());
      break;
    case 's':
      if (write)
        replyOk = setValEnum(buf + 1, PAR_SR_MIN, PAR_SR_MAX, devPars.sampleRate);
      else
        snprintf(reply, replySize, "%i", (int)devPars.sampleRate.get());
      break;
    case 't':
      if (write)
        replyOk = setValFloat(buf + 1, PAR_RECTIM_MIN, PAR_RECTIM_MAX, devPars.recTime);
      else
        snprintf(reply, replySize, "%f", devPars.recTime.get());
      break;
    case 'h':
      if (write)
        replyOk = setValFloat(buf + 1, PAR_RECTHRESH_MIN, PAR_RECTHRESH_MAX, devPars.recThreshhold);
      else
        snprintf(reply, replySize, "%f", devPars.recThreshhold.get());
      break;
    case 'r':
      if (write)
        replyOk = setValEnum(buf + 1, PAR_TRIGTYPE_MIN, PAR_TRIGTYPE_MAX, devPars.triggerType);
      else
        snprintf(reply, replySize, "%i", (int)devPars.triggerType.get());
      break;
    case 'f':
      if (write)
        replyOk = setValFloat(buf + 1, PAR_TRIGFILTFREQ_MIN, PAR_TRIGFILTFREQ_MAX, devPars.filtFreq);
      else
        snprintf(reply, replySize, "%f", devPars.filtFreq.get());
      break;
    case 'm':
      if (write)
        replyOk = setValFloat(buf + 1, PAR_TRIGEVENT_MIN, PAR_TRIGEVENT_MAX, devPars.minEventLen);
      else
        snprintf(reply, replySize, "%f", devPars.minEventLen.get());
      break;
    case 'y':
      if (write)
        replyOk = setValEnum(buf + 1, 0, 1, devPars.filtType);
      else
        snprintf(reply, replySize, "%i", (int)devPars.filtType.get());
      break;
    case 'p':
      if (write)
        replyOk = setValInt(buf + 1, PAR_PRETRIG_MIN, PAR_PRETRIG_MAX, devPars.preTrigger);
      else
        snprintf(reply, replySize, "%i", (int)devPars.preTrigger.get());
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


void MEMF cTerminal::showCommands() 
{
  Serial.println("Available commands:");
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
  Serial.println("g        GPS test cmd: Serial connected to GPS, terminate with 'q!'");
  Serial.println("L        load parameters from EEPROM");
  Serial.println("n<old> <new> rename file or directory");  
  Serial.println("o        key OK");
  Serial.println("p        print parameters");
  Serial.println("Pao<val> set auto recording mode (0 = OFF, 1 = ON, 2 = TIME, 3 = TWILIGHT)");
  Serial.println("pao<val> get auto recording mode");
  Serial.println("Paf<val> set auto recording file format (0 = RAW, 1 = WAV)");
  Serial.println("paf<val> get auto recording file");
  Serial.println("Pap<val> set auto recording project format (0 = DATE_TIME, 1 = ELEKON");
  Serial.println("pap<val> get auto recording project format");
  Serial.println("Pah<val> set auto recording start hour (0 ... 23");
  Serial.println("pah<val> get auto recording start hour");
  Serial.println("Pam<val> set auto recording start minute (0 ... 59");
  Serial.println("pam<val> get auto recording start minute");
  Serial.println("PaH<val> set auto recording stop hour (0 ... 23");
  Serial.println("paH<val> get auto recording stop hour");
  Serial.println("PaM<val> set auto recording stop minute (0 ... 59");
  Serial.println("paM<val> get auto recording stop minute");
  Serial.println("Pls<val> set location source (0 = FIX, 1 = GPS)");
  Serial.println("pls<val> get location source");
  Serial.println("Pla<val> set location latitude (0 ... 90");
  Serial.println("pla<val> get location latitude");
  Serial.println("Plo<val> set location longitude (-180 ... 180");
  Serial.println("plo<val> get location longitude");
  Serial.println("Plh<val> set location heigth (0 ... 10000");
  Serial.println("plh<val> get location heigth");
  Serial.println("Pm<val>  set mixer frequency [kHz] ( 1 .. 79)");
  Serial.println("pm<val>  get mixer frequency [kHz]");
  Serial.println("Pv<val>  set volume [dB] (-30 .. 18 dB)");
  Serial.println("pv<val>  get volume [dB]");
  Serial.println("Prs<val> set sample rate (0 .. 8)");
  Serial.println("prs<val> get sample rate");
  Serial.println("Prt<val> set recording time [s] (1 .. 30)");
  Serial.println("prt<val> get recording time [s]");
  Serial.println("Prd<val> set dead time after recording [s] (1 .. 30)");
  Serial.println("prd<val> get dead time after recording [s]");
  Serial.println("Prh<val> set trig threshold [dB] (-24 ... -1)");
  Serial.println("prh<val> get trig threshold [dB]");
  Serial.println("Prr<val> set trig type (0=LEVEL, 1=FREQ, 2=LEVEL + FREQ)");
  Serial.println("prr<val> get trig type");
  Serial.println("Prf<val> set trig filter frequency [kHz] (5 .. 70)");
  Serial.println("prf<val> get trig filter frequency [kHz]");
  Serial.println("Pry<val> set trig filter type (0=HIGHPASS , 1=LOWPASS, 2=BANDPASS)");
  Serial.println("pry<val> get trig filter type");
  Serial.println("s        print status");
  Serial.println("S        save parameters to EEPROM");
  Serial.println("r<name>  dump file <name>");
  Serial.println("u        key cursor up");
  Serial.println("w<name>  write file <name> in curr. directory"); 
  Serial.println("W        key cursor down"); 
  Serial.println("v        display software version"); 
  Serial.println("x<name>  delete file/dir <name>");
}

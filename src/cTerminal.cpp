/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}.com             *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "cTerminal.h"
#include <Arduino.h>
#include "cSdCard.h"
#include "cmenue.h"

extern cMenue menue;


cTerminal::cTerminal() {
}

void cTerminal::parseCmd() {
  int numBytes = Serial.available();
  if(numBytes <= 0)
    return;
  while(numBytes > 0) {
    m_recbuf[m_recIdx] = Serial.read();
    numBytes--;
    if(m_recbuf[m_recIdx] == '\n')
      execCmd();
    else
      m_recIdx++;
  }
}

void cTerminal::execCmd() {
  enSdRes rc;
  m_recbuf[m_recIdx] = 0;
  tDirInfo dirInfo;
  size_t dirIdx;
  char ret;

  switch(m_recbuf[0]) {

    case 'h':
      Serial.println("");
      showCommands();
      break;

    case 'c': 
      rc = cSdCard::inst().chdir(&m_recbuf[1]);
      if(rc != 0)
        Serial.printf("1\nerror changing directory: %i\n", rc);
      Serial.printf("0\n%s\n", cSdCard::inst().getActDir());
      Serial.write(0x04);
      break;

    case 'C':
      parseControlCmd(&m_recbuf[1]);
      break;
      
    case 'd':
      {
        Serial.println("");
        dirIdx = 0;
        char* p = NULL;
        if((m_recbuf[1] == '*') && (m_recbuf[2] == '.')) {
          p = &m_recbuf[3];
          m_recbuf[6] = 0;
        }
        do {
          rc = cSdCard::inst().dir(dirInfo, true, p, dirIdx);
          dirIdx += MAX_DIRENTRIES;
        } while(dirInfo.size() >= MAX_DIRENTRIES);
      
        Serial.write(0x04);
      }
      break;
      
    case 'D':
      if(m_recbuf[1] == '0') {
        devStatus.forceDisplay = 0;
        Serial.write('0');
      }
      else if(m_recbuf[1] == '1') {
        devStatus.forceDisplay = 1;
        Serial.write('0');
      }
      else
        Serial.write('1');      
      break;
      
    case 'm':
      ret = cSdCard::inst().mkDir(&m_recbuf[1]);
      if(ret == 0)
        Serial.write('0');
      else
        Serial.write('1');
      Serial.write(0x04);      
      break;
      
    case 'n':
      {
        char oldName[FILENAME_LEN];
        char newName[FILENAME_LEN];
        char* p = strchr(&m_recbuf[1], ' ');
        *p = 0;
        if(p != NULL) {
          strcpy(oldName, &m_recbuf[1]);    
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
      menue.printPars();
      Serial.write(0x04);
      break;

    case 'P':
      parseSetCmd(&m_recbuf[1]);
      break;
    
    case 'r':
      cSdCard::inst().sendFileToTerminal(&m_recbuf[1], devPars.sendDelay.get());
      break;
    

    case 's':
      menue.printStatus();
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
      cSdCard::inst().readFileFromTerminal(&m_recbuf[1]);      
      break;
    case 'v':
      Serial.write(devStatus.version.get());      
      Serial.write(0x04);
      break;

    case 'x':
    case 'X':
      ret = (char)cSdCard::inst().del(&m_recbuf[1]);
      ret += '0';
      if (ret > '9')
        ret += 7;
      Serial.write(ret);      
      Serial.write(0x04);
      break;  
      
    default:
      Serial.printf("unknown command: %c\n",m_recbuf[0]);
      break;
  }
  m_recIdx = 0;
}

void cTerminal::parseControlCmd(const char* buf) {
  m_key = enKey::TER;
  bool replyOk = true;
  switch(buf[0]) {
    case 'p':
      devStatus.playStatus.set(1);
      break;
    case 'r':
      devStatus.playStatus.set(2);
      break;
    case 's':
      devStatus.playStatus.set(0);
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

void cTerminal::parseSetCmd(const char* buf) {
  m_key = enKey::TER;
  bool replyOk = true;
  int val = 0;
  switch(buf[0]) {
    case 'm':
      val = atoi(buf+1);
      if((val >= 0) && (val <= 80))
        devPars.mixFreq.set(val);
      else
        replyOk = false;
      break;
    case 'v':
      val = atoi(buf+1);
      if((val > -30) && (val <= 18))
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


void cTerminal::showCommands() {
  Serial.println("Available commands:");
  Serial.println("c<name>  change directory");
  Serial.println("Cf<name> set play name");  
  Serial.println("Cm<mode> set display mode 0..3");       
  Serial.println("Cr       record");
  Serial.println("Cp       play");
  Serial.println("Cs       stop");
  Serial.println("d        show directory");
  Serial.println("D0       Debug: force display activity off");
  Serial.println("D1       Debug: force display activity on");
  Serial.println("n<old> <new> rename file or directory");  
  Serial.println("o        key OK");
  Serial.println("p        print parameters");
  Serial.println("Pm<val>  set mixer frequency [kHz] ( 1 .. 79)");
  Serial.println("Pv<val>  set volume [dB] (-30 .. 18 dB)");
  Serial.println("s        print status");
  Serial.println("r<name>  dump file <name>");
  Serial.println("u        key cursor up");
  Serial.println("w<name>  write file <name> in curr. directory"); 
  Serial.println("W        key cursor down"); 
  Serial.println("v        display software version"); 
  Serial.println("x<name>  delete file/dir <name>");
}

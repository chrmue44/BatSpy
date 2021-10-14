/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "cCassette.h"
#include "cfileinfo.h"
#include "cutils.h"
#include <cstring>
#include "cmenue.h"
#include "cRtc.h"
//#define DEBUG_LEVEL 1
#include "debug.h"

#ifndef ARDUINO_TEENSY41
extern "C" uint32_t usd_getError(void);
#endif   //#ifndef ARDUINO_TEENSY41

cCassette::cCassette(AudioAnalyzePeak& peak) :
m_peak(peak)
{

}

void cCassette::setSamplingRate(uint32_t s) {
  m_sampleRate = s;
  m_player.setSampleRate(s);
}

int cCassette::startRec(float recTime, enRecFmt recFmt) 
{
  DPRINTLN1("cCassette::startRecording()");
  m_maxRecSamples = calcRecSamples(recTime);
  if (m_isRecFileOpen)
  {
    enSdRes rc = cSdCard::inst().closeFile(m_fil);
    if (rc != OK) 
      return 1;
    m_isRecFileOpen = false;
  }

  if (!m_isRecFileOpen) 
  {
    enSdRes ret = createRecordingDir();
    if(ret == OK)
    {
      char buf[16];
      char ext[5];
      switch(recFmt)
      {
        case enRecFmt::RAW:
          strncpy(ext, "raw", sizeof(ext));
          break;
        case enRecFmt::WAV:
          strncpy(ext, "wav", sizeof(ext));
          break;
      }
      snprintf(buf,sizeof(buf),"/%02i%02i.%s",m_min, m_sec, ext);
      strcat(m_fileName, buf);
      return startRec(recFmt);
    }
  }
  return 0;
}


int cCassette::startRec(const char* name, float recTime, enRecFmt recFmt) {
  m_maxRecSamples = calcRecSamples(recTime);
  strcpy(m_fileName, name);
  return startRec(recFmt);
}

int cCassette::startRec(enRecFmt recFmt) {
  m_mode = enCassMode::REC;
  cSdCard& sd = cSdCard::inst();
  enSdRes rc = sd.openFile(m_fileName, m_fil, WRITE);
    // check if file is Good
  if (rc != OK) { // only option is to close file
    sd.closeFile(m_fil);
   return 1;
  }
  else
    m_isRecFileOpen = true;
  m_sampleCnt = 0;
  m_recFmt = recFmt;
  if(recFmt == enRecFmt::WAV)
    writeWavHeader();

  m_recorder.begin();
  return 0;
}


int cCassette::operate()
{
  enSdRes rc = enSdRes::OK;
  if (m_mode == enCassMode::STOP)
  {
    return 0;
  }
  else if (m_mode == enCassMode::REC)
  {
    size_t av = m_recorder.available();
    if (av >= N_BUFFER  )
    {// one buffer = 256 (8bit)-bytes = block of 128 16-bit samples
      if(av > sizeof(m_buffern) / AUDIO_BLOCK_SAMPLES / sizeof(int16_t))
        av = sizeof(m_buffern) / AUDIO_BLOCK_SAMPLES / sizeof(int16_t);
      for (size_t i = 0; i < av; i++)
      {
        memcpy(m_buffern + i * AUDIO_BLOCK_SAMPLES * sizeof(int16_t),
               m_recorder.readBuffer(), AUDIO_BLOCK_SAMPLES * sizeof(int16_t));
        m_recorder.freeBuffer();
      }
      m_sampleCnt += av * AUDIO_BLOCK_SAMPLES;

      cSdCard& sd = cSdCard::inst();
      size_t cnt = av * AUDIO_BLOCK_SAMPLES * sizeof(int16_t);
      rc = sd.writeFile (m_fil, (const char*)m_buffern, m_wr, cnt);
      DPRINTF1("wr buf %lu\n", cnt);
      if (rc != OK)
      { // IO error
        m_mode = enCassMode::STOP;
        return 1;
      }
    }

    if(m_sampleCnt >= m_maxRecSamples)
      stop();
    return 0;
  }

  else if(m_mode == enCassMode::PLAY)
  {
    if (!m_player.isPlaying())
    {
      m_player.stop();
      DPRINTLN1("File is over");
      m_mode = enCassMode::STOP;
    }
  } 
  return 0;
}


int cCassette::stop()
{
  enSdRes rc = enSdRes::OK;
  float peakVal = -1;
  if (m_mode == enCassMode::REC)
  {
    m_recorder.end();
    cSdCard& sd = cSdCard::inst();
    while (m_recorder.available() > 0)
    {
      rc = sd.writeFile(m_fil, (char*)m_recorder.readBuffer(), m_wr, AUDIO_BLOCK_SAMPLES * sizeof(int16_t));
      m_recorder.freeBuffer();
      m_sampleCnt += AUDIO_BLOCK_SAMPLES;
    }
    if(m_recFmt == enRecFmt::WAV)
      finalizeWavFile();

    //close file
    rc = sd.closeFile(m_fil);
    if (m_peak.available())
      peakVal = m_peak.read();
    
    writeInfoFile(peakVal);
    if (rc)
      return 1;
    //
    m_isRecFileOpen = false;
    DPRINTLN1(" Recording stopped!");
  }
  
  else if(m_mode == enCassMode::PLAY)
  {
    DPRINTLN1("stopPlaying");
    m_player.stop();
  }
  m_mode = enCassMode::STOP;
  return 0;
}


void cCassette::startPlay()
{
  DPRINTLN1("startPlaying ");
  DPRINTF1("Playfile: %s\n", m_fileName);
  delay(100);
  m_player.play(m_fileName);
  m_mode = enCassMode::PLAY;
}

void cCassette::writeWavHeader()
{
  //https://www.cplusplus.com/forum/beginner/166954/
  char buf[24];
  memcpy(buf,"RIFF----WAVEfmt ", 16);     // (chunk size to be filled in later)
  //f_write(&m_fil, buf,16, &m_wr);
  cSdCard::inst().writeFile(m_fil, buf, m_wr, 16);
  writeWord(16, 4 );  // no extension data
  writeWord( 1, 2 );  // PCM - integer samples
  writeWord( 1, 2 );  // one channel (mono file)
  writeWord( m_sampleRate, 4 );  // samples per second (Hz)
  writeWord( m_sampleRate * 2, 4 );  // (Sample Rate * BitsPerSample * Channels) / 8
  writeWord( 2, 2 );  // data block size (size of two integer samples, one for each channel, in bytes)
  writeWord(16, 2 );  // number of bits per sample (use a multiple of 8)

  // Write the data chunk header
  memcpy(buf,"data----", 8);     // (chunk size to be filled in later)
//  f_write(&m_fil, buf,8, &m_wr);
  cSdCard::inst().writeFile(m_fil, buf, m_wr, 8);
}


void cCassette::writeWord(uint32_t value, size_t size) 
{
  for (; size; --size, value >>= 8) 
  {
    char val = value & 0xFF;
//    f_write(&m_fil, &val, 1, &m_wr);
    cSdCard::inst().writeFile(m_fil, &val, m_wr, 1);
//    &m_fil.put( static_cast <char> (value & 0xFF) );
  }
}

void cCassette::finalizeWavFile() 
{
  
   // (We'll need the final file size to fix the chunk sizes above)
  //size_t file_length = f.tellp();
  size_t file_length = cSdCard::inst().fileSize(m_fil);
  // Fix the data chunk header to contain the data size
//  f.seekp( data_chunk_pos + 4 );
  cSdCard::inst().setFilePos(m_fil, WAV_DATACHUNK_POS + 4);
  writeWord(file_length - WAV_DATACHUNK_POS + 8 );

  // Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
  cSdCard::inst().setFilePos(m_fil, 0 + 4);
  //f.seekp( 0 + 4 );
  writeWord(file_length - 8, 4 ); 
}


void cCassette::writeInfoFile(float peakVal)
{
  cFileInfo info;
  char date [32];
  char infoFile[80];
 
  strncpy(infoFile, m_fileName, sizeof(infoFile));
  if (m_recFmt == enRecFmt::RAW)
    cUtils::replace(m_fileName, ".raw", ".xml", infoFile, sizeof(infoFile));
  if (m_recFmt == enRecFmt::WAV)
    cUtils::replace(m_fileName, ".wav", ".xml", infoFile, sizeof(infoFile));
  
  snprintf(date,sizeof(date),"%02i.%02i.%02i %02i:%02i:%02i",m_day, m_month, m_year, m_hour, m_min, m_sec);
  float duration = (float)m_sampleCnt/ m_sampleRate;
  info.write(infoFile, duration, m_sampleRate, date, cUtils::getFileName(m_fileName), 
              devStatus.geoPos.getLat(), devStatus.geoPos.getLon(), peakVal); 
}

enSdRes cCassette::createRecordingDir()
{
  char buf[80];

  m_year = year();
  m_month = month();
  m_day = day();
  m_hour = hour();
  m_min = minute();
  m_sec = second();

  snprintf(m_fileName, sizeof(m_fileName),"/rec/%02i/%02i/%02i/%02i",m_year, m_month, m_day, m_hour);
  enSdRes ret = cSdCard::inst().chdir(m_fileName);
  if(ret != OK)
  {
    ret = cSdCard::inst().chdir("/rec");
    if(ret != OK)
    {
      ret = cSdCard::inst().mkDir("/rec");
    }
    snprintf(buf, sizeof(buf),"/rec/%02i",m_year);
    ret = cSdCard::inst().chdir(buf);
    if(ret != OK)
    {
      ret = cSdCard::inst().mkDir(buf);
    }
    snprintf(buf, sizeof(buf),"/rec/%02i/%02i", m_year, m_month);
    ret = cSdCard::inst().chdir(buf);
    if(ret != OK)
    {
      ret = cSdCard::inst().mkDir(buf);
    }      
    snprintf(buf, sizeof(buf),"/rec/%02i/%02i/%02i",m_year ,m_month ,m_day);
    ret = cSdCard::inst().chdir(buf);
    if(ret != OK)
    {
      ret = cSdCard::inst().mkDir(buf);
    }      
    snprintf(buf, sizeof(buf),"/rec/%02i/%02i/%02i/%02i",m_year ,m_month, m_day, m_hour);
    ret = cSdCard::inst().chdir(buf);
    if(ret != OK)
    {
      ret = cSdCard::inst().mkDir(buf);
    }      
    ret = cSdCard::inst().chdir(m_fileName);
  }
  return ret;
}

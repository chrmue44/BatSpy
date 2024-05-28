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
//#define DEBUG_LEVEL 4
#include "debug.h"

//#ifndef ARDUINO_TEENSY41
//extern "C" uint32_t usd_getError(void);
//#endif   //#ifndef ARDUINO_TEENSY41

cCassette::cCassette() 
{

}

void cCassette::setSamplingRate(uint32_t s)
{
  m_sampleRate = s;
  m_player.setSampleRate(s);
}

int cCassette::startRec(const char* name, float recTime)
{
  m_maxRecSamples = calcRecSamples(recTime);
  strncpy(m_fileName, name, sizeof (m_fileName) - 1);
  return startRec();
}

int cCassette::startRec()
{
  m_mode = enCassMode::REC;
  DPRINTF4("cCassete:startRec: %s,  samples: %i", m_fileName, m_maxRecSamples);
  cSdCard& sd = cSdCard::inst();
  enSdRes rc = sd.openFile(m_fileName, m_fil, WRITE);
    // check if file is Good
  if (rc != OK) { // only option is to close file
    sd.closeFile(m_fil);
    m_mode = enCassMode::STOP;
    return 1;
  }
  else
    m_isRecFileOpen = true;

  m_sampleCnt = 0;
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
      AudioNoInterrupts();
      for (size_t i = 0; i < av; i++)
      {
        memcpy(m_buffern + i * AUDIO_BLOCK_SAMPLES * sizeof(int16_t),
        m_recorder.readBuffer(), AUDIO_BLOCK_SAMPLES * sizeof(int16_t));
        m_recorder.freeBuffer();
      }
      AudioInterrupts();
      m_sampleCnt += av * AUDIO_BLOCK_SAMPLES;

      cSdCard& sd = cSdCard::inst();
      size_t cnt = av * AUDIO_BLOCK_SAMPLES * sizeof(int16_t);
      rc = sd.writeFile (m_fil, (const char*)m_buffern, m_wr, cnt);
      DPRINTF1("wr buf %lu\n", cnt);
      if (rc != OK)
      { // IO error
        m_mode = enCassMode::STOP;
        m_isRecFileOpen = false;
        DPRINTF4("error writing SD: %i\n", rc);
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
  //float peakVal = -1.0;
  DPRINTLN4("cCassette:stop recording");
  if (m_mode == enCassMode::REC)
  {
    m_recorder.end();
    cSdCard& sd = cSdCard::inst();
    while (sd.isMounted() && (m_recorder.available() > 0))
    {
      rc = sd.writeFile(m_fil, (char*)m_recorder.readBuffer(), m_wr, AUDIO_BLOCK_SAMPLES * sizeof(int16_t));
      m_recorder.freeBuffer();
      DPRINTF1("wr buf %lu\n",  AUDIO_BLOCK_SAMPLES * sizeof(int16_t));
      m_sampleCnt += AUDIO_BLOCK_SAMPLES;
    }
    
    finalizeWavFile();

    rc = sd.closeFile(m_fil);
    DPRINTLN4("cCassette::stop: Recording stopped!");
    m_isRecFileOpen = false;
    m_mode = enCassMode::STOP;
    if (rc != enSdRes::OK)
      return 1;
    return 0;
  }
  
  else if(m_mode == enCassMode::PLAY)
  {
    DPRINTLN4("stopPlaying");
    m_player.stop();
  }
  m_mode = enCassMode::STOP;
  return 0;
}


void cCassette::startPlay()
{
  DPRINTLN1("startPlaying ");
  DPRINTF4("Playfile: %s\n", m_fileName);
  delay(100);
  m_player.play(m_fileName);
  m_mode = enCassMode::PLAY;
}

void cCassette::writeWavHeader()
{
  //https://www.cplusplus.com/forum/beginner/166954/
  char buf[24];
  memcpy(buf,"RIFF----WAVEfmt ", 16);     // (chunk size to be filled in later)
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
  cSdCard::inst().writeFile(m_fil, buf, m_wr, 8);
}


void cCassette::writeWord(uint32_t value, size_t size) 
{
  for (; size; --size, value >>= 8) 
  {
    char val = value & 0xFF;
    cSdCard::inst().writeFile(m_fil, &val, m_wr, 1);
  }
}

void cCassette::finalizeWavFile() 
{
  
  size_t file_length = cSdCard::inst().fileSize(m_fil);
  cSdCard::inst().setFilePos(m_fil, WAV_DATACHUNK_POS + 4);
  writeWord(file_length - WAV_DATACHUNK_POS + 8 );

  cSdCard::inst().setFilePos(m_fil, 0 + 4);
  writeWord(file_length - 8, 4 ); 
}




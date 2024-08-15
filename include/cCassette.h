/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * The cCassette class encapsulates the recording and play
 * back functionality based on external SD-Card libraries
 * and the Teensy Audio Library
 * ***********************************************************/
#ifndef CCASSETTE_H
#define CCASSETTE_H

//#define WAV
#ifdef SIMU_DISPLAY
#include <Audio_sim.h>
#else
#include <Audio.h>
#endif
#include <cSdCard.h>

#include "types.h"
#ifdef WAV
#include "play_fats_wav.h"
#else
#include "play_fats_raw.h"
#endif
#include "ctimer.h"

#define WAV_DATACHUNK_POS  36

//#define MXFN 100 // maximal number of files
#if defined(ARDUINO_TEENSY41) || defined(ARDUINO_TEENSY40)
  #define N_BUFFER 8  // min. nr of buffers to write to SD card
  #ifdef SIMU_DISPLAY
    #define BUFFSIZE (384000 / 10 *sizeof (int16_t))
  #else
    #define BUFFSIZE (8 * N_BUFFER * AUDIO_BLOCK_SAMPLES * sizeof(int16_t)) // size of buffer
  #endif
#elif defined(__MK20DX256__)
  #define BUFFSIZE (8*1024) // size of buffer to be written
#elif defined(__MK66FX1M0__)
//#define BUFF 96
  #define BUFF 48
  #define BUFFSIZE (BUFF*1024) // size of buffer to be written
#endif


class cCassette
{
  public:
    cCassette();
    int startRec(const char* name, float recTime);
    int startRec();
    int stop();
    void startPlay();
    int operate();
    void setFileName(const char* name) { strncpy(m_fileName, name, sizeof(m_fileName)); }
    AudioRecordQueue& getRecorder() { return m_recorder; }
#ifdef WAV
    AudioPlayFatsWav& getPlayer() { return m_player; }
#else
    AudioPlayFatsRaw& getPlayer() { return m_player; }
#endif
    void setSamplingRate(uint32_t s);
    enCassMode getMode() { return m_mode; }

    /**
     * return the play time of the actual title [s] 
     */
    float getRunTime() { return m_player.getRunTime(); }

    /**
     *  returns the length of the actual title [s]
     */
    float getTitleTime() { return m_player.getTitleTime(); }

    size_t getSampleCnt() { return m_sampleCnt; }
    uint32_t getErrCounter() {return m_errCount; }

  private:
    void writeWavHeader();
    void writeWord(uint32_t value, size_t size = sizeof(uint32_t));
    void finalizeWavFile();
    uint32_t calcRecSamples(float time) { return (uint32_t)(time * (float)m_sampleRate); }

  private:
    enCassMode m_mode = enCassMode::STOP;
    AudioRecordQueue m_recorder;
    bool m_isRecFileOpen = false;
    bool m_isPlayFileOpen = false;
    tFILE m_fil;                     ///< File object
#ifdef SIMU_DISPLAY
    uint8_t m_buffern[BUFFSIZE];
#else
    uint8_t m_buffern[BUFFSIZE] __attribute__((aligned(16)));
#endif
    uint32_t m_nj = 0;
    size_t m_wr;
    uint32_t m_sampleRate;
    uint32_t m_sampleCnt;
    uint32_t m_maxRecSamples;
    uint32_t m_errCount = 0;

#ifdef WAV
    AudioPlayFatsWav m_player;
#else
    AudioPlayFatsRaw m_player;
#endif
    char m_fileName[80];
};

#endif  //#ifndef CCASSETTE_H

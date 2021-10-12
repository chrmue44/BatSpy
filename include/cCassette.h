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

#include "types.h"
//#define WAV
#include "Audio.h"
#include "cSdCard.h"
#ifdef WAV
#include "play_fats_wav.h"
#else
#include "play_fats_raw.h"
#endif
#include "ctimer.h"

#define WAV_DATACHUNK_POS  36

//#define MXFN 100 // maximal number of files
#if defined(ARDUINO_TEENSY41)
#define N_BUFFER 8  // min. nr of buffers to write to SD card
#define BUFFSIZE (3 * N_BUFFER * AUDIO_BLOCK_SAMPLES * sizeof(int16_t)) // size of buffer
#elif defined(__MK20DX256__)
#define BUFFSIZE (8*1024) // size of buffer to be written
#elif defined(__MK66FX1M0__)
//#define BUFF 96
#define BUFF 48
#define BUFFSIZE (BUFF*1024) // size of buffer to be written
#endif


class cCassette {
  public:
    cCassette(AudioAnalyzePeak& peak);
    int startRec(float recTime, enRecFmt recFmt);
    int startRec(const char* name, float recTime, enRecFmt recFmt);
    int startRec(enRecFmt recFmt);
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

  protected:
    cCassette();

  private:
    void writeWavHeader();
    void writeInfoFile(float peakVal);
    void writeWord(uint32_t value, size_t size = sizeof(uint32_t));
    void finalizeWavFile();
    enSdRes createRecordingDir();
    uint32_t calcRecSamples(float time) { return (uint32_t)(time * (float)m_sampleRate); }

  private:
    enCassMode m_mode = STOP;
    AudioRecordQueue m_recorder;
    bool m_isRecFileOpen = false;
    bool m_isPlayFileOpen = false;
    //FATFS m_fatfs;                 ///< File system object
    tFILE m_fil;                     ///< File object

    uint8_t m_buffern[BUFFSIZE] __attribute__( ( aligned ( 16 ) ) );
    uint32_t m_nj = 0;
    size_t m_wr;
    uint32_t m_sampleRate;
    int m_year;
    int m_month;
    int m_day;
    int m_hour;
    int m_min;
    int m_sec;
    enRecFmt m_recFmt;
    AudioAnalyzePeak& m_peak;
    uint32_t m_sampleCnt;
    uint32_t m_maxRecSamples;

#ifdef WAV
    AudioPlayFatsWav m_player;
#else
    AudioPlayFatsRaw m_player;
#endif
    char m_fileName[80];
};

#endif  //#ifndef CCASSETTE_H

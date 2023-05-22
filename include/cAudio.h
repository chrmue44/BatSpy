/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * ***********************************************************/
#ifndef _CAUDIO_H
#define _CAUDIO_H

#include "cCassette.h"
#include "ctimer.h"
#include "cRtc.h"
#include <Audio.h>
#include "types.h"
#include "config.h"
#include "cproject.h"
#include "cTrigger.h"

#include <cstdint>
#include <cstddef>

class cMenue;


// channel numbers for audio mixer
#define MIX_CHAN_MIC     0
#define MIX_CHAN_PLAY    1
#define MIX_CHAN2        2
#define MIX_CHAN3        3

#define LIVE_CNT_EXTEND  20            ///< counter ticks to extend live display after signal drops below threshold 

// audio settings that cause a change in the settings (and a therefor a longer pause of a few 100 ms)
struct stAudioSettings
{
  uint32_t sampleRate = 352000;        ///< current sampling rate [Hz]
  enSampleRate parSampleR = SR_384K;   ///< selected sample rate (enumerator)
  float oscFrequency = 0.0;
  enOpMode opMode;
  float volume = 1.0;
};
  
// sampl rate description
struct stSrDesc
{
  const enSampleRate SR_n;
  const uint32_t osc_frequency;
  const uint32_t MUL_Fs;
  const uint32_t DIV_Fs;
  const enSampleRate stretched;
  const uint32_t FFT_N;
};



/***
 * contains everything needed for audio processing
 * The cAudio class encapsulates the complete audio
 * functionality based on the Teensy Audio Library 
 */
class cAudio
{
private:
  AudioInputSpiMono        m_audioIn;   // audio shield: mic or line-in
  AudioOutputMQS           m_audioOut;  // medium quality output Teensy 4.x
  AudioSynthWaveformSine   m_sineHet;   // sinus generator for heterodyne
  AudioEffectMultiply      m_mult1;     // multiplier for heterodyne
  AudioMixer4              m_mixer;     // selector for input: player or mic
  AudioFilterBiquad        m_filter;    // filter before peak detection
  AudioEffectDelay         m_delay;     // delay for pre trigger
  cCassette                m_cass;      // player/recorder
  AudioAnalyzePeak         m_peak;
  AudioAnalyzeFFT1024      m_fft;       // FFT analyzer
  AudioFilterBiquad        m_filtDisp;  // high pass filter for live display

  AudioConnection m_cMi2Mx; // mic to mixer
  AudioConnection m_cCa2Mx; // player to mixer
  AudioConnection m_cMx2Mu; // mixer to multiplier
  AudioConnection m_cSi2Mu; // sine to multiplier
  AudioConnection m_cMi2Fi; // microphone to filter
  AudioConnection m_cFi2Pk; // filter to peak detector
  AudioConnection m_cMu2Ol; // multiplier to audio output left
  AudioConnection m_cMu2Or; // multiplier to audio output right
  AudioConnection m_cMi2De; // microphone to delay
  AudioConnection m_cDe2Ca; // delay to recorder
  AudioConnection m_cMi2Hp; // microphone to high pass
  AudioConnection m_cHp2Ft; // high pass to fft

  uint32_t m_sampleRate;            // sample rate in Hz  
  uint32_t m_oscFreq = 45000;       // start heterodyne detecting at this frequency
  float m_freq_oscillator = 50000;  // oscillator frequency calculated for m_sine (depending on SR)
  stAudioSettings m_old;            // old settings
  cTimer m_timeout;                 // blocking time out after recording 
  int m_oldCassMode = -1;
  cPrjoject m_prj;
  stFftInfo m_fftInfo;              // some variables to handle FFT operation
  cTrigger m_trigger;               // state machine to detect trigger

 public:
  cAudio();
  void init();
  static int32_t getSampleRateHz(enSampleRate sr);
  static size_t getFftOutputSize() { return sizeof(AudioAnalyzeFFT1024::output) / sizeof(AudioAnalyzeFFT1024::output[0]); }
  void setSampleRate(enSampleRate sr);
  void setPreAmpType(enPreAmp type);
  void setPreAmpGain(enGain gain);
  void setPreAmpGain(enGainRevB gain);
  void setup();
  void updateCassMode();
  void checkAutoRecording(cMenue& menue, cRtc& rtc);
  void operate(bool liveFft);
  float getLastPeakVal() { return m_trigger.lastPeakVal();}
  bool isRecording() { return m_cass.getMode() == enCassMode::REC;}
  void closeProject() { if (m_prj.getIsOpen()) m_prj.closePrjFile(); }

 private:
  void setMixOscFrequency(float freq);
  void setTrigFilter(float freq, enFiltType type);
  bool isSetupNeeded();
  void setAudioConnections(int i) {}
  void calcLiveFft();
  void startRec();
  void addLiveMsPerDiv(int waitTick, int sampleRate);
  int getLiveMsPerDiv(int waitTick, int sampleRate);

};

#endif   //#ifndef _CAUDIO_H

#ifndef _CAUDIO_H
#define _CAUDIO_H

#include <Audio.h>
#include "cCassette.h"
#include "ctimer.h"
#include "types.h"

class cMenue;

// *********************************SAMPLING ****************************


// channel numbers for audio mixer
#define MIX_CHAN_MIC     0
#define MIX_CHAN_PLAY    1
#define MIX_CHAN2        2
#define MIX_CHAN3        3


// audio settings that cause a change in the settings (and a therefor a longer pause of a few 100 ms)
struct stAudioSettings
{
  enSampleRate sampleRate = SR_44K;
  float oscFrequency = 0.0;
  enOpMode opMode;  
};
  
// sampl rate description
struct stSrDesc
{
  const enSampleRate SR_n;
  const u_int osc_frequency;
  const u_int MUL_Fs;
  const u_int DIV_Fs;
  const enSampleRate stretched;
  const u_int FFT_N;
};


/***
 * contains everything needed for audio processing
 */
class cAudio
{
 private:
  AudioInputI2S            m_audioIn;   // audio shield: mic or line-in
  AudioOutputI2S           m_audioOut;  // audio shield: headphones & line-out
  AudioSynthWaveformSine   m_sineHet;   // sinus generator for heterodyne
  AudioEffectMultiply      m_mult1;     // multiplier for heterodyne
  AudioMixer4              m_mixer;     // selector for input: player or mic
  AudioAnalyzePeak         m_peak;      // peak detector
  AudioFilterStateVariable m_filter;    // filter before peak detection
  cCassette                m_cass;      // player/recorder


  AudioConnection m_cMi2Mx; // mic to mixer
  AudioConnection m_cCa2Mx; // player to mixer
  AudioConnection m_cMx2Mu; // mixer to multiplier
  AudioConnection m_cSi2Mu; // sine to multiplier
  AudioConnection m_cMi2Fi; // microphone to filter
  AudioConnection m_cFi2Pk; // filter to peak detector
  AudioConnection m_cMi2Ol; // mixer to audio output left
  AudioConnection m_cMi2Or; // mixer to audio output right
  AudioConnection m_cMi2Ca; // microphone to recorder
  
  AudioControlSGTL5000 m_audioShield;

  int m_input;
  int m_sampleRate;                 // sample rate in Hz  
  int m_oscFreq = 45000;            // start heterodyne detecting at this frequency
  float m_freq_oscillator = 50000;  // oscillator frequency calculated for m_sine (depending on SR)
  stAudioSettings m_old;            // old settings
  float m_recThresh;                // recording thresh hold
  cTimer m_timeout;
  int m_oldCassMode = -1;
  
 public:
  cAudio();
  void init();
  void volume(float vol) { m_audioShield.volume(vol); }
  void setSampleRate(enSampleRate sr);
  void setPreAmpType(enPreAmp type);
  void setPreAmpGain(enGain gain);
  void setup();
  void updateCassMode();
  void checkAutoRecording(cMenue& menue);
  void operateRecorder();

 private:
  void setOscFrequency(float freq);
  bool isSetupNeeded();
  void setAudioConnections(int i) {}

};
#endif   //#ifndef _CAUDIO_H

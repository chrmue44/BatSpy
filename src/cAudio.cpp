/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "globals.h"
//#define DEBUG_LEVEL 4
#include "debug.h"
#include "pnllive.h"
#include "cutils.h"

#if defined ARDUINO_TEENSY41 || defined ARDUINO_TEENSY40
#include <utility/imxrt_hw.h>
#endif

// SRtext and position for the FFT spectrum display scale
const stSrDesc SR[] =
    { //SR_CODE   SR_FREQ, MUL_Fs, DIV_Fs, FFT_N
//        {SR_17K,   17640,  70, 2800, SR_17K, 1},
        {SR_19K,   19200,  70, 2573, SR_19K, 1},
//        {SR_23K,   23400,  70, 2111, SR_23K, 1},
//        {SR_28K,   28100,  98, 2461, SR_28K, 1},
        {SR_32K,   32000, 183, 4021, SR_32K, 1},
//        {SR_35K,   35300, 170, 3399, SR_35K, 1},
        {SR_38K,   38400, 170, 3123, SR_38K, 1},
        {SR_44K,   44100, 196, 3125, SR_44K, 1}, //F: 43940.0 1 16
        {SR_48K,   48000, 128, 1875, SR_48K, 1}, //F: 47940.0 3 44
//        {SR_88K,   88200, 107,  853, SR_88K, 2},  //F: 87910.0 1  8
//        {SR_96K,   96000, 219, 1604, SR_96K, 2}, //F: 95880.0 3 22
//        {SR_176K, 176400, 1,      4, SR_17K, 3},
        {SR_192K, 192000, 219,  802, SR_19K, 3}, //F: 191790.0 3 11
 //       {SR_234K, 234000, 1,      3, SR_23K, 4},
//        {SR_281K, 281000, 2,      5, SR_28K, 5},
        {SR_312K, 312800, 4,      9, SR_32K, 5},
//        {SR_352K, 352800, 1,      2, SR_35K, 6},
        {SR_384K, 383500, 6,     11, SR_38K, 7},
        {SR_480K, 480000, 6,      9, SR_48K, 7}
      };

int32_t cAudio::getSampleRateHz(enSampleRate sr)
{
  uint32_t idx = sr;
  if(idx >= sizeof (SR) / sizeof (SR[0]))
    idx = 0;
  return SR[idx].osc_frequency;
}

cAudio::cAudio() : 
m_audioIn(),
m_audioOut(),
m_cass(),
m_peak(),
m_cMi2Mx(m_audioIn, 0, m_mixer, MIX_CHAN_MIC),
m_cCa2Mx(m_cass.getPlayer(), 0, m_mixer, MIX_CHAN_PLAY),
m_cMx2Mu(m_mixer, 0, m_mult1, 0),
m_cSi2Mu(m_sineHet, 0, m_mult1, 1),
m_cMi2Fi(m_audioIn, 0, m_trigFilter, 0),
m_cMi2Rf(m_audioIn, 0, m_recFilter, 0),
m_cRf2Pk(m_recFilter, m_peak),
m_cMu2Ol(m_mult1, 0, m_audioOut, 0),
m_cMu2Or(m_mult1, 0, m_audioOut, 1),
//m_cMi2De(m_audioIn, 0, m_delay, 0),
m_cMi2De(m_recFilter, 0, m_delay, 0),
m_cDe2Ca(m_delay, 7, m_cass.getRecorder(), 0),
m_cMi2Hp(m_audioIn, m_filtDisp),
m_cHp2Ft(m_filtDisp, m_fft),
m_trigger(m_fftInfo, m_peak)
{
  DPRINTLN4("Audio connections initialized");
}


void cAudio::init()
{
  #ifdef ARDUINO_TEENSY40
  //nothing to do here
  #endif
  #ifdef ARDUINO_TEENSY41
  // set pre amp to the lowest posible amplification
  digitalWrite(pinAmp0, 1);
  digitalWrite(pinAmp1, 1);
  digitalWrite(pinAmp2, 1);
  digitalWrite(pinAmp3, 0);
#endif
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  //AudioMemory(600);
  audio_block_t data[600];
  AudioStream::initialize_memory(data, 600);
  m_old.oscFrequency = 999;
  m_filtDisp.setHighpass(0, 50);
}

void cAudio::enable(bool on)
{
  AudioNoInterrupts();
  if(!on)
  {
    m_cMi2Mx.disconnect(); // mic to mixer
    m_cCa2Mx.disconnect(); // player to mixer
    m_cMx2Mu.disconnect(); // mixer to multiplier
    m_cSi2Mu.disconnect(); // sine to multiplier
    m_cMi2Fi.disconnect(); // microphone to trigger filter
    m_cMi2Rf.disconnect(); // microphone to recording filter
    m_cRf2Pk.disconnect(); // filter to peak detector
    m_cMu2Ol.disconnect(); // multiplier to audio output left
    m_cMu2Or.disconnect(); // multiplier to audio output right
    m_cMi2De.disconnect(); // microphone to delay
    m_cDe2Ca.disconnect(); // delay to recorder
    m_cMi2Hp.disconnect(); // microphone to high pass
    m_cHp2Ft.disconnect(); // high pass to fft
  }
  else
  {
    m_cMi2Mx.connect(); // mic to mixer
    m_cCa2Mx.connect(); // player to mixer
    m_cMx2Mu.connect(); // mixer to multiplier
    m_cSi2Mu.connect(); // sine to multiplier
    m_cMi2Fi.connect(); // microphone to trigger filter
    m_cMi2Rf.connect(); // microphone to recording filter
    m_cRf2Pk.connect(); // filter to peak detector
    m_cMu2Ol.connect(); // multiplier to audio output left
    m_cMu2Or.connect(); // multiplier to audio output right
    m_cMi2De.connect(); // microphone to delay
    m_cDe2Ca.connect(); // delay to recorder
    m_cMi2Hp.connect(); // microphone to high pass
    m_cHp2Ft.connect(); // high pass to fft
  }
  AudioInterrupts();
}

void cAudio::setSampleRate(enSampleRate sr)
{
  // PLL between 27*24 = 648MHz und 54*24=1296MHz
  if ((int)sr >= sizeof(SR) / sizeof(SR[0]))
    sr = enSampleRate::SR_384K;
  if (m_old.sampleRate != SR[sr].osc_frequency)
  {
    m_sampleRate = SR[sr].osc_frequency;
    m_sampleRate /= 2;
    int n1 = 4; //SAI prescaler 4 => (n1*n2) = multiple of 4
    int n2 = 1 + (24000000 * 27) / (m_sampleRate * 256 * n1);
    double C = ((double)m_sampleRate * 256 * n1 * n2) / 24000000;
    int c0 = C;
    int c2 = 10000;
    int c1 = C * c2 - (c0 * c2);
    set_audioClock(c0, c1, c2, true);
    m_sampleRate *= 2;
#ifndef SIMU_DISPLAY
    // clock for input
    CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI1_CLK_PRED_MASK | CCM_CS1CDR_SAI1_CLK_PODF_MASK))
       | CCM_CS1CDR_SAI1_CLK_PRED(n1-1) // &0x07
       | CCM_CS1CDR_SAI1_CLK_PODF(n2-1); // &0x3f 
    // clock for output
  	CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI3_CLK_PRED_MASK | CCM_CS1CDR_SAI3_CLK_PODF_MASK))
		   | CCM_CS1CDR_SAI3_CLK_PRED(n1-1)
		   | CCM_CS1CDR_SAI3_CLK_PODF(n2-1);      
#endif
    DPRINTF1("factors c0:%i  c1:%i   c2:%i\n", c0, c1, c2);
    DPRINTF1("setSampleRate(%d)\n", m_sampleRate);
    m_cass.setSamplingRate(m_sampleRate);
    m_old.sampleRate = SR[sr].osc_frequency;
    m_old.parSampleR = sr;
  }
}


#ifdef ARDUINO_TEENSY40
void cAudio::setPreAmpGain(enGainRevC gain)
{
  switch (gain)
  {
    case enGainRevC::GAINC_48DB:
      digWrite(SPIN_AMP0, 1);
      break;
    case enGainRevC::GAINC_58DB:
      digWrite(SPIN_AMP0, 0);
      break;
  }
}
#endif
#ifdef ARDUINO_TEENSY41
void cAudio::setPreAmpGain(enGain gain)
{
  switch (gain)
  {
  case enGain::GAIN_27DB:
    digitalWrite(pinAmp0, 0);
    digitalWrite(pinAmp1, 1);
    digitalWrite(pinAmp2, 0);
    break;
  case enGain::GAIN_36DB:
    digitalWrite(pinAmp0, 0);
    digitalWrite(pinAmp1, 0);
    digitalWrite(pinAmp2, 1);
    break;
  case enGain::GAIN_45DB:
    digitalWrite(pinAmp0, 0);
    digitalWrite(pinAmp1, 0);
    digitalWrite(pinAmp2, 1);
    break;
  case enGain::GAIN_54DB:
    digitalWrite(pinAmp0, 0);
    digitalWrite(pinAmp1, 0);
    digitalWrite(pinAmp2, 0);
    break;
  }
}


void cAudio::setPreAmpGain(enGainRevB gain)
{
  switch (gain)
  {
  case enGainRevB::GAIN_30DB:
    digitalWrite(pinAmp0, 1);
    digitalWrite(pinAmp1, 1);
    digitalWrite(pinAmp2, 0);
    digitalWrite(pinAmp3, 1);
    break;
  case enGainRevB::GAIN_33DB:
    digitalWrite(pinAmp0, 0);
    digitalWrite(pinAmp1, 1);
    digitalWrite(pinAmp2, 1);
    digitalWrite(pinAmp3, 0);
    break;
  case enGainRevB::GAIN_40DB:
    digitalWrite(pinAmp0, 1);
    digitalWrite(pinAmp1, 0);
    digitalWrite(pinAmp2, 0);
    digitalWrite(pinAmp3, 0);
    break;
  case enGainRevB::GAIN_48DB:
    digitalWrite(pinAmp0, 0);
    digitalWrite(pinAmp1, 0);
    digitalWrite(pinAmp2, 0);
    digitalWrite(pinAmp3, 1);
    break;
  case enGainRevB::GAIN_53DB:
    digitalWrite(pinAmp0, 0);
    digitalWrite(pinAmp1, 0);
    digitalWrite(pinAmp2, 1);
    digitalWrite(pinAmp3, 0);
    break;
  case enGainRevB::GAIN_58DB:
    digitalWrite(pinAmp0, 0);
    digitalWrite(pinAmp1, 0);
    digitalWrite(pinAmp2, 0);
    digitalWrite(pinAmp3, 0);
    break;
  }
}
#endif


void cAudio::setTrigFilter(float freq, enFiltType type)
{
  float maxFreq = m_sampleRate/2000;
  float f = freq * AUDIO_SAMPLE_RATE_EXACT / m_sampleRate;
  devPars.trigFiltFreq.init(5,maxFreq, 1, 0);
  switch (type)
  {
    case enFiltType::HIGHPASS:
      m_trigFilter.setHighpass(0, f, 0.7f);
      break;
    case enFiltType::LOWPASS:
      m_trigFilter.setLowpass(0, f, 0.7f);
      break;
    case enFiltType::BANDPASS:
      m_trigFilter.setBandpass(0, f, 0.7f);
      break;  
    default:
      break;
  }
}

void cAudio::setRecFilter(float freq, enFiltType type)
{
  float maxFreq = m_sampleRate/2000;
  float f = freq * AUDIO_SAMPLE_RATE_EXACT / m_sampleRate;
  devPars.recFiltFreq.init(0,maxFreq, 1, 0);
  switch (type)
  {
    case enFiltType::HIGHPASS:
      m_recFilter.setHighpass(0, f, 0.7f);
      break;
    case enFiltType::LOWPASS:
      m_recFilter.setLowpass(0, f, 0.7f);
      break;
    case enFiltType::BANDPASS:
      m_recFilter.setBandpass(0, f, 0.7f);
      break;  
    default:
      break;
  }
}

bool cAudio::isSetupNeeded()
{
  bool retVal = false;
  if (m_old.sampleRate != m_sampleRate)
  {
    DPRINTF2("new sample rate %i, old %i\n", m_sampleRate, m_old.sampleRate);
    retVal = true;
  }
  if (m_old.parSampleR != devPars.sampleRate.get())
  {
    DPRINTF2("new sample rate %i, old %i\n", devPars.sampleRate.get(), m_old.parSampleR);
    retVal = true;
  }
  if (m_old.oscFrequency != devPars.mixFreq.get())
  {
    DPRINTF2("new mix frequency %f, old %f\n", devPars.mixFreq.get() * 1000, m_old.oscFrequency);
    retVal = true;
  }
  if (m_old.opMode != devStatus.opMode.get())
  {
    DPRINTF2("new operation mode %i, old: %i\n", devStatus.opMode.get(), m_old.opMode);
    retVal = true;
  }
  if((m_old.volume != devPars.volume.get()))
  {
    DPRINTF2("new volume %f, old: %f\n", devPars.volume.get(), m_old.volume);
    retVal = true;
  }
  return retVal;
}


void cAudio::setup()
{
  #ifdef ARDUINO_TEENSY41
  if(hasAmpRevB())  
    setPreAmpGain((enGainRevB)devPars.preAmpGain.get());
  else
  {
    setPreAmpType((enPreAmp)devPars.preAmpType.get());
    setPreAmpGain((enGain)devPars.preAmpGain.get());
  }
  #endif
  setAnalogPower(true);
  #ifdef ARDUINO_TEENSY40
  setPreAmpGain((enGainRevC)devPars.preAmpGain.get());
  #endif
  if (isSetupNeeded())
  {
    float vol = pow(10, (devPars.volume.get() / 10));
    if (vol > 32)
      vol = 32;
    m_old.volume = devPars.volume.get();
    AudioNoInterrupts();
    delay(100);
    setSampleRate((enSampleRate)devPars.sampleRate.get());
    bool mic = false;
    float freq = 0;
    switch (devStatus.opMode.get()) {
      case enOpMode::HEAR_DIRECT:
        mic = true;
        break;

      case enOpMode::HEAR_HET:
        mic = true;
        freq = devPars.mixFreq.get();
        break;

      case enOpMode::PLAY_STRETCHED:
        setSampleRate((enSampleRate)SR[devPars.sampleRate.get()].stretched);
        break;

      case enOpMode::PLAY_DIRECT:
        break;

      case enOpMode::PLAY_HET:
        freq = devPars.mixFreq.get();
        break;
    }
    m_mixer.gain(MIX_CHAN_MIC, mic ? vol : 0);
    m_mixer.gain(MIX_CHAN_PLAY, mic ? 0 : vol);
    m_mixer.gain(MIX_CHAN2, 0);
    m_mixer.gain(MIX_CHAN3, 0);

    setMixOscFrequency(freq * 1000.0);
    m_old.opMode = (enOpMode)devStatus.opMode.get();
    m_trigger.setThreshold(pow(10, (devPars.recThreshhold.get() / 10.0)));
    m_trigger.setMinEventLength(devPars.minEventLen.get(), m_sampleRate);
    m_delay.delay(7, devPars.preTrigger.get() *  m_sampleRate / 44100);
    setTrigFilter(devPars.trigFiltFreq.get() * 1000.0, (enFiltType)devPars.trigFiltType.get());
    setRecFilter(devPars.recFiltFreq.get() * 1000.0, (enFiltType)devPars.recFiltType.get());
    devStatus.liveMsPerDiv.clear();
    for(int i = 0; i < 4; i++)
      addLiveMsPerDiv(i, m_sampleRate);
    delay(100);
    AudioInterrupts();
    delay(20);

    DPRINTLN4("\n[***** AUDIO SETTINGS ******]\n");
    DPRINTF4("       op mode: %i %s\n", devStatus.opMode.get(), devStatus.opMode.getActText());
    DPRINTF4("   sample rate: %s  index: %i   value: %i Hz\n", devPars.sampleRate.getActText(), devPars.sampleRate.get(), m_sampleRate);
    DPRINTF4("      mix freq: %.1f kHz; real osc freq: %.1f Hz\n", freq, m_freq_oscillator);
    DPRINTF4("rec threshhold: %.1f dB\n", devPars.recThreshhold.get());
    DPRINTF4("  trig. filter: %.1f kHz\n", devPars.trigFiltFreq.get())
    DPRINTF4("   pre trigger: %.1f ms\n", devPars.preTrigger.get());
    DPRINTF4("         mixer: %s\n", mic ? "mic" : "play");
    DPRINTF4("        volume: %.4f\n", vol);
  }
  else
    DPRINTLN2("no setup needed");
  DPRINTF4("  pre amp gain: %s\n", devPars.preAmpGain.getActText());
}


void cAudio::updateCassMode()
{
  switch (devStatus.playStatus.get())
  {
  case enPlayStatus::ST_STOP:
  case enPlayStatus::TIMEOUT:
    setAudioConnections(1);
    m_cass.stop();
    break;
  case enPlayStatus::ST_PLAY:
    if (m_cass.getMode() != enCassMode::PLAY)
    {
      m_cass.setFileName(devPars.fileName.get());
      m_cass.startPlay();
      delay(5);
    }
    break;
  case enPlayStatus::ST_REC:
    if (m_cass.getMode() != enCassMode::REC)
    {
      setAudioConnections(0);
      startRecording();
      DPRINTF4("updateCassMode start recording: %i\n", (int)devStatus.recCount.get());
      delay(5);
    }
    break;
  }
}


void cAudio::setMixOscFrequency(float freq)
{
  m_sineHet.amplitude(1.0);
  float maxF = m_sampleRate / 1000.0 / 2;
  devPars.mixFreq.init(10, maxF, 1, 0);
  if (devPars.mixFreq.get() > devPars.mixFreq.getMax())
    devPars.mixFreq.set(devPars.mixFreq.getMax());
  m_freq_oscillator = freq * AUDIO_SAMPLE_RATE_EXACT / m_sampleRate;
  m_sineHet.frequency(m_freq_oscillator);
  if (m_freq_oscillator < 0.1)
    m_sineHet.phase(90);
  m_old.oscFrequency = devPars.mixFreq.get();
}

bool cAudio::isRecordingActive()
{
  bool retVal = false;
  if(devPars.recAuto.get() == enRecAuto::OFF)
    retVal = false;
  else if(devPars.recAuto.get() == enRecAuto::ON)
    retVal = true;
  else
  {
    if(devPars.startH.get() > devPars.stopH.get())
    {
      retVal = (devStatus.time.getMinOfDay() >= (devPars.startH.get() * 60 + devPars.startMin.get())) ||
               (devStatus.time.getMinOfDay() <= (devPars.stopH.get() * 60 + devPars.stopMin.get()));
    }
    else
    {
      retVal = (devStatus.time.getMinOfDay() >= (devPars.startH.get() * 60 + devPars.startMin.get())) && 
                 (devStatus.time.getMinOfDay() <= (devPars.stopH.get() * 60 + devPars.stopMin.get()));
    }
  }
  return retVal;
}


void cAudio::checkAutoRecording(bool recActive)
{
  if((devStatus.playStatus.get() == enPlayStatus::ST_STOP) && (m_cass.getMode() == enCassMode::STOP))
  {
    if (menue.keyPauseLongEnough(300) && (devPars.recAuto.get() != enRecAuto::OFF))
    {
      if (!recActive && m_prj.getIsOpen())
        m_prj.closePrjFile();
      if (recActive && m_trigger.getRecTrigger())
      {
        DPRINTLN4("checkAutorecording: start recording");
        startRecording();
        delay(5);
      }
    }
    else
      m_prj.closePrjFile();
  }
}

/*
void cAudio::checkAutoRecording(cMenue &menue, cRtc& rtc)
{
  if(m_cass.getMode() != enCassMode::REC)
  {
    if (menue.keyPauseLongEnough(300) && (devPars.recAuto.get() != enRecAuto::OFF))
    {
      if ((devStatus.playStatus.get() == enPlayStatus::ST_STOP))
      {
        devStatus.time.set(rtc.getTime());
        devStatus.time.update(false);
        bool startRec = false;
        if (devPars.recAuto.get() == enRecAuto::ON)
          startRec = true;
        else
        {
          if(devPars.startH.get() > devPars.stopH.get())
          {
             startRec = (devStatus.time.getMinOfDay() >= (devPars.startH.get() * 60 + devPars.startMin.get())) ||
                       (devStatus.time.getMinOfDay() <= (devPars.stopH.get() * 60 + devPars.stopMin.get()));
          }
          else
          {
             startRec = (devStatus.time.getMinOfDay() >= (devPars.startH.get() * 60 + devPars.startMin.get())) && 
                     (devStatus.time.getMinOfDay() <= (devPars.stopH.get() * 60 + devPars.stopMin.get()));
          }
        }

        if (!startRec && m_prj.getIsOpen())
          m_prj.closePrjFile();

        if (m_trigger.getRecTrigger() && startRec)
        {
          startRecording();
          delay(5);
        }
      }
    }
    else
      m_prj.closePrjFile();
  }
}
*/

void cAudio::openProject()
{
  char buf[2 * PAR_STR_LEN + 3];
  cUtils::replaceInternalCodingWithUTF8(devStatus.notes1.getActText(), buf, sizeof(buf));
  strncat(buf, "\n", 2);
  size_t len = strlen(buf);
  if (len < sizeof(buf))
    cUtils::replaceInternalCodingWithUTF8(devStatus.notes2.getActText(), buf + len, sizeof(buf) - len);
  m_prj.openPrjFile(buf);
  devStatus.recCount.set(m_prj.getRecCount());
}


void cAudio::operate(bool liveFft)
{
  m_cass.operate();
  bool fftAvailable = m_fft.available();  
  if(fftAvailable)
  {
    int idx = m_fft.find_max_amp();
    m_fftInfo.lastMaxAmpl = m_fft.output[idx];
    m_fftInfo.lastMaxFreq = (float)m_sampleRate * idx / getFftOutputSize() / 2.0; 
    int32_t avg = 0;
    int32_t bw = 0;
    int i = 0;
    int32_t avgPeak = 0;

    // calculate average 
    for(i = 0; i < 512; i++)
      avg += m_fft.output[i];
    
    // calculate bandwidth around maximum intensity
    i = idx;
    while((m_fft.output[i] > FREQ_THRESHOLD) && (i < 512))
    {
      bw++;
      i++;
      avgPeak += m_fft.output[i];
    }
    i = idx;
    while((m_fft.output[i] > FREQ_THRESHOLD) && (i >= 0))
    {
      bw++;
      i--;
      avgPeak += m_fft.output[i];
    }
    
    if(bw == 0)
      bw++;
    m_fftInfo.lastAvgPeak = (float)avgPeak / (float)bw;
    m_fftInfo.lastAvg =  (float)(avg - avgPeak) / 512.0;
    m_fftInfo.bw = (float)(bw * m_sampleRate) / 1024.0;
    m_trigger.checkTrigger();
  }

  if(liveFft && fftAvailable && !m_haltLiveFft)
    calcLiveFft();
  
  // handle recording
  if (m_oldCassMode != m_cass.getMode())
  {
    if (m_cass.getMode() == enCassMode::STOP)
    {
      DPRINTF4("cAudio::operate: cass mode: %i, playStatus: %i\n", m_cass.getMode(), devStatus.playStatus.get());
      switch (devStatus.playStatus.get())
      {
      case enPlayStatus::ST_PLAY:
        devStatus.playStatus.set(enPlayStatus::ST_STOP);
        break;
      case enPlayStatus::ST_REC:
        m_timeout.setAlarm(devPars.deadTime.get());
        statusDisplay.setRecRunning(false);
        devStatus.playStatus.set(enPlayStatus::TIMEOUT);
        m_prj.writeInfoFile(m_trigger.lastPeakVal(), m_cass.getSampleCnt());
        m_trigger.logTrigInfo(m_prj.getWavFileName());
        DPRINTLN4("start timeout");
        break;
      case enPlayStatus::ST_STOP:
        if(m_oldCassMode == enCassMode::REC)
        {
          statusDisplay.setRecRunning(false);
          m_trigger.releaseRecTrigger();
          m_prj.writeInfoFile(m_trigger.lastPeakVal(), m_cass.getSampleCnt());
          m_trigger.logTrigInfo(m_prj.getWavFileName());
          DPRINTLN4("cAudio::operate: recording stopped");
        }
        break;

      default:
        break;
      }
    }
    else
    {
      devStatus.playStatus.set(m_cass.getMode() - 1);
    }
    m_oldCassMode = m_cass.getMode();
  }

  if (devStatus.playStatus.get() == enPlayStatus::TIMEOUT)
  {
    if (m_timeout.isAlarm())
    {
      m_timeout.stop();
      devStatus.playStatus.set(enPlayStatus::ST_STOP);
      m_trigger.releaseLiveTrigger();
      m_trigger.releaseRecTrigger();
      DPRINTF4("timeout over, timeout %f,  timer %f\n", devPars.deadTime.get(), m_timeout.runTime());
    }
  }
}


void cAudio::stopRecording()
{
  DPRINTLN4("stop recording");
  m_cass.stop();
  devStatus.playStatus.set(enPlayStatus::ST_STOP);
}


void cAudio::calcLiveFft()
{
  if (m_fftInfo.sweepDelayCnt >= devStatus.liveMsPerDiv.get())
  {
    m_fftInfo.sweepDelayCnt = 0;
    if (m_fftInfo.liveCnt == 0)
    {
      if (m_trigger.getLiveTrigger())
        m_fftInfo.liveCnt = LIVE_CNT_EXTEND;
    }
    else
    {
      if (m_fftInfo.lastMaxAmpl < 100)
        m_fftInfo.liveCnt--;
      DPRINTF4("count %i\n", m_fftInfo.liveCnt);
      if (m_fftInfo.liveCnt == 0)
        m_trigger.releaseLiveTrigger();
    }

    if (m_fftInfo.liveCnt > 0)
    {
      cParGraph* graph = getLiveFft();
      if(hasDisplay() == enDisplayType::TFT_320)
        graph->updateLiveData(m_fft.output, devPars.liveAmplitude.get());
      else
        updateExtFftBuf();
    }
  }
  else
    m_fftInfo.sweepDelayCnt++;
}


void cAudio::updateExtFftBuf()
{
  int size = sizeof(m_fft.output)/sizeof(m_fft.output[0]) / 4;
  for(int i = 0; i < size; i++)
  {
    int j = 4 * i;
    uint32_t  sum = m_fft.output[j] + m_fft.output[j+1] + m_fft.output[j+2] + m_fft.output[j+3];
    sum >>= 10;
    *m_extFftBufPtr =(unsigned char)(sum & 0xFF);
    m_extFftBufPtr++;
    if(m_extFftBufPtr >= &m_extFftBuf[0] + sizeof(m_extFftBuf))
      m_extFftBufPtr = &m_extFftBuf[0];
  }
}


void cAudio::sendFftBuffer(int delayTime, int part)
{
  unsigned char* p = m_extFftBufPtr;
  int cnt = 0;
  int chunkSize = 128;
  int chunkCnt = (EXTFFT_H * EXTFFT_W) / chunkSize / 2;

  if(part == 0)
    m_haltLiveFft = true;
  else
  {
    size_t toEnd = m_extFftBuf + sizeof(m_extFftBuf) - m_extFftBufPtr;
    size_t offs = chunkCnt * chunkSize / 2; 
    if(offs < toEnd)
      p += offs;
    else
      p = m_extFftBuf + (offs - toEnd);
  }

  for(int i = 0; i < chunkCnt; i++)
  {
    if(p >= &m_extFftBuf[0] + sizeof(m_extFftBuf))
      p = &m_extFftBuf[0];
    Serial.write((const char*)p, chunkSize);
    p += chunkSize;
    cnt++;
    if(cnt >= 8)
    {
      cnt = 0;
      delay(delayTime);
    }
  }
  if(part == 1)
    m_haltLiveFft = false;
}


void cAudio::startRecording()
{
  if (!m_prj.getIsOpen())
    openProject();
  statusDisplay.setRecRunning(true);
  m_prj.createElekonFileName();
  m_prj.addFile();
  devStatus.recCount.set(m_prj.getRecCount());
  DPRINTF4("start recording: %i\n", (int)devStatus.recCount.get());
  m_cass.startRec(m_prj.getWavFileName(), devPars.recTime.get());
  devStatus.playStatus.set(enPlayStatus::ST_REC);
}


void cAudio::addLiveMsPerDiv(int waitTick, int sampleRate)
{
  char buf[LIST_ITEM_LEN];
  int ms = getLiveMsPerDiv(waitTick, sampleRate);
  snprintf(buf, sizeof(buf), "%i ms/div", ms);
  devStatus.liveMsPerDiv.addItem(buf);
}


int cAudio::getLiveMsPerDiv(int waitTick, int sampleRate)
{
  return (int)(1000.0 / sampleRate * getFftOutputSize() / 2 * DISP_WIDTH_TFT / X_TICK_CNT * (1 + waitTick));
}
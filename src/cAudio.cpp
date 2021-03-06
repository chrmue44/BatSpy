/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "globals.h"
//#define DEBUG_LEVEL 2
#include "debug.h"
#include "pnllive.h"

#ifdef ARDUINO_TEENSY41
#include <utility/imxrt_hw.h>
#endif

// SRtext and position for the FFT spectrum display scale
const stSrDesc SR[] =
    { //SR_CODE   SR_FREQ, MUL_Fs, DIV_Fs, FFT_N
        {SR_17K,   17640,  70, 2800, SR_17K, 1},
        {SR_19K,   19200,  70, 2573, SR_19K, 1},
        {SR_23K,   23400,  70, 2111, SR_23K, 1},
        {SR_28K,   28100,  98, 2461, SR_28K, 1},
        {SR_32K,   32000, 183, 4021, SR_32K, 1},
        {SR_35K,   35300, 170, 3399, SR_35K, 1},
        {SR_38K,   38400, 170, 3123, SR_38K, 1},
        {SR_44K,   44100, 196, 3125, SR_44K, 1}, //F: 43940.0 1 16
        {SR_48K,   48000, 128, 1875, SR_48K, 1}, //F: 47940.0 3 44
        {SR_88K,   88200, 107,  853, SR_88K, 2},  //F: 87910.0 1  8
        {SR_96K,   96000, 219, 1604, SR_96K, 2}, //F: 95880.0 3 22
        {SR_176K, 176400, 1,      4, SR_17K, 3},
        {SR_192K, 192000, 219,  802, SR_19K, 3}, //F: 191790.0 3 11
        {SR_234K, 234000, 1,      3, SR_23K, 4},
        {SR_281K, 281000, 2,      5, SR_28K, 5},
        {SR_312K, 312800, 4,      9, SR_32K, 5},
        {SR_352K, 352800, 1,      2, SR_35K, 6},
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
m_peak(),
m_cass(m_peak),
m_cMi2Mx(m_audioIn, 0, m_mixer, MIX_CHAN_MIC),
m_cCa2Mx(m_cass.getPlayer(), 0, m_mixer, MIX_CHAN_PLAY),
m_cMx2Mu(m_mixer, 0, m_mult1, 0),
m_cSi2Mu(m_sineHet, 0, m_mult1, 1),
m_cMi2Fi(m_audioIn, 0, m_filter, 0),
m_cFi2Pk(m_filter, m_peak),
m_cMu2Ol(m_mult1, 0, m_audioOut, 0),
m_cMu2Or(m_mult1, 0, m_audioOut, 1),
m_cMi2De(m_audioIn, 0, m_delay, 0),
m_cDe2Ca(m_delay, 7, m_cass.getRecorder(), 0),
m_cMi2Hp(m_audioIn, m_filtDisp),
m_cHp2Ft(m_filtDisp, m_fft)
{
  DPRINTLN4("Audio connections initialized");
}

void cAudio::init()
{
  // set pre amp to the lowest posible amplification
  digitalWrite(PIN_AMP_0, 1);
  digitalWrite(PIN_AMP_1, 1);
  digitalWrite(PIN_AMP_2, 1);
  digitalWrite(PIN_AMP_3, 0);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(600);

  m_old.oscFrequency = 999;
  m_filtDisp.setHighpass(0, 50);
}

#ifdef ARDUINO_TEENSY41

void cAudio::setSampleRate(enSampleRate sr)
{
  // PLL between 27*24 = 648MHz und 54*24=1296MHz
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
    // clock for input
    CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI1_CLK_PRED_MASK | CCM_CS1CDR_SAI1_CLK_PODF_MASK))
       | CCM_CS1CDR_SAI1_CLK_PRED(n1-1) // &0x07
       | CCM_CS1CDR_SAI1_CLK_PODF(n2-1); // &0x3f 
    // clock for output
  	CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI3_CLK_PRED_MASK | CCM_CS1CDR_SAI3_CLK_PODF_MASK))
		   | CCM_CS1CDR_SAI3_CLK_PRED(n1-1)
		   | CCM_CS1CDR_SAI3_CLK_PODF(n2-1);      
    DPRINTF1("factors c0:%i  c1:%i   c2:%i\n", c0, c1, c2);
    DPRINTF1("setSampleRate(%d)\n", m_sampleRate);
    m_cass.setSamplingRate(m_sampleRate);
    m_old.sampleRate = SR[sr].osc_frequency;
    m_old.parSampleR = sr;
  }
}

#else  //#ifdef ARDUINO_TEENSY41

void cAudio::setSampleRate(enSampleRate sr)
{
  if (m_old.sampleRate != sr)
  {
    m_sampleRate = SR[sr].osc_frequency;
    //  AudioNoInterrupts();
    //set FS directly from the SR
    while (I2S0_MCR & I2S_MCR_DUF)
      ;
    I2S0_MDR = I2S_MDR_FRACT((SR[sr].MUL_Fs - 1)) | I2S_MDR_DIVIDE((SR[sr].DIV_Fs - 1));
    m_cass.setSamplingRate(m_sampleRate);
    delay(200); // this delay seems to be very essential !
    //  AudioInterrupts();
    delay(20);
    m_old.sampleRate = sr;
  }
}
#endif //#ifdef ARDUINO_TEENSY41


void cAudio::setPreAmpType(enPreAmp type)
{
  switch (type)
  {
  case enPreAmp::LINEAR:
    digitalWrite(PIN_AMP_3, 1);
    break;

  case enPreAmp:: HIGH_PASS:
    digitalWrite(PIN_AMP_3, 0);
    break;
  }
}

void cAudio::setPreAmpGain(enGain gain)
{
  switch (gain)
  {
  case enGain::GAIN_27DB:
    digitalWrite(PIN_AMP_0, 0);
    digitalWrite(PIN_AMP_1, 1);
    digitalWrite(PIN_AMP_2, 0);
    break;
  case enGain::GAIN_36DB:
    digitalWrite(PIN_AMP_0, 0);
    digitalWrite(PIN_AMP_1, 0);
    digitalWrite(PIN_AMP_2, 1);
    break;
  case enGain::GAIN_45DB:
    digitalWrite(PIN_AMP_0, 0);
    digitalWrite(PIN_AMP_1, 0);
    digitalWrite(PIN_AMP_2, 1);
    break;
  case enGain::GAIN_54DB:
    digitalWrite(PIN_AMP_0, 0);
    digitalWrite(PIN_AMP_1, 0);
    digitalWrite(PIN_AMP_2, 0);
    break;
  }
}

void cAudio::setTrigFilter(float freq, enFiltType type)
{
  float maxFreq = m_sampleRate/2;
  float f = freq * AUDIO_SAMPLE_RATE_EXACT / m_sampleRate;
  devPars.filtFreq.init(5,maxFreq, 1, 0);
  switch (type)
  {
    case enFiltType::HIGHPASS:
      m_filter.setHighpass(0, f, 0.7);
      break;
    case enFiltType::LOWPASS:
      m_filter.setLowpass(0, f, 0.7);
      break;
    case enFiltType::BANDPASS:
      m_filter.setBandpass(0, f, 0.7);
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
  setPreAmpType((enPreAmp)devPars.preAmpType.get());
  setPreAmpGain((enGain)devPars.preAmpGain.get());

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
    m_recThresh = pow(10, (devPars.recThreshhold.get() / 10.0));
    m_delay.delay(7, devPars.preTrigger.get() *  m_sampleRate / 44100);
    setTrigFilter(devPars.filtFreq.get() * 1000.0, (enFiltType)devPars.filtType.get());
    delay(100);
    AudioInterrupts();
    delay(20);

    DPRINTLN4("\n[***** AUDIO SETTINGS ******]\n");
    DPRINTF4("       op mode: %i %s\n", devStatus.opMode.get(), devStatus.opMode.getActText());
    DPRINTF4("   sample rate: %s  index: %i   value: %i Hz\n", devPars.sampleRate.getActText(), devPars.sampleRate.get(), m_sampleRate);
    DPRINTF4("      mix freq: %.1f kHz; real osc freq: %.1f Hz\n", freq, m_freq_oscillator);
    DPRINTF4("rec threshhold: %.1f dB\n", devPars.recThreshhold.get());
    DPRINTF4("  trig. filter: %.1f kHz\n", devPars.filtFreq.get())
    DPRINTF4("   pre trigger: %.1f ms\n", devPars.preTrigger.get());
    DPRINTF4("         mixer: %s\n", mic ? "mic" : "play");
    DPRINTF4("        volume: %.4f\n", vol);
  }
  else
    DPRINTLN2("no setup needed");
  DPRINTF4("  pre amp type: %s\n", devPars.preAmpType.getActText());
  DPRINTF4("  pre amp gain: %s\n", devPars.preAmpGain.getActText());
}

void cAudio::updateCassMode()
{
  switch (devStatus.playStatus.get())
  {
  case 0:
  case 3:
    setAudioConnections(1);
    m_cass.stop();
    break;
  case 1:
    if (m_cass.getMode() != enCassMode::PLAY)
    {
      m_cass.setFileName(devPars.fileName.get());
      m_cass.startPlay();
      delay(5);
    }
    break;
  case 2:
    if (m_cass.getMode() != enCassMode::REC)
    {
      setAudioConnections(0);
      startRec();
      devStatus.recCount.set(devStatus.recCount.get() + 1);
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

void cAudio::checkAutoRecording(cMenue &menue, cRtc& rtc)
{
  if(m_cass.getMode() != enCassMode::REC)
  {
    if (m_peak.available())
    {
      m_peakVal = m_peak.read();
      DPRINTF2("peak: %f   threshhold: %f\n", m_peakVal, m_recThresh);
    }
    if (menue.keyPauseLongEnough(300) && (devPars.recAuto.get() != enRecAuto::OFF))
    {
      if ((devStatus.playStatus.get() == enPlayStatus::ST_STOP))
      {
        devStatus.time.set(rtc.getTime());
        devStatus.time.update(false);
        bool startRecording = false;
        if (devPars.recAuto.get() == enRecAuto::ON)
          startRecording = true;
        else
        {
          if(devPars.startH.get() > devPars.stopH.get())
          {
             startRecording = (devStatus.time.getMinOfDay() >= (devPars.startH.get() * 60 + devPars.startMin.get())) ||
                       (devStatus.time.getMinOfDay() <= (devPars.stopH.get() * 60 + devPars.stopMin.get()));
          }
          else
          {
             startRecording = (devStatus.time.getMinOfDay() >= (devPars.startH.get() * 60 + devPars.startMin.get())) && 
                     (devStatus.time.getMinOfDay() <= (devPars.stopH.get() * 60 + devPars.stopMin.get()));
          }
        }

        if ((m_peakVal > m_recThresh) && startRecording)
        {
          if(startRecording && (devPars.projectType.get() == enProjType::ELEKON) && !m_prj.getIsOpen())
            m_prj.openPrjFile();
          devStatus.recCount.set(devStatus.recCount.get() + 1);
          startRec();
          devStatus.playStatus.set(enPlayStatus::ST_REC);
          delay(5);
        }
      }
    }
    else
      m_prj.closePrjFile();

  }
}

void cAudio::operate(bool liveFft)
{
  m_cass.operate();  
  if(liveFft)
    calcLiveFft();
  if (m_oldCassMode != m_cass.getMode())
  {
    if (m_cass.getMode() == enCassMode::STOP)
    {
      switch (devStatus.playStatus.get())
      {
      case enPlayStatus::ST_PLAY:
        devStatus.playStatus.set(enPlayStatus::ST_STOP);
        break;
      case enPlayStatus::ST_REC:
        m_timeout.start();
        devStatus.playStatus.set(enPlayStatus::TIMEOUT);
        m_prj.writeInfoFile(m_peakVal, m_cass.getSampleCnt(), cPrjoject::getFileFmt());
        DPRINTLN1("start timeout");
        break;
      case enPlayStatus::ST_STOP:
        if(m_oldCassMode == enCassMode::REC)
          m_prj.writeInfoFile(m_peakVal, m_cass.getSampleCnt(), cPrjoject::getFileFmt());
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
    if (m_timeout.runTime() > devPars.deafTime.get())
    {
      devStatus.playStatus.set(0);
      m_peak.read();
      DPRINTF1("timeout over, timeout %f,  timer %f\n", devPars.deafTime.get(), m_timeout.runTime());
    }
  }
}

void cAudio::calcLiveFft()
{
  if(m_fft.available() && millis() > (m_lastFft + devPars.sweepSpeed.get()))
  {
    m_lastFft = millis();
    cParGraph* graph = getLiveFft();
    graph->updateLiveData(m_fft.output, devPars.liveAmplitude.get());
  }
}

void cAudio::startRec()
{
  if(devPars.projectType.get() == enProjType::ELEKON)
  {
    m_prj.createElekonFileName();
    m_prj.addFile();
    m_cass.startRec(m_prj.getWavFileName(), devPars.recTime.get(), enRecFmt::WAV);
  }
  else
  {
    enRecFmt fmt = static_cast<enRecFmt>(devPars.recFmt.get());
    m_prj.createTimeFileName(fmt);
    m_cass.startRec(m_prj.getWavFileName(), devPars.recTime.get(), fmt);
  }
}

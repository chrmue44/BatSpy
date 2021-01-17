#include "cAudio.h"
#include "config.h"
#include "debug.h"
#include "cmenue.h"
#ifdef ARDUINO_TEENSY41
#include <utility/imxrt_hw.h>
#endif
//705600

// SRtext and position for the FFT spectrum display scale
const stSrDesc SR[] =
    { //SR_CODE   SR_FREQ, MUL_Fs, DIV_Fs, FFT_N
        {SR_17K, 17640, 70, 2800, SR_17K, 1},
        {SR_19K, 19200, 70, 2573, SR_19K, 1},
        {SR_23K, 23400, 70, 2111, SR_23K, 1},
        {SR_28K, 28100, 98, 2461, SR_28K, 1},
        {SR_32K, 32000, 183, 4021, SR_32K, 1},
        {SR_35K, 35300, 170, 3399, SR_35K, 1},
        {SR_38K, 38400, 170, 3123, SR_38K, 1},
        {SR_44K, 44100, 196, 3125, SR_44K, 1}, //F: 43940.0 1 16
        {SR_48K, 48000, 128, 1875, SR_48K, 1}, //F: 47940.0 3 44
        {SR_88K, 88200, 107, 853, SR_88K, 2},  //F: 87910.0 1  8
        {SR_96K, 96000, 219, 1604, SR_96K, 2}, //F: 95880.0 3 22
        {SR_176K, 176400, 1, 4, SR_17K, 3},
        {SR_192K, 192000, 219, 802, SR_19K, 3}, //F: 191790.0 3 11
        {SR_234K, 234000, 1, 3, SR_23K, 4},
        {SR_281K, 281000, 2, 5, SR_28K, 5},
        {SR_352K, 352800, 1, 2, SR_35K, 6},
        {SR_384K, 383500, 6, 11, SR_38K, 7}};

cAudio::cAudio() : m_cMi2Mx(m_audioIn, 0, m_mixer, MIX_CHAN_MIC),
                   m_cCa2Mx(m_cass.getPlayer(), 0, m_mixer, MIX_CHAN_PLAY),
                   m_cMx2Mu(m_mixer, 0, m_mult1, 0),
                   m_cSi2Mu(m_sineHet, 0, m_mult1, 1),
                   m_cMi2Fi(m_audioIn, 0, m_filter, 0),
                   m_cFi2Pk(m_filter, 0, m_peak, 0),
                   m_cMi2Ol(m_mixer, 0, m_audioOut, 0),
                   m_cMi2Or(m_mixer, 0, m_audioOut, 1),
                   m_cMi2Ca(m_audioIn, 0, m_cass.getRecorder(), 0),

                   m_input(AUDIO_INPUT_LINEIN)
{
}

void cAudio::init()
{
  pinMode(PIN_AMP_0, OUTPUT);
  pinMode(PIN_AMP_1, OUTPUT);
  pinMode(PIN_AMP_2, OUTPUT);
  pinMode(PIN_AMP_3, OUTPUT);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(100);
  // Enable the audio shield and set the output volume.
#ifdef AUDIO_SGTL5000
  m_audioShield.enable();
  m_audioShield.inputSelect(m_input);
  m_audioShield.lineInLevel(0);
  m_audioShield.volume(0.5);
  m_audioShield.audioPreProcessorEnable();
  // here are some settings for AVC that have a fairly obvious effect
  m_audioShield.autoVolumeControl(2, 1, 0, -5, 0.5, 0.5); // see comments starting line #699 of control_sgtl5000.cpp in ./libraries/audio/
#endif
  // AVC has its own enable/disable bit
  // you can use audioShield.autoVolumeEnable(0); to turn off AVC
  m_old.oscFrequency = 999;
}

#ifdef ARDUINO_TEENSY41

void cAudio::setSampleRate(enSampleRate sr)
{
  // PLL between 27*24 = 648MHz und 54*24=1296MHz
  if (m_old.sampleRate != SR[sr].osc_frequency)
  {
    m_sampleRate = SR[sr].osc_frequency;
    int n1 = 4; //SAI prescaler 4 => (n1*n2) = multiple of 4
    int n2 = 1 + (24000000 * 27) / (m_sampleRate * 256 * n1);
    double C = ((double)m_sampleRate * 256 * n1 * n2) / 24000000;
    int c0 = C;
    int c2 = 10000;
    int c1 = C * c2 - (c0 * c2);
    set_audioClock(c0, c1, c2, true);
    CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI1_CLK_PRED_MASK | CCM_CS1CDR_SAI1_CLK_PODF_MASK)) | CCM_CS1CDR_SAI1_CLK_PRED(n1 - 1) // &0x07
                 | CCM_CS1CDR_SAI1_CLK_PODF(n2 - 1);                                                                                // &0x3f
    m_old.sampleRate = m_sampleRate;
    DPRINTF1("factors c0:%i  c1:%i   c2:%i\n", c0, c1, c2);
    DPRINTF1("setSampleRate(%d)\n", m_sampleRate);
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

/*  if (!(m_playActive or m_recorderActive))
     {D_PRINTXY("DETECTOR SampleRate:",m_SR_real);
     }
  if (m_playActive)
  {
     D_PRINTXY("PLAYER SampleRate:",SR_real);
  }
  if (m_recorderActive)
   {
     D_PRINTXY("RECORDER SampleRate:",SR_real);
   }  */

void cAudio::setPreAmpType(enPreAmp type)
{
  switch (type)
  {
  case PRE_AMP_LINEAR:
    digitalWrite(PIN_AMP_2, 1);
    digitalWrite(PIN_AMP_3, 0);
    break;

  case PRE_AMP_HIGH_PASS:
    digitalWrite(PIN_AMP_2, 0);
    digitalWrite(PIN_AMP_3, 1);
    break;
  }
}

void cAudio::setPreAmpGain(enGain gain)
{
  switch (gain)
  {
  case GAIN_LO:
    digitalWrite(PIN_AMP_0, 0);
    digitalWrite(PIN_AMP_1, 1);
    break;

  case GAIN_MED:
    digitalWrite(PIN_AMP_0, 1);
    digitalWrite(PIN_AMP_1, 0);
    break;

  case GAIN_HI:
    digitalWrite(PIN_AMP_0, 0);
    digitalWrite(PIN_AMP_1, 0);
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
  float vol = pow(10, (devPars.volume.get() / 10));
  if (vol > 32)
    vol = 32;
  float freq = devPars.mixFreq.get() * 1000.0;

  setPreAmpType((enPreAmp)devPars.preAmpType.get());
  setPreAmpGain((enGain)devPars.preAmpGain.get());

  if (isSetupNeeded())
  {
    m_old.volume = devPars.volume.get();
    AudioNoInterrupts();
    delay(200);
    setSampleRate((enSampleRate)devPars.sampleRate.get());
    switch (devStatus.opMode.get())
    {
    case OPMODE_HEAR_DIRECT:
      m_mixer.gain(MIX_CHAN_MIC, vol);
      m_mixer.gain(MIX_CHAN_PLAY, 0);
      setOscFrequency(0);
      break;

    case OPMODE_HEAR_HET:
    case OPMODE_REC_AUTO:
      m_mixer.gain(MIX_CHAN_MIC, vol);
      m_mixer.gain(MIX_CHAN_PLAY, 0);
      setOscFrequency(freq);
      break;

    case OPMODE_PLAY_STRETCHED:
    {
      m_mixer.gain(MIX_CHAN_MIC, 0.0);
      m_mixer.gain(MIX_CHAN_PLAY, vol);
      setOscFrequency(0);
      enSampleRate srStretch = SR[devPars.sampleRate.get()].stretched;
      setSampleRate(srStretch);
    }
    break;

    case OPMODE_PLAY_DIRECT:
      m_mixer.gain(MIX_CHAN_MIC, 0.0);
      m_mixer.gain(MIX_CHAN_PLAY, vol);
      setOscFrequency(0);
      break;

    case OPMODE_PLAY_HET:
      m_mixer.gain(MIX_CHAN_MIC, 0.0);
      m_mixer.gain(MIX_CHAN_PLAY, vol);
      setOscFrequency(freq);
      break;
    }
    m_old.opMode = (enOpMode)devStatus.opMode.get();
    m_recThresh = pow(10, (devPars.recThreshhold.get() / 10));
    delay(200);
    AudioInterrupts();
    delay(20);

    DPRINTLN4("[***** AUDIO SETTINGS ******]\n");
    DPRINTF4("       op mode: %i %s\n", devStatus.opMode.get(), devStatus.opMode.getActText());
    DPRINTF4("   sample rate: %s  index: %i   value: %i\n", devPars.sampleRate.getActText(), devPars.sampleRate.get(), m_sampleRate);
    DPRINTF4("      mix freq: %f  setup freq: %f\n", freq, m_freq_oscillator);
    DPRINTF4("rec threshhold: %f\n", devPars.recThreshhold.get());
    DPRINTF4("        volume: %f\n", vol);
  }
  else
    DPRINTLN2("no setup needed");
  DPRINTF4("    pre amp type: %s\n", devPars.preAmpType.getActText());
  DPRINTF4("    pre amp gain: %s\n", devPars.preAmpGain.getActText());
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
    if (devStatus.cassMode != CAS_PLAY)
    {
      m_cass.setFileName(devPars.fileName.get());
      m_cass.startPlay();
      delay(5);
    }
    break;
  case 2:
    if (devStatus.cassMode != CAS_REC)
    {
      setAudioConnections(0);
      m_cass.startRec(devPars.recTime.get());
      devStatus.recCount.set(devStatus.recCount.get() + 1);
      delay(5);
    }
    break;
  }
}

void cAudio::setOscFrequency(float freq)
{
  if (freq < 0.1)
    m_sineHet.phase(90);
  m_sineHet.amplitude(1.0);
  float maxF = m_sampleRate / 1000.0 / 2;
  devPars.mixFreq.init(10, maxF, 1, 0);
  if (devPars.mixFreq.get() > devPars.mixFreq.getMax())
    devPars.mixFreq.set(devPars.mixFreq.getMax());
  m_freq_oscillator = freq * AUDIO_SAMPLE_RATE_EXACT / m_sampleRate;
  m_sineHet.frequency(m_freq_oscillator);
  m_old.oscFrequency = devPars.mixFreq.get();
}

void cAudio::checkAutoRecording(cMenue &menue)
{
  float pv;
  if (m_peak.available())
  {
    pv = m_peak.read();
    if (menue.keyPauseLongEnough(300) && (devStatus.opMode.get() == OPMODE_REC_AUTO))
    {
      if (devStatus.playStatus.get() == 0)
      {
        if ((pv > m_recThresh) && (devStatus.cassMode != CAS_REC))
        {
          //        Serial.println(pv);
          devStatus.recCount.set(devStatus.recCount.get() + 1);
          m_cass.startRec(devPars.recTime.get());
          m_timeout.start();
          delay(5);
        }
      }
    }
  }
}

void cAudio::operateRecorder()
{
  m_cass.operate(devStatus.cassMode);

  if (m_oldCassMode != devStatus.cassMode)
  {
    if (devStatus.cassMode == CAS_STOP)
    {
      switch (devStatus.playStatus.get())
      {
      case 1:
        devStatus.playStatus.set(0);
        break;
      case 2:
        devStatus.playStatus.set(3);
        break;
      default:
        break;
      }
    }
    else
    {
      devStatus.playStatus.set(devStatus.cassMode - 1);
    }
    m_oldCassMode = devStatus.cassMode;
  }

  if (devStatus.playStatus.get() == 3)
  {
    if (m_timeout.runTime() > devPars.deafTime.get())
      devStatus.playStatus.set(0);
  }
}

void cAudio::volume(float vol)
{
#ifdef AUDIO_SGTL5000
  m_audioShield.volume(vol);
#endif
}

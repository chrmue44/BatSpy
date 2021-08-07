#ifndef CMENUE_H
#define CMENUE_H

#include <MenueSystem.h>
#include "types.h"
#include "ctext.h"
#ifndef SIMU_DISPLAY
#include "cCassette.h"
#include "cAudio.h"
#include "cRtc.h"
#endif

#define FREQ_MIN   0
#define FREQ_MAX   150
#define MAX_ADC             0x7FFF
#define DEC_TIM_MIN   5   ///< nr of decimals for left border of graph

/**
 * @brief operation modes of the bat detector
 */

/**
 * @brief The stBatInfo struct
 */
struct stBatInfo
{
  stBatInfo() {}
  cParList name = 0;
  cParStr nameLat = "";
  cParStr freq = "";
  cParStr callLen = "";
  cParStr callDist = "";
  cParStr characteristic = "";
  cParStr wingSpan = "";
  cParStr weight = "";
  cParStr size = "";
  cParStr habitat = "";
  cParStr comment = "";
  cParStr occurrence = "";
};


/**
 * @brief device status
 */
struct stStatus
{
  stStatus() : graph(GRAPH_XT), waterf(GRAPH_FFT) {}

  cParEnum opMode = enOpMode::HEAR_HET; ///< display mode
  cParNum recCount = 0;           ///< nr of recordings
  cParGeoPos geoPos;              ///< actual geographic position
  cParStr fPlayName = "";         ///< name of file to play
  cParStr fRecName = "";          ///< name of actual recFile
  cParEnum playStatus = 0;        ///< status Aufnahme/Abspielen
  cParNum timMin = 0;             ///< left border of time-, waterfall-diagram
  cParNum timMax = 2.5;           ///< right border of time-, waterfall-diagram
  cParNum timStep = 0.5;          ///< time step for cursor in time-, waterfall-diagram
  cParNum cpuAudioAvg = 0;        ///< average audio CPU usage
  cParNum cpuAudioMax = 0;        ///< maximum audio CPU usage
  cParNum audioMem = 0;           ///< audio memory usage
  cParTime time;                  ///< actual time
  cParDate date;                  ///< actual date
  cParGraph graph;                ///< x-t-diagram
  cParBtn* btnMeas;               ///< button to measure pulse width
  cParNum pulseWidth = 0;         ///< measured pulse width
  cParStr measUnit = "ms";
  cParGraph waterf;               ///< waterfall diagram
  int forceDisplay = 0;           ///< force display activity
  cParNum amplMax = 100;          ///< max. amplitude for td display
  cParNum amplMin = -100;         ///< min. amplitude for td display
  stBatInfo bats;                 ///< struct to store bat informations
  cParBtn* btnAudio;              ///< button to clear audio performance measurement
  tDirInfo dir;                   ///< content of current dir
  size_t fileIndex = 0;           ///< index of file in dir
  size_t startIndex = 0;          ///< start index of directory
  cParBtn* btnSetTime;
  cParNum year = 2021;
  cParNum month = 7;
  cParNum day = 13;
  cParNum hour = 22;
  cParNum minute = 0;
  cParEnum lonSign = 0;
  cParNum lonDeg = 8;
  cParNum lonMin = 37;
  cParNum lonSec = 925;
  cParEnum latSign = 0;
  cParNum latDeg = 49;
  cParNum latMin = 45;
  cParNum latSec = 236;
  cParNum freqMin = FREQ_MIN;     ///< min freq for waterfall diagram
  cParNum freq1Tick = (FREQ_MAX - FREQ_MIN)/Y_TICK_CNT;
  cParNum freqMax = FREQ_MAX;     ///< max freq for waterfall diagram
  cParNum peakVal = 0;
  cParStr version = "BatSpy " __DATE__  "/"  __TIME__;
  cParNum freeSpace = 0;
  cParNum voltage = 0;
  cParNum digits = 0;
  cParNum temperature = 0;
};


/**
 * @brief device parameters
 */
struct stParams
{
  cParNum volume =   6;              ///< volume setting
  cParNum mixFreq = 40;              ///< mixer frequency
  cParNum recTime = 3;               ///< recording time
  cParEnum sampleRate = enSampleRate::SR_384K;  ///< sample rate
  cParEnum lang   = LANG_GER;        ///< display language
  cParList dirSel = 0;               ///< select directory
  cParList fileSel = 0;              ///< select file
  cParStr dir = "";                  ///< actual directory on SD card
  cParStr fileName = "";             ///< actual file name
  cParNum threshHold = 10;           ///< threshhold level graph, waterfall
  cParNum fftLevelMin = 3500;        ///< low (threshhold) level for FFT display
  cParNum fftLevelMax = 70000;       ///< high level for FFT display
  cParNum recThreshhold = -18;       ///< auto recording threshhold
  cParNum deafTime = 3;              ///< timeout after one recording
  cParNum backLightTime = 120;       ///< time for backlight
  cParEnum preAmpGain = 1;           ///< gain of pre amplifier
  cParEnum preAmpType = HIGH_PASS;   ///< type of pre amplifier
  cParEnum dispOrient = RIGHT_HAND;  ///< display orientation
  cParEnum knobRotation = CLOCKWISE; ///< knob rotation
  cParNum preTrigger = 20;           ///< pre trigger time [ms]
  cParEnum recFmt = 0;               ///< recording format
  cParNum filtFreq = 16;             ///< hight pass freq for recording trigger
  cParEnum filtType = 0;             ///< filter type for recording trigger
  cParNum startH = 21;               ///< hour of start time
  cParNum startMin = 0;              ///< minute of start time
  cParNum stopH = 6;                 ///< hour of start time
  cParNum stopMin = 0;               ///< minute of start time
  cParNum voltFactor = 1;            ///< factor digits to voltage
  cParEnum recAuto = 0;
  cParNum sendDelay = 2;             ///< delay [ms] after sending 2048 Bytes on USB
};


class cMenue : public cMenuesystem
{
 public:
  cMenue(int width, int height, ILI9341_t3* pDisplay);
  virtual ~cMenue();
  virtual void initDialogs();
  virtual void save();
  virtual void load();
  void printPars();
  void printStatus();

protected:
  virtual void initPars();

private:
  bool checkCRC();
  void loadLanguage();
};


extern stStatus devStatus;           ///< status of the device
extern stParams devPars;             ///< parameters of the device
extern cMenue menue;                 ///< menue system of the device

#endif // CMENUE_H

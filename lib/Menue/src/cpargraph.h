/*************************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * The graph object
 * ***********************************************************/
#ifndef CPARGRAPH_H
#define CPARGRAPH_H

#include "crfft.h"
//#include "crfft_o2.h"  internal fft (doesn't work...)

#include "menuedefs.h"
#include "my_vector.h"

#define SCALE_WIDTH   5
#define COLMAP_DIST   3    ///< distance FFT to color map in pixel
#define COLMAP_HEGHT  15
#define Y_TICK_CNT     5   ///< number of Y ticks in waterfall diagram
#define X_TICK_CNT     4   ///< number of X ticks in live diagram 

struct stTime {
  int16_t yMin[DISP_WIDTH];
  int16_t yMax[DISP_WIDTH];
  int32_t lastX;        ///< last X-value of partial graph
  int32_t lastY;        ///< last X-value of partial graph

};

struct stFft {
  stFft() :
    firstFft(false),
    levelMin(0.0),
    levelMax(1.0),
    fft() 
    {
      memset(scratch, 0, sizeof(scratch));
    }
  
  cRfft fft;
  float levelMin;
  float levelMax;
  bool firstFft;
  int16_t scratch[FFT_SIZE];
};

struct stRtFft
{
  uint32_t tStart;
  float msPerDiv;
  uint16_t line_buffer[DISP_HEIGHT];
  uint16_t count = SCALE_WIDTH;
  int16_t squeeze = 1;    ///< factor to display: 1: each freq of fft, 2: each 3nd freq, ...
  uint16_t x_tick[X_TICK_CNT];
};

struct uData {
  uData() :
  t(),
  f(),
  rf()
  {}
  stTime t;
  stFft f;
  stRtFft rf;
};

enum enGraphMode {
  GRAPH_XT = 1,
  GRAPH_FFT = 2,
  LIVE_FFT = 3,
};

/**
 * @brief represents an x-y-graph
 */
class cParGraph : public cParBase {
 public:
  cParGraph(enGraphMode mode) :
    m_mode(mode) 
  {}
  void setAmplitude(int16_t a) {if(a > 0) m_amplitude = 100 * m_height / 2 / a; }
  void setAmplitudeRaw(int16_t ar) { m_amplRaw = ar; }
  void setY0(int16_t y0) { m_y0 = y0; }
  void initPlot(bool b);
  bool isMinMax() { return m_isMinMax; }
  void setPlotBorders(float tMin, float tMax) { m_tMin = tMin; m_tMax = tMax; }
  void setSize(int16_t x, int16_t y, uint16_t w, uint16_t h) { m_x = x; m_y = y; m_width = w; m_height = h; m_y0 = m_y + h/2; m_yMax = m_y + h;}
  size_t getRemBytes() { return m_fileSize - m_filePos; }
  void advanceFilePos(size_t bytes) { m_filePos += bytes; }
  bool getInitPlot() { return m_initPlot; }
  void setPlotFile(const char* pName, uint32_t sampleRate) {
    strncpy(m_plotFileName, pName, sizeof(m_plotFileName));
    m_sampleRate = sampleRate;
  }
  void setSampleRate(uint32_t sampleRate) { m_sampleRate = sampleRate;}
  void setFftLevels(float min, float max) { m_dat.f.levelMin = min; m_dat.f.levelMax = max; }
  void initDiagram();
  void updateLiveData(uint16_t* data, int16_t maxAmpl);

  /**
   * @brief measure width of a bat call
   * @param levelRaw trigger level for length measurement as raw value
   * @return pulse width [s], if no pulse found: -1
   */
  float measure(int16_t levelRaw);
  void plotGraph();
  /**
   * @brief getMaxFreq get maximum frequency displayed in live graph
   * @return
   */
  float getMaxFreq(size_t sizeFft);
  void setSqueeze(int16_t s, size_t sizeFft);
  uint16_t getSqueeze();
  float getMsPerDiv() { return m_dat.rf.msPerDiv; }

 protected:
  cParGraph();

 private:
  /**
   * @brief drawGrid draw grid for x-t-graph
   */
  void drawGrid();
  void drawYscale();
  void createXtPlot(float samplesPerPixelf);
  void createFftPlot(float samplesPerPixelf);
  int16_t limitY(int16_t y);
  void sety(size_t i, uint16_t x);
  void sety(size_t i, int16_t min, int16_t max);
  void plotAsBand(size_t samplesPerPixel, int16_t xMax);
  void plotAsSinglePixels(float samplesPerPixelf, int16_t pixelToPlot);
  void plotCursor(uint16_t x);
  int initFile();
  uint16_t getColor(float val, float min, float max);
  void drawColorMap();

 private:
  bool m_isMinMax = false;
  int16_t m_amplitude = 240;     ///< amplitude of 100% in pixel
  int16_t m_y0 = 120;            ///< y-coordinate for value 0
  int16_t m_amplRaw = 100;       ///< amplitude of raw value

  int m_measState = 0;     ///< state of measurement
  tFILE m_file;            ///< handle to plotted file
  bool m_plotComplete = false;
  bool m_initPlot = false;
  float m_tMin = 0.0;      ///< min x value of graph in physical units
  float m_tMax = 2.0;      ///< max x value of graph in physical units
  int m_plotState = 0;     ///< state of plotting
  char m_plotFileName[PAR_STR_LEN]; ///< name of plotted file
  uint32_t m_sampleRate = 384000;    ///< sample rate of actual file
  uint16_t m_x = 0;
  uint16_t m_y = 0;
  uint16_t m_yMax = 1;
  uint16_t m_width = 320;
  uint16_t m_height = 240;
  size_t m_filePos = 0;      ///< start position in file
  size_t m_fileSize = 0;     ///< size of the opened file
  uint16_t m_actPixel = 0;   ///< x pos. of currently plotted pixel};
  uData m_dat;
  enGraphMode m_mode = enGraphMode::GRAPH_FFT;
};

#endif // CPARGRAPH_H

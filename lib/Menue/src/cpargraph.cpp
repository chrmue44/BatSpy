#include <cstddef>
#include "cpargraph.h"
#include "debug.h"

#ifndef size_t
typedef std::size_t size_t;
#endif

const uint16_t colorMap[] = {
  0x0000, //0
  0x0002, //1
  0x0004, //2
  0x0010, //3
  0x0038, //4
  0x00F8, //5
  0x01D8, //6
  0x0238, //7
  0x0438, //8
  0x0730, //9
  0x07F0, //10
  0x0FC0, //11
  0x2F40, //12
  0xFFC0, //13
  0xFF00, //14
  0xFD00, //15
  0xFC00, //16
};

void cParGraph::sety(size_t i, uint16_t y) {
  long cy_old = limitY(m_amplitude * m_dat.t.yMin[i] / m_amplRaw + m_y0);
  long cy_new = limitY(m_amplitude * y / m_amplRaw + m_y0);
  int16_t x = m_x + i;

  gpDisplay->drawPixel(x, cy_old, COL_TEXTBACK);
  gpDisplay->drawPixel(x, cy_new, COL_GRAPH);
  m_dat.t.yMin[i] = y;
}


void cParGraph::sety(size_t i, int16_t min, int16_t max) {
  long cy_old_min = limitY(m_y0 - m_amplitude * m_dat.t.yMin[i] / m_amplRaw);
  long cy_old_max = limitY(m_y0 - m_amplitude * m_dat.t.yMax[i] / m_amplRaw);
  long cy_new_min = limitY(m_y0 - m_amplitude * min / m_amplRaw);
  long cy_new_max = limitY(m_y0 - m_amplitude * max / m_amplRaw);
  int16_t x = m_x + i;

  if((cy_old_min < cy_old_max) && (cy_old_min > (m_y0 - m_amplitude)))
    gpDisplay->drawLine(x, cy_old_min, x, cy_old_max, COL_TEXTBACK);
  gpDisplay->drawLine(x, cy_new_min, x , cy_new_max, COL_GRAPH);
  m_dat.t.yMin[i] = min;
  m_dat.t.yMax[i] = max;
}

void cParGraph::plotGraph() {
    cSdCard& sd = cSdCard::inst();
    int result;
    float samplesPerPixelf = (float)((m_tMax - m_tMin) * m_sampleRate) / m_width;
    bool plot = false;

    switch (m_plotState) {
      case 0:
        if (m_initPlot && !m_plotComplete) {
          result = initFile();
          if(result == 0) {
            plot = true;
            m_plotState = 1;
            m_actPixel = 0;
            m_initPlot = false;
          }
        }
        else
          plot = false;
        break;

      case 1:
        plot = true;
        break;

      default:
        m_plotState = 0;
        break;
    }

    if(plot) {
      switch(m_mode) {
        default:
        case GRAPH_XT:
          createXtPlot(samplesPerPixelf);
          break;
        case GRAPH_FFT:
          createFftPlot(samplesPerPixelf);
          break;

      }

      if((m_actPixel == m_width) || (m_fileSize - m_filePos) <= 0) {
        result = sd.closeFile(m_file);
        m_plotState = 0;
        m_plotComplete = true;
      }
    }
}

void cParGraph::createXtPlot(float samplesPerPixelf) {
  int16_t pixelToPlot;
  size_t samplesPerPixel = samplesPerPixelf;
  if(((m_tMax - m_tMin) * m_sampleRate) < 5000)
    pixelToPlot =m_width;
  else if (((m_tMax - m_tMin) * m_sampleRate) < 50000)
    pixelToPlot =m_width / 2;
  else
    pixelToPlot = GRAPH_PIXEL_PER_TICK;

  int16_t xMax = (m_actPixel + pixelToPlot) < m_width ? m_actPixel + pixelToPlot :m_width;
  if (samplesPerPixelf > 1.0) {
    m_isMinMax = true;
    plotAsBand(samplesPerPixel, xMax);
  }
  else {
    plotAsSinglePixels(samplesPerPixelf, pixelToPlot);
    m_isMinMax = false;
  }
  update(true);
}

int16_t cParGraph::limitY(int16_t y){
  if(y > m_yMax)
    return m_yMax;
  if(y < m_y)
    return m_y;
  return y;
}

void cParGraph::plotAsSinglePixels(float samplesPerPixelf, int16_t pixelToPlot) {
  int16_t scratch[FFT_SIZE];
  size_t bytesRead;
  cSdCard& sd = cSdCard::inst();

  uint32_t totBytes2read = (uint32_t)(samplesPerPixelf * pixelToPlot) << 1;
  size_t bytes2read = totBytes2read < sizeof(scratch) ? totBytes2read : sizeof(scratch);
  enSdRes result = sd.readFile(m_file, scratch, bytesRead, bytes2read);
  if (result != OK)
    return;

  int32_t x0 = 0;
  int32_t y0 = 0;
  int32_t x1 = 0;
  int32_t y1 = 0;
  size_t size = bytesRead / 2 - 1;
  if(m_actPixel != 0) {
    x1 = m_x + m_actPixel;
    y1 = m_y0 - scratch[0] * m_amplitude / m_amplRaw;
    gpDisplay->drawLine(m_dat.t.lastX, m_dat.t.lastY, x1, y1, COL_GRAPH);
  }
  for(size_t i = 0; i < size; i++) {
    x0 = m_x +m_actPixel + (pixelToPlot - 1) * i / size;  //TODO: X coords not aequidistant if plotted in slices
    x1 = m_x +m_actPixel + (pixelToPlot - 1) *(i + 1) / size; //TODO: X coords not aequidistant if plotted in slices
    y0 = limitY(m_y0 - scratch[i] * m_amplitude / m_amplRaw);
    y1 = limitY(m_y0 - scratch[i + 1] * m_amplitude / m_amplRaw);
    if(x0 > m_x +m_width)
      break;
    gpDisplay->drawLine(x0, y0, x1, y1, COL_GRAPH);
    m_dat.t.yMin[i] = scratch[i];
    m_dat.t.yMax[i] = x0;
  }
  m_dat.t.lastX = x1;
  m_dat.t.lastY = y1;
  m_actPixel = m_width;
}

void cParGraph::plotAsBand(size_t samplesPerPixel, int16_t xMax) {
  int16_t scratch[256];
  size_t bytesRead;
  cSdCard& sd = cSdCard::inst();

  for(int i =m_actPixel; i < xMax; i++) {
    long totBytes2read = (samplesPerPixel * 2) < (getRemBytes()) ?
                          samplesPerPixel * 2 : getRemBytes();
    int16_t min = 0x7FFF;
    int16_t max = -0x7FFF;
    while (totBytes2read > 0) {
      size_t bytes2read = (size_t)totBytes2read < sizeof(scratch) ? totBytes2read : sizeof(scratch);
      enSdRes result = sd.readFile(m_file, scratch, bytesRead, bytes2read);
      if (result != OK)
        break;
      advanceFilePos(bytesRead);
      for (size_t j = 0; j < bytes2read / 2; j++) {
        int16_t val = scratch[j];
        if (min > val)
          min = val;
        if (max < val)
          max = val;
      }
      totBytes2read -= bytes2read;
    }
    if(getRemBytes() <= 0)
      break;
    sety(i, min, max);
    m_actPixel++;
  }
}

void cParGraph::initDiagram() {
  switch (m_mode) {
    case enGraphMode::GRAPH_XT:
      drawGrid();
      break;
    case enGraphMode::GRAPH_FFT:
      drawYscale();   
      drawColorMap();  
      break;
  case enGraphMode::LIVE_FFT:
      drawYscale();
      break;
  }
}

void cParGraph::drawYscale() {
//  for( int y = m_y0 - m_height /2; y <= m_y0 + m_height/2; y+=16) {
  float y = m_y0 - m_height /2;
  for(int i = 0; i <= Y_TICK_CNT; i++, y+=(float)m_height/Y_TICK_CNT) {
    gpDisplay->drawLine(m_x, y, m_x + SCALE_WIDTH, y, COL_GRID);
  }
}

void cParGraph::drawGrid() {
  if(m_initPlot) {
    int16_t ampl = m_height / 2;
    for (int16_t i = 0; i < GRAPH_DIVX_COUNT; i++) {
      int16_t y = m_y0 - ampl + i * ampl / 2;

      for (int16_t x = m_x; x < m_x +m_width; x+=3) {
        gpDisplay->drawPixel(x, y, COL_GRID);
      }
    }

    for (int16_t i = 0; i < 6; i++) {
      int16_t x = m_x + i *m_width / GRAPH_DIVX_COUNT;
      for (int16_t y = m_y0 - ampl; y < m_y0 + ampl; y+=3) {
        gpDisplay->drawPixel(x, y, COL_GRID);
      }
    }
  }
}

void cParGraph::initPlot(bool b) {
  m_initPlot = b;
  m_plotComplete = false;
  if(m_plotState == 1) {
    cSdCard& sd = cSdCard::inst();
    sd.closeFile(m_file);
  }
  if(m_mode == GRAPH_FFT)
    m_dat.f.firstFft = true;
  if(m_mode == enGraphMode::LIVE_FFT)
  {
    m_dat.rf.count = m_x + SCALE_WIDTH;
    for(int i = 0; i < X_TICK_CNT; i++)
      m_dat.rf.x_tick[i] = m_x + (m_width - SCALE_WIDTH)/(X_TICK_CNT) * i + SCALE_WIDTH;
  }
  m_plotState = 0;
  update(true);
}


int cParGraph::initFile() {
  cSdCard& sd = cSdCard::inst();
  int retVal = 1;
  enSdRes result = sd.openFile(m_plotFileName, m_file, READ);
  if(result == OK) {
    m_filePos = m_sampleRate * m_tMin * 2;
    m_filePos >>= 1;
    m_filePos <<= 1;
    m_fileSize = sd.fileSize(m_file);
    result = sd.setFilePos(m_file, m_filePos);
    if(result == OK)
      retVal = 0;
  }
  DPRINTF1("opened file: %s, pos: %lu, result: %i\n",m_plotFileName, m_filePos, retVal);  //@@@
  return retVal;
}


float cParGraph::measure(int16_t levelRaw) {
  float retVal = -1;
  size_t start = 9999;
  size_t end = 9999;
  int32_t level = levelRaw * 2;
  bool done = false;
  if (m_isMinMax)
    m_measState = 0;
  else
    m_measState = 10;
  for(size_t i = 0; i <m_width; i++) {
    int32_t diff = m_dat.t.yMax[i] - m_dat.t.yMin[i];
    switch (m_measState) {
      case 0:
        if (diff < level)
           m_measState = 1;
        break;

      case 1:
        if (diff >= level) {
           start = i;
           m_measState = 2;
        }
        break;

      case 2:
        if (diff < level) {
           end = i;
           retVal = (float)(end - start)/m_width * (m_tMax - m_tMin);
           plotCursor(m_x + start);
           plotCursor(m_x + end);
           done = true;
           m_measState = 0;
        }
        break;

      case 10:
        if (m_dat.t.yMin[i] < levelRaw)
          m_measState = 11;
        break;

      case 11:
        if (m_dat.t.yMin[i] >= levelRaw) {
          m_measState = 12;
          start = i;
        }
        break;

      case 12:
         if (m_dat.t.yMin[i] < levelRaw)
           m_measState = 13;
         break;

      case 13:
        if (m_dat.t.yMin[i] >= levelRaw) {
          m_measState = 0;
          end = i;
          retVal = (float)m_sampleRate / (end - start);
          plotCursor(m_dat.t.yMax[start]);
          plotCursor(m_dat.t.yMax[end]);
          done = true;
        }
        break;

      default:
        m_measState = 0;
    }
    if (done) {
      break;
    }
  }
  return retVal;
}

void cParGraph::plotCursor(uint16_t x) {
  size_t yMin = m_y0 + m_amplitude;
  for(size_t i = m_y0 - m_amplitude; i < yMin; i+=2)
    gpDisplay->drawPixel(x, i, COL_CURSOR);
}

uint16_t cParGraph::getColor(float val, float min, float max) {
  uint16_t size = sizeof(colorMap) / sizeof(colorMap[0]);
  if (val < min)
    return colorMap[0];
  else if (val > max)
    return colorMap[size - 1];
  else {
    uint16_t i = (val - min) / (max - min) * size;
    if(i < size)
      return colorMap[i];
    else
      return colorMap[size-1];
  }
}

void cParGraph::drawColorMap() {
  int16_t min = m_x + 40;
  int16_t max = m_x + m_width - 40;
  for (int16_t x = min; x < max; x++) {
     uint16_t color = getColor(x, min, max);
     gpDisplay->drawLine(x, m_y0 + m_height/2 + COLMAP_DIST, x, m_y0 + m_height/2 + COLMAP_HEGHT, color);
  }
}

void cParGraph::createFftPlot(float samplesPerPixelF) {
  uint16_t pixelToPlot;
  size_t samplesPerPixel = samplesPerPixelF;
  if(((m_tMax - m_tMin) * m_sampleRate) < 5000)
    pixelToPlot =m_width - SCALE_WIDTH;
  else if (((m_tMax - m_tMin) * m_sampleRate) < 50000)
    pixelToPlot = (m_width - SCALE_WIDTH)/ 2;
  else
    pixelToPlot = GRAPH_PIXEL_PER_TICK;
  size_t fftWidth = m_width - SCALE_WIDTH;
  size_t xMax = (m_actPixel + pixelToPlot) < fftWidth ? m_actPixel + pixelToPlot :m_width;

  DPRINTF1("fft plot: s/pix: %f, sRate: %i, pixToPlot: %i\n", samplesPerPixelF, m_sampleRate, pixelToPlot); //@@@

  cSdCard sd = cSdCard::inst();
  size_t totBytes2read;
  int16_t* pStart;
  float max = 0;
  for(size_t x = m_actPixel; x < xMax; x++) {
    DPRINTF1("start copy to scratch, x: %lu\n", x);  //@@@
    if ((samplesPerPixel >= fftWidth) || m_dat.f.firstFft){
      totBytes2read = getRemBytes() < sizeof(m_dat.f.scratch) ? getRemBytes() : sizeof(m_dat.f.scratch);
      pStart = &m_dat.f.scratch[0];
      m_dat.f.firstFft = false;
    }
    else {
      totBytes2read = samplesPerPixel * 2;
      for(size_t a = 0; a < FFT_SIZE - samplesPerPixel; a++)
        m_dat.f.scratch[a] = m_dat.f.scratch[a + samplesPerPixel];
      pStart = &m_dat.f.scratch[FFT_SIZE - samplesPerPixel];
    }
    size_t bytes2read = (size_t)totBytes2read;
//    pStart = &scratch[0];
//    size_t bytes2read = FFT_SIZE * 2;
    size_t bytesRead = 0;
    DPRINTF1("available: %lu bytes to read %i\n", sd.available(m_file), bytes2read);  //@@@
    enSdRes result = sd.readFile(m_file, pStart, bytesRead, bytes2read);
    if ((result != OK) || (bytesRead < bytes2read))
      break;
    advanceFilePos(bytesRead);

    float v[FFT_SIZE/2];


    for(int j = 0; j < FFT_SIZE; j++)
      m_dat.f.fft.setInput(j, m_dat.f.scratch[j]);
    m_dat.f.fft.process();

    for(int j = 0; j < FFT_SIZE/2; j++) {
      v[j] = m_dat.f.fft.getOutput(j);
        if (max < v[j])
          max = v[j];
    }

    for(int j = 0; j < FFT_SIZE/2; j++) {
      uint16_t col = getColor(v[j], m_dat.f.levelMin, m_dat.f.levelMax);
      gpDisplay->drawPixel(x + m_x + SCALE_WIDTH, m_y0 + m_height/2 - j, col);
    } 

    if (samplesPerPixel > fftWidth) {
      advanceFilePos(((samplesPerPixel*2 - bytesRead) >> 1) << 1);
      sd.setFilePos(m_file, m_filePos);
    }
  }
 
  m_actPixel = xMax;
  update(true);
}

void cParGraph::updateLiveData(uint16_t *data, int16_t maxAmpl)
{
  int max = maxAmpl * m_dat.rf.squeeze;
  if(m_dat.rf.squeeze == 0)
    max = maxAmpl;
  bool xTick = false;
  for(int i = 0; i < X_TICK_CNT; i++)
  {
    if (m_dat.rf.x_tick[i] == m_dat.rf.count)
      xTick = true;
  }
  if(xTick)
  {
    for (int i = 0; i < m_height; i++)
      m_dat.rf.line_buffer[i] = COL_GRID;
  }
  else
  {
    switch (m_dat.rf.squeeze)
    {
      case 0:
        for (int i = 0; i < m_height; i++)
        {
          int j = m_height - i - 1;
          int k = i/2;
          m_dat.rf.line_buffer[j] = getColor(data[k], 0, max);
        }
        break;
      case 1:
        for (int i = 0; i < m_height; i++)
        {
          int j = m_height - i - 1;
          m_dat.rf.line_buffer[j] = getColor(data[i], 0, max);
        }
        break;
      case 2:
        for (int i = 0; i < m_height; i++)
        {
          int j = m_height - i - 1;
          int k = 2 * i;
          m_dat.rf.line_buffer[j] = getColor(data[k] + data[k + 1], 0, max);
        }
        break;
      case 3:
        for (int i = 0; i < m_height; i++)
        {
          int j = m_height - i - 1;
          int k = 3 * i;
          m_dat.rf.line_buffer[j] = getColor(data[k] + data[k + 1] +  data[k + 2], 0, max);
        }
        break;
      case 4:
        for (int i = 0; i < m_height; i++)
        {
          int j = m_height - i - 1;
          int k = 4 * i;
          m_dat.rf.line_buffer[j] = getColor(data[k] + data[k + 1] +  data[k + 2] +  data[k + 4], 0, max);
        }
        break;
     }
     float y = 0;
     for(int i = 0; i <= Y_TICK_CNT; i++, y+=(float)m_height/Y_TICK_CNT) 
      m_dat.rf.line_buffer[(int)y] = COL_GRID;
   }

   gpDisplay->writeRect(m_dat.rf.count , m_y, 1, m_height, m_dat.rf.line_buffer);
   m_dat.rf.count++;
   if(m_dat.rf.count > m_width + m_x)
     m_dat.rf.count = m_x + SCALE_WIDTH;;
}

void cParGraph::setSqueeze(int16_t s, size_t sizeFft)
{
  if(m_mode == enGraphMode::LIVE_FFT)
  {
    int max = sizeFft / m_height;
    m_dat.rf.squeeze = s;
    if(m_dat.rf.squeeze > max)
      m_dat.rf.squeeze = max;
    if(m_dat.rf.squeeze < 0)
      m_dat.rf.squeeze = 0;
  }
}

uint16_t cParGraph::getSqueeze()
{
  if(m_mode == enGraphMode::LIVE_FFT)
    return m_dat.rf.squeeze;
  else
    return 1;
}

float cParGraph::getMaxFreq(size_t sizeFft)
{
  if(m_mode == enGraphMode::LIVE_FFT)
  {
    float f;
    if(m_dat.rf.squeeze > 0)
      f = (float)m_sampleRate / 2 * m_height * m_dat.rf.squeeze / sizeFft;
    else
      f = (float)m_sampleRate / 2 * m_height /2  / sizeFft;
    return f;
  }
  else
    return m_sampleRate / 2;
}

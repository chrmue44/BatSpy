#include "cdownsample.h"
#include "utility/dspinst.h"

cDownSample::cDownSample()
  : AudioStream(1, inputQueueArray)
{
}

void cDownSample::setParams(size_t count, int16_t* pParams, size_t downSampleRate )
{
  m_parCnt = count;
  m_pParams = pParams;
  m_downSampleRate = downSampleRate;
  init();
}


void cDownSample::update(void)
{
  audio_block_t *block;
  
  if(m_writeIndex == 0)
  {
    m_dstBlock = allocate();
    if(m_dstBlock == nullptr)
      return;
  }
	// start of processing functions. Could be more elegant based on external
  block = receiveWritable();
  if (!block) return;

  size_t i=0;
  while (i < AUDIO_BLOCK_SAMPLES)
  {
    put(block->data[i]);
    i++;
    m_sum += get();
    m_sampleCnt++;
    if(m_sampleCnt >= m_downSampleRate)
    {
      m_sampleCnt = 0;
      m_dstBlock->data[m_writeIndex] = m_sum / m_downSampleRate;
      m_writeIndex++;
      m_sum = 0;
    }
  }

  if(m_writeIndex >= AUDIO_BLOCK_SAMPLES)
  {
    transmit(m_dstBlock);
    m_writeIndex = 0;
  }
  release(block);
}


void cDownSample::init()
{
  for(size_t i = 0; i < m_parCnt; ++i)
    m_history[i] = 0;
  m_lastReadIndex = 0;
}

void cDownSample::put(int16_t input) 
{
  m_history[m_lastReadIndex++] = input;
  if(m_lastReadIndex == m_parCnt)
    m_lastReadIndex = 0;
}

int16_t cDownSample::get()
{
  long long acc = 0;
  int index = m_lastReadIndex;
  for(size_t i = 0; i < m_parCnt; ++i)
  {
    index = index != 0 ? index-1 : m_parCnt - 1;
//    acc += signed_multiply_32x16b(m_history[index], m_pParams[i]);  (a * b) >> 16
    acc += (long long)m_history[index] * m_pParams[i];
  };
  return acc >> 16;
}

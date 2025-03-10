#ifndef _CDOWNSAMPLE_H_
#define _CDOWNSAMPLE_H_


#include "Arduino.h"
#include "AudioStream.h"


class cDownSample : public AudioStream
{
public:
	cDownSample(void);
	void setParams(size_t count, int16_t* pParams, size_t downSampleRate );
	virtual void update(void);
	
private:
  void init();
  void put(int16_t input);
  int16_t get();

private:
	audio_block_t* inputQueueArray[1];
  audio_block_t* m_dstBlock;
  int16_t* m_pParams;
  size_t m_parCnt;
  size_t m_downSampleRate;
  size_t m_sampleCnt = 0;
  size_t m_lastReadIndex = 0;
  size_t m_writeIndex = 0;
  int32_t m_sum = 0;
  int16_t m_history[32];
};

#endif //#ifndef _CDOWNSAMPLE_H_


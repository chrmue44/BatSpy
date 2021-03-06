/*************************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * FFT class implementation
 * ***********************************************************/
#ifndef _CRFFT_H_
#define _CRFFT_H_

#define FFT_SIZE    256

// https://www.cs.indiana.edu/~bhimebau/CMSIS/Documentation/DSP/html/group___c_f_f_t___c_i_f_f_t.html#ga521f670cd9c571bc61aff9bec89f4c26

#include <Arduino.h>

#include "arm_math.h"
#include "utility/sqrt_integer.h"
#include "utility/dspinst.h"
#include "debug.h"

extern "C" {
extern const int16_t AudioWindowHanning256[];
}


/**
 * calculate FFT and IFFT  
 */
class cRfft {
 public:
  cRfft() :
    m_window(AudioWindowHanning256),    
    m_naverage(1),
    m_count(0)
  {
    m_status = arm_cfft_radix4_init_q15(&m_fft_inst, 256, 0, 1);
  }  

  /**
   * set a single value in the input buffer
   */
  void setInput(uint32_t i, int16_t v) { m_input[i * 2] = v; m_input[i*2+1] = 0; }
  
  int16_t* getInputBuf() { return m_input; }

  /**
   * get amplitude at specified index
   */
  float getOutput(size_t i) { return m_output[i]; }
  
  /**
   * get frequency at specified index
   */
  float getFrequency(size_t i) { return (float)i/ FFT_SIZE * m_sampleRate; }

  /**
   * set sampling rate (for frequency calculation)
   */
  void setSampleRate(float rate) { m_sampleRate = rate; }

  static void apply_window_to_fft_buffer(void *buffer, const void *window) {
    DPRINTF1("apply_window_to_fft_buffer(%i,%i)", (int32_t)buffer, (int32_t)window);
    int16_t *buf = (int16_t *)buffer;
    const int16_t *win = (int16_t *)window;

    for (uint32_t i=0; i < FFT_SIZE; i++) {
      int32_t val = *buf * *win++;
      //*buf = signed_saturate_rshift(val, 16, 15);
      *buf = val >> 15;
      buf += 2;
    }
  }
  
  /**
   * do the calculation
   */
  void process() {

    if(m_status == ARM_MATH_SUCCESS) {
      DPRINTLN1("apply window");
      apply_window_to_fft_buffer(m_input, m_window);
      DPRINTLN1("perform arm_fft");
      arm_cfft_radix4_q15(&m_fft_inst, m_input);
	// G. Heinzel's paper says we're supposed to average the magnitude
	// squared, then do the square root at the end.

/*      if (m_count == 0) {
        for (uint32_t i=0; i < FFT_SIZE/2; i++) {
          uint32_t tmp = *((uint32_t *)m_input + i);
          uint32_t magsq = multiply_16tx16t_add_16bx16b(tmp, tmp);
          m_sum[i] = magsq / m_naverage;
        }
      } else {
        for (uint32_t i=0; i < FFT_SIZE/2; i++) {
          uint32_t tmp = *((uint32_t *)m_input + i);
          uint32_t magsq = multiply_16tx16t_add_16bx16b(tmp, tmp);
          m_sum[i] += magsq / m_naverage;
        }
      }
      if (++m_count == m_naverage) {
        m_count = 0;
        for (uint32_t i=0; i < FFT_SIZE/2; i++) {
          m_output[i] = sqrt_uint32_approx(m_sum[i]);
        }
        m_outputflag = true;
      }*/
      for (uint32_t i=0; i < FFT_SIZE/2; i++) {
        uint32_t tmp = *((uint32_t *)m_input + i);
        uint32_t magsq = multiply_16tx16t_add_16bx16b(tmp, tmp);
        uint32_t sum = magsq / m_naverage;
        m_output[i] = sqrt_uint32_approx(sum);
      }
    }
    else
      Serial.println("Error initialisiation FFT!"); 
  }
  
 private:
	const int16_t* m_window;
	arm_cfft_radix4_instance_q15 m_fft_inst;
  arm_status m_status;
  int16_t m_input[2*FFT_SIZE] __attribute__ ((aligned (4)));;
  int16_t m_output[FFT_SIZE/2] __attribute__ ((aligned (4)));
	//uint32_t m_sum[FFT_SIZE/2] __attribute__ ((aligned (4)));
	uint8_t m_naverage;
  float m_maxValue;
  uint32_t m_maxIndex;
  float m_sampleRate;
	uint8_t m_count;
	volatile bool m_outputflag;
};
#endif   // #ifndef _CRFFT_H_

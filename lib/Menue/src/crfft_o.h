#ifndef _CRFFT_H_
#define _CRFFT_H_

// https://www.cs.indiana.edu/~bhimebau/CMSIS/Documentation/DSP/html/group___c_f_f_t___c_i_f_f_t.html#ga521f670cd9c571bc61aff9bec89f4c26

#include "arm_math.h"
/**
 * calculate FFT and IFFT (Supported FFT Lengths are 16, 64, 256, 1024) 
 */
template  <size_t NN> class cRfft {
 public:
  cRfft(uint32_t inverse) {
     m_status = arm_cfft_radix4_init_f32(&m_instR4, NN, inverse, 1);  
  }  

  /**
   * set a single value in the input buffer
   */
  float setInput(uint32_t i, float v) { m_input[i * 2] = v; m_input[i*2+1] = 0; }
  float* getInputBuf() { return m_input; }

  /**
   * get amplitude at specified index
   */
  float getOutput(size_t i) { return m_output[i]; }
  
  /**
   * get frequency at specified index
   */
  float getFrequency(size_t i) { return (float)i/ NN * m_sampleRate; }

  /**
   * set sampling rate (for frequency calculation)
   */
  void setSampleRate(float rate) { m_sampleRate = rate; }

  /**
   * do the calculation
   */
  void process() {
  /*  arm_cfft_radix4_f32(&m_instR4, m_input); 
    arm_cmplx_mag_f32(m_input, m_output, NN);           
    arm_max_f32(m_output, NN, &m_maxValue, &m_maxIndex); */
  }
 
 protected:
   cRfft(); 
  
 private:
  arm_cfft_radix4_instance_f32 m_instR4;
  arm_status m_status;
  float m_input[2*NN];
  float m_output[NN];
  float m_maxValue;
  uint32_t m_maxIndex;
  float m_sampleRate;
};
#endif   // #ifndef _CRFFT_H_

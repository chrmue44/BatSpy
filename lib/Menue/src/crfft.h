#ifndef _CRFFT_H_
#define _CRFFT_H_

#include <cmath>
#include <cstdint>
#include <cstring>

#define FFT_SIZE    256

//https://it.wikipedia.org/wiki/Trasformata_di_Fourier_veloce

/**
 * simulation for class cRfft for Teensy
 * The number of samples is divided by 2 because one complex value counts as 2 values in Teensy implementation
 */

typedef float tComplexBase;
//#define M_PI 3.14159265

class complex {
 public:
    complex() {}
    complex(tComplexBase r, tComplexBase i) :
      m_r(r), m_i(i) { }
    tComplexBase real() { return m_r; }
    tComplexBase imag() { return m_i; }
    void setImag(tComplexBase i) { m_i = i; }
    void setReal(tComplexBase r) { m_r = r; }
    void setPolar(tComplexBase b, tComplexBase phi) {
      m_r = b * cos(phi);
      m_i = b * sin(phi);
    }

    tComplexBase getR() { return sqrt(m_r * m_r + m_i * m_i); }

    tComplexBase getPhi() {
      if(m_r == 0) {
        if(m_i > 0)
          return M_PI / 2;
        else
          return 1.5 * M_PI;

      } else {
        return atan(m_i / m_r);
      }
    }

  complex operator+(complex& c) {
    complex retVal(this->m_r + c.m_r, this->m_i + c.m_i);
    return retVal;
  }

  complex operator-(complex& c) {
    complex retVal(this->m_r - c.m_r, this->m_i - c.m_i);
    return retVal;
  }

   complex operator*(complex& c) {
    complex retVal(
    this->m_r * c.m_r - this->m_i * c.m_i,
    this->m_r * c.m_i + this->m_i * c.m_r);
    return retVal;
  }

  void operator*=(tComplexBase c) {

    this->m_r *= c;
    this->m_i *= c;
  }

  void operator=(tComplexBase c) {
    m_r = c;
    m_i = 0;
  }

  static complex ePowI(tComplexBase phi) {
    complex retVal(cos(phi), sin(phi));
    return retVal;
  }

  static complex pow(complex c, tComplexBase p) {
    complex retVal;
    retVal.setPolar(::pow(c.m_r, p), p * c.getPhi());
    return retVal;
  }

 private:
   tComplexBase m_r = 0;
   tComplexBase m_i = 0;
};





/**
 * calculate FFT and IFFT (Supported FFT Lengths are 16, 64, 256, 1024) 
 */
class cRfft {
 public:
  cRfft()  {
  }  

  /**
   * set a single value in the input buffer
   */
  void setInput(uint32_t i, float v) {
    complex c(v, 0);
    m_input[i] = c;
  }

  /**
   * get amplitude at specified index
   */
  float getOutput(size_t i) {
     complex c(m_input[i]);
     return sqrt(c.real() * c.real() + c.imag() * c.imag()); }
  
  /**
   * get frequency at specified index
   */
  float getFrequency(size_t i) { return (float)i/ FFT_SIZE * m_sampleRate; }

  /**
   * set sampling rate (for frequency calculation)
   */
  void setSampleRate(float rate) { m_sampleRate = rate; }

  /**
   * do the calculation
   */
  void process() {
     FFT(m_input, FFT_SIZE, 1.0);
  }
  

 private:
   int log2(int N)    //funzione per calcolare il logaritmo in base 2 di un intero
   {
     int k = N, i = 0;
     while(k) {
       k >>= 1;
       i++;
     }
     return i - 1;
   }

   int check(int n)    //usato per controllare se il numero di componenti del vettore di input è una potenza di 2
   {
     return n > 0 && (n & (n - 1)) == 0;
   }

   int reverse(int N, int n)    //calcola il reverse number di ogni intero n rispetto al numero massimo N
   {
     int j, p = 0;
     for(j = 1; j <= log2(N); j++) {
       if(n & (1 << (log2(N) - j)))
         p |= 1 << (j - 1);
     }
     return p;
   }

   void ordina(complex* f1, int N)     //dispone gli elementi del vettore ordinandoli per reverse order
   {
     complex f2[FFT_SIZE];
     for(int i = 0; i < N; i++)
       f2[i] = f1[reverse(N, i)];
     for(int j = 0; j < N; j++)
       f1[j] = f2[j];
   }

   void transform(complex* f, int N)     //calcola il vettore trasformato
   {
     ordina(f, N);    //dapprima lo ordina col reverse order
     complex W[FFT_SIZE]; //vettore degli zeri dell'unità.
                               //Prima N/2-1 ma genera errore con ciclo for successivo
                              //in quanto prova a copiare in una zona non allocata "W[N/2-1]"
     W[1].setPolar(1., -2. * M_PI / N);
     W[0] = 1;
     for(int i = 2; i < N / 2; i++)
       W[i] = complex::pow(W[1], i);
     int n = 1;
     int a = N / 2;
     for(int j = 0; j < log2(N); j++) {
       for(int i = 0; i < N; i++) {
         if(!(i & n)) {
           /*ad ogni step di raddoppiamento di n, vengono utilizzati gli indici */
           /*'i' presi alternativamente a gruppetti di n, una volta si e una no.*/
           complex temp = f[i];
           complex Temp = W[(i * a) % (n * a)] * f[i + n];
           f[i] = temp + Temp;
           f[i + n] = temp - Temp;
         }
       }
       n *= 2;
       a = a / 2;
     }
   }

   void FFT(complex* f, int N, tComplexBase d)
   {
     transform(f, N);
     for(int i = 0; i < N; i++)
       f[i] *= d; //moltiplica il vettore per il passo in modo da avere il vettore trasformato effettivo
   }

 private:
  complex m_input[FFT_SIZE];
  float m_maxValue;
  uint32_t m_maxIndex;
  float m_sampleRate;
};
#endif   // #ifndef _CRFFT_H_

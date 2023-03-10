/*************************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * A vector like container without dynamic memory allocation
 * ***********************************************************/
#ifndef MY_VECTOR_H
#define MY_VECTOR_H
#include <Arduino.h>
template  <class T, size_t NN> class  my_vector {
 public:
  my_vector()
  {
  }

  virtual ~my_vector() {

  }
  
  size_t size() { return m_size; }
  size_t maxSize() { return NN; }
  
  T &operator[](size_t i) {
    if( i >= m_size ) {
      return m_arr[0];
    }
    return m_arr[i];
  }

  int push_back(T p) {
    if(m_size < NN) {
      m_arr[m_size] = p;
      m_size++;
      return 0;
    }
    else {
      Serial.printf("my_vector, maxsize reached: %i\n", NN);
      return 1;
    }
  }

  T pop_back() {
    if (m_size > 0)
      m_size--;
    return m_arr[m_size];
  }

  size_t begin() { return 0; }

  void erase (size_t i) {
    if(i < m_size) {
      for( size_t j = i; j < (m_size - 1); j++) {
        m_arr[j] = m_arr[j+1];
      }
      m_size--;
    }
  }

  void clear() { m_size = 0; }

 private:
  
  size_t m_size = 0;
  T m_arr[NN];
};


#endif //#ifndef MY_VECTOR_H

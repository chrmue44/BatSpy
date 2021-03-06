/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * A simple template class to calculate moving mean with
 * minimum CPU usage
 *************************************************************/
#ifndef C_MEANCALC_H
#define C_MEANCALC_H

#include <stddef.h>

template <class T, size_t N> class cMeanCalc
{
 public:
  cMeanCalc() :
    m_cnt(0),
    m_idx(0),
    m_sum(0)
  {
    for(size_t i = 0; i < N; i++)
      m_list[i] = 0;

  }

  T get(T newVal)
  {
    m_list[m_idx] = newVal;
    m_sum += newVal;
    m_idx++;
    if(m_cnt < N)
      m_cnt++;
    if(m_idx >= N)
      m_idx = 0;
    m_sum -= m_list[m_idx];
    T retVal = m_cnt < N ? m_sum / m_cnt : m_sum / (m_cnt - 1);
    return retVal;
  }

 private:
  size_t m_cnt;
  size_t m_idx;
  T m_sum;
  T m_list[N];
};

#endif  //#ifndef C_MEANCALC_H

/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "cWheels.h"
//#define DEBUG_LEVEL  1
#include "debug.h"

cWheels::cWheels(int la, int lb, int btnL, int ra, int rb, int btnR) :
m_left(la, lb),
m_btnLeft(btnL, DEBOUNCE_TIME),
m_right(ra, rb),
m_btnRight(btnR, DEBOUNCE_TIME)
{
  pinMode(btnL,INPUT_PULLUP);
  pinMode(btnR,INPUT_PULLUP);
  pinMode(la, INPUT_PULLUP);
  pinMode(ra, INPUT_PULLUP);
  pinMode(lb, INPUT_PULLUP);
  pinMode(rb, INPUT_PULLUP);
  init();
}


void cWheels::init() {
  m_leftPos = m_left.read();
  m_rightPos = m_right.read();
}

void cWheels::increaseWrIdx() {
  m_wrIdx++;
  if(m_wrIdx == BUF_SIZE)
    m_wrIdx = 0;
}

void cWheels::checkEncoders() {
  int left = m_left.read();
  int right = m_right.read();
  
  if (left != m_leftPos) {
    m_diffl += (m_leftPos - left);
    m_leftPos = left;
    DPRINTF1("pos left: %i\n",m_leftPos);
  }

  if (right != m_rightPos) {
    m_diffr += (m_rightPos - right);
    m_rightPos = right;
    DPRINTF1("pos right: %i\n", m_rightPos);
  }
  
  if(m_diffl <= -SENSITIVITY) {
    DPRINTF1("diff left: %i\n", m_diffl);
    while (m_diffl <= -SENSITIVITY) {
      m_keys[m_wrIdx] = m_ccw ? enKey::DOWN : enKey::UP;
      increaseWrIdx();
      m_diffl += SENSITIVITY;
      DPRINTF1("encoder left: enKey::UP, diff: %i, index %i\n", m_diffl, m_wrIdx); 
    }
  }
  else if(m_diffl >= SENSITIVITY) {
    DPRINTF1("diff left: %i\n", m_diffl);
    while (m_diffl >= SENSITIVITY) {
      m_keys[m_wrIdx] = m_ccw ? enKey::UP : enKey::DOWN;
      increaseWrIdx();
      m_diffl -= SENSITIVITY;
    }
    DPRINTF1("encoder left: enKey::DOWN, diff: %i, index %i\n", m_diffl, m_wrIdx); 
  }
  m_btnLeft.update();
  if(m_btnLeft.fallingEdge()) {
    m_keys[m_wrIdx] = enKey::KEY_OK;
    increaseWrIdx();
    DPRINTLN1("encoder left: enKey::KEY_OK");
  }
  /*
  if(diffr < 0) {
    for(int i = 0; i < -diffl; i++) {
      m_keys[m_wrIdx] = LEFT;
      m_wrIdx++;
      if(m_wrIdx == BUF_SIZE)
        m_wrIdx = 0;
    }
  }
  if(diffr > 0) {
    for(int i = 0; i < diffl; i++) {
      m_keys[m_wrIdx] = RIGHT;
      m_wrIdx++;
      if(m_wrIdx == BUF_SIZE)
        m_wrIdx = 0;
    }
  }*/
}

enKey cWheels::getKey() {
  checkEncoders();
  if(m_wrIdx != m_rdIdx) {
    enKey retVal = m_keys[m_rdIdx];
    m_rdIdx++;
    if(m_rdIdx == BUF_SIZE)
      m_rdIdx = 0;
#if DEBUG_LEVEL == 1
    if(retVal == enKey::DOWN)
      DPRINTLN1("Key: pressed enKey::DOWN\n");
    if(retVal == enKey::UP)
      DPRINTLN1("Key: pressed enKey::UP\n");
    if(retVal == enKey::KEY_OK)
      DPRINTLN1("Key: pressed enKey::KEY_OK\n");
#endif
    return retVal;
  }
  else
    return enKey::NOKEY;
}

/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "cWheels.h"
//#define DEBUG_LEVEL  1
#include "config.h"
#include "debug.h"

cWheels::cWheels(int la, int lb, int btnL):
m_enc(la, lb),
m_pos(0),
m_btn(btnL, DEBOUNCE_TIME, START_REPEAT, BTN_REPEAT_TIME),
m_btnUp(la, DEBOUNCE_TIME, START_REPEAT, BTN_REPEAT_TIME),
m_btnDown(lb,DEBOUNCE_TIME, START_REPEAT, BTN_REPEAT_TIME),
m_ccw(false),
m_wrIdx(0),
m_rdIdx(0),
m_diff(0),
m_btnPin(btnL)
{
}


void cWheels::init()
{
  pinMode(m_btnPin, INPUT_PULLUP);
  m_pos = m_enc.read();
  m_enc.init();
}

void cWheels::increaseWrIdx()
{
  m_wrIdx++;
  if(m_wrIdx >= KEYBUF_SIZE)
    m_wrIdx = 0;
}

void cWheels::checkButtons()
{
  m_btn.update();
  if (m_btn.fallingEdge())
  {
    m_keys[m_wrIdx] = enKey::KEY_OK;
    increaseWrIdx();
    DPRINTLN1("buttons enKey::KEY_OK");
  }
  m_btnUp.update();
  if (m_btnUp.fallingEdge())
  {
    m_keys[m_wrIdx] = enKey::UP;
    increaseWrIdx();
    DPRINTLN1("buttons enKey::UP");
  }
  m_btnDown.update();
  if (m_btnDown.fallingEdge())
  {
    m_keys[m_wrIdx] = enKey::DOWN;
    increaseWrIdx();
    DPRINTLN1("buttons enKey::DOWN");
  }
}

void cWheels::checkEncoders()
{
  int left = m_enc.read();

  if (left != m_pos)
  {
    m_diff += (m_pos - left);
    m_pos = left;
    DPRINTF1("pos left: %i\n",m_pos);
  }

  if(m_diff <= -SENSITIVITY)
  {
    DPRINTF1("diff left: %i\n", m_diff);
    while (m_diff <= -SENSITIVITY)
    {
      m_keys[m_wrIdx] = m_ccw ? enKey::DOWN : enKey::UP;
      increaseWrIdx();
      m_diff += SENSITIVITY;
      DPRINTF1("encoder left: enKey::UP, diff: %i, index %i\n", m_diff, m_wrIdx); 
    }
  }
  else if(m_diff >= SENSITIVITY)
  {
    DPRINTF1("diff left: %i\n", m_diff);
    while (m_diff >= SENSITIVITY)
    {
      m_keys[m_wrIdx] = m_ccw ? enKey::UP : enKey::DOWN;
      increaseWrIdx();
      m_diff -= SENSITIVITY;
    }
    DPRINTF1("encoder left: enKey::DOWN, diff: %i, index %i\n", m_diff, m_wrIdx); 
  }
  m_btn.update();
  if(m_btn.fallingEdge())
  {
    m_keys[m_wrIdx] = enKey::KEY_OK;
    increaseWrIdx();
    DPRINTLN1("encoder left: enKey::KEY_OK");
  }
  if(m_btn.risingEdge())
  {
    m_keys[m_wrIdx] = enKey::KEY_RELEASED;
    increaseWrIdx();
    DPRINTLN1("encoder left: enKey::KEY_RELEASED");
  }
}

enKey cWheels::getKey()
{
  if (hasDisplay() == enDisplayType::OLED_128)
    checkButtons();
  else
    checkEncoders();

  if(m_wrIdx != m_rdIdx)
  {
    enKey retVal = m_keys[m_rdIdx];
    m_rdIdx++;
    if(m_rdIdx >= KEYBUF_SIZE)
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

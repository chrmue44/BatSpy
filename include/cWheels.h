/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * A wrapper class to access two encoders with push button
 * ***********************************************************/
#ifndef CWHEELS_H_
#define CWHEELS_H_

#include <MenueSystem.h>
#include <SimpleEncoder.h>
#include <Bounce.h>


#define KEYBUF_SIZE 20
#define DEBOUNCE_TIME 100  //50
#define SENSITIVITY   4  //4
/*
 *
 */
class cWheels {
 public:
    cWheels(int la, int lb, int btnL);
  void init();
  enKey getKey();
  void setDirection(bool ccw) { m_ccw = ccw; }
  SimpleEncoder* getEncoder() { return &m_enc; }     //for simulation
  Bounce* getBtnOk() { return &m_btn; }              //for simulation
  Bounce* getBtnUp() { return &m_btnUp; }              //for simulation
  Bounce* getBtnDown() { return &m_btnDown; }              //for simulation
  bool isKeyPressed() { return (digitalRead(m_btnPin) == 0); }

 protected:
  void checkEncoders();
  void increaseWrIdx();
  void checkButtons();

 private:
  SimpleEncoder m_enc;
  int m_pos;
  Bounce m_btn;
  Bounce m_btnUp;
  Bounce m_btnDown;
  bool m_ccw;

  enKey m_keys[KEYBUF_SIZE];
  int m_wrIdx;
  int m_rdIdx;
  int m_diff;
  int m_btnPin;
};



#endif  //#ifndef CWHEELS_H_

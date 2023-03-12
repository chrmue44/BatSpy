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


#define BUF_SIZE 20
#define DEBOUNCE_TIME 100  //50
#define SENSITIVITY   4  //4

class cWheels {
 public:
    cWheels(int la, int lb, int btnL, int ra, int rb, int btnR);
  void init();
  enKey getKey();
  void setDirection(bool ccw) { m_ccw = ccw; }
  SimpleEncoder& getEncoder() { return m_left; }     //for simulation
  Bounce& getBounce() { return m_btnLeft; }
   
 protected:
  void checkEncoders();
  void increaseWrIdx();

 private:
  SimpleEncoder m_left;
  int m_leftPos;
  Bounce m_btnLeft;
  SimpleEncoder m_right;
  int m_rightPos;
  Bounce m_btnRight;
  bool m_ccw;

  enKey m_keys[BUF_SIZE];
  int m_wrIdx = 0;
  int m_rdIdx = 0;
  int m_diffl = 0;
  int m_diffr = 0;
};



#endif  //#ifndef CWHEELS_H_

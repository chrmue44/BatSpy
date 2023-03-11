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

////#define ENCODER_DO_NOT_USE_INTERRUPTS
////#include <Encoder.h>
#include <MenueSystem.h>
#include <Bounce.h>

class Encoder
{
 public:
  Encoder(uint8_t pin1, uint8_t pin2) : 
  m_pin1(pin1),
  m_pin2(pin2) 
  {
		pinMode(m_pin1, INPUT_PULLUP);
		pinMode(m_pin2, INPUT_PULLUP);
  }

  uint32_t read()
  {
    update();
    return m_position;
  }

 private:
 	void update(void)
  {
		uint8_t s = m_state & 3;
		if (digitalRead(m_pin1)) 
      s |= 4;
		if (digitalRead(m_pin2))
      s |= 8;
		switch (s) 
    {
			case 0: case 5: case 10: case 15:
				break;
			case 1: case 7: case 8: case 14:
				m_position++; break;
			case 2: case 4: case 11: case 13:
				m_position--; break;
			case 3: case 12:
				m_position += 2; break;
			default:
				m_position -= 2; break;
		}
		m_state = (s >> 2);
	}

 private:
  uint8_t m_pin1;
  uint8_t m_pin2;
  uint8_t m_state = 0;
  uint32_t m_position = 0;
};

#define BUF_SIZE 20
#define DEBOUNCE_TIME 100  //50
#define SENSITIVITY   4  //4

class cWheels {
 public:
    cWheels(int la, int lb, int btnL, int ra, int rb, int btnR);
  void init();
  enKey getKey();
  void setDirection(bool ccw) { m_ccw = ccw; }
  Encoder& getEncoder() { return m_left; }     //for simulation
  Bounce& getBounce() { return m_btnLeft; }
   
 protected:
  void checkEncoders();
  void increaseWrIdx();

 private:
  Encoder m_left;
  int m_leftPos;
  Bounce m_btnLeft;
  Encoder m_right;
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

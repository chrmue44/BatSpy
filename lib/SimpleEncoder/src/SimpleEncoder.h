#ifndef _SIMPLEENCODER_H_
#define _SIMPLEENCODER_H_

#include <Arduino.h>

class SimpleEncoder
{
 public:
  SimpleEncoder(uint8_t pin1, uint8_t pin2) : 
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

#endif //#ifndef _SIMPLEENCODER_H_

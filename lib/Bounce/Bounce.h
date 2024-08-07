
/*
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */



/*  * * * * * * * * * * * * * * * * * * * * * * * * * * *
 Main code by Thomas O Fredericks
 Rebounce and duration functions contributed by Eric Lowry
 Write function contributed by Jim Schimpf
 risingEdge and fallingEdge contributed by Tom Harkaway
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef Bounce_h
#define Bounce_h

#include <inttypes.h>

enum class enBounceState
{
  RELEASED = 0,
  PRESSED = 1,
  REPEAT= 2,
};

class Bounce
{

public:
	// Initialize
  Bounce(uint8_t pin, uint32_t interval_millis, uint32_t start_repeat_millis, uint32_t repeat_rate_millis);
	// Updates the pin
	// Returns 1 if the state changed
	// Returns 0 if the state did not change
  int update();
	// Returns the updated pin state
  int read() { return (int)m_pinState; }
  // Returns the number of milliseconds the pin has been in the current state
  uint32_t duration() {return timeDiff(m_previous_millis, millis()); }
// The risingEdge method is true for one scan after the de-bounced input goes from off-to-on.
  bool  risingEdge() { return m_pinStateChanged && m_pinState; }
// The fallingEdge  method it true for one scan after the de-bounced input goes from on-to-off. 
  bool  fallingEdge() { return m_pinStateChanged && !m_pinState; }
  /// calculate the time difference between two milli values (handles overroll)
  uint32_t timeDiff(uint32_t start, uint32_t end);

protected:
  Bounce() {}
  int debounce();
  uint8_t m_pin;
  uint32_t m_interval_millis;
  uint32_t m_start_repeat_millis;
  uint32_t m_repeat_rate_millis;

  uint32_t m_previous_millis;

  uint8_t m_pinState;
  uint8_t m_pinStateChanged;
  enBounceState m_state;
};

#endif



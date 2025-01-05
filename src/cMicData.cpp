#include "cMicData.h"

cMicData::cMicData(uint8_t pin) :
_ds(new OneWire(pin))
{
  memset(&_info, 0, sizeof(_info));
  strncpy(_info.id, "unknown", sizeof(_info.type));
  strncpy(_info.type, "not detected", sizeof(_info.type));
  _info.crc = calcCrc();
}

void cMicData::setId(const char* id) 
{
  strncpy(_info.id, id, sizeof(_info.id));
  _info.id[sizeof(_info.id) - 1] = 0;
}

void cMicData::setType(const char* type) 
{
  strncpy(_info.type, type, sizeof(_info.type));
  _info.type[sizeof(_info.type) - 1] = 0; 
}

void cMicData::setComment(const char* comment) 
{
  strncpy(_info.stuff, comment, sizeof(_info.stuff));
  _info.stuff[sizeof(_info.stuff) - 1] = 0; 
}

bool cMicData::setFreqResponsePoint(stFreqItemFloat p, uint32_t index)
{
  bool retVal = true;
  if((index < sizeof(_info.f)/sizeof(_info.f[0])) &&
     (index < _info.pointCnt))
    _info.f[index] = p;
  else
    retVal = false;
  return retVal;  
} 


bool cMicData::flashRead(void)
{
  bool retVal = false;
  if (_ds->reset())
  {
    _ds->write(0xCC);
    _ds->write(0xF0);
    _ds->write(0x00);
    _ds->write(0x00);
      // Read each byte of 512 and mirror onto prox
    char* p = (char*) &_info;
    for( int i = 0; i < 512; i++) 
    {
      *p = _ds->read();
      p++;
    }
   uint16_t crc = calcCrc();
    retVal = _info.crc == crc;
  }
  return retVal;
}


bool cMicData::flashWrite(uint16_t startAddr, uint16_t len)
{
  // Write to the Flash

  // Each loop is a 4 byte chunk
  uint8_t bytesPerWrite = 8;
  _info.crc = calcCrc();  
  char* p = (char*) &_info;
  for( uint16_t offset = startAddr; offset < (len/bytesPerWrite); offset++)
  {
    uint16_t memAddress = offset * bytesPerWrite;
      // Fail if presense fails
    if (!_ds->reset())
    {
      return false;
    } 
    _ds->write(0xCC); // Skip ROM
    _ds->write(0x0F); // Write Scratch Pad

      // Compute the two byte address
    uint8_t a1 = (uint8_t) memAddress;
    uint8_t a2 = (uint8_t) (memAddress >> 8);

      // Write the offset bytes . . . why in this order?!?!?
    _ds->write(a1);
    _ds->write(a2);

      // Fill the 4 byte scratch pad
    for (uint8_t i = 0; i < bytesPerWrite; i++)
      _ds->write(p[memAddress + i]);

    // Fail if presense fails
    if (!_ds->reset())  
    {
      return false; 
    }
    _ds->write(0xCC); // Skip ROM
    _ds->write(0xAA); // Read scratch pad

      // Confirm offset gets mirrored back, otherwise fail
    if(_ds->read() != a1) 
    {
      return false;
    }
    if(_ds->read() != a2) 
    {
      return false;
    }
      // Read the confirmation byte, store it
    uint8_t eaDS = _ds->read();
      // Check the scratchpad contents
    for (int i = 0; i < bytesPerWrite; i++)
    {  
      uint8_t verifyByte = _ds->read();
      if ( verifyByte != p[(offset*bytesPerWrite + i)]) 
      {
        return false; 
      }
    }
      // Fail if presense fails
    if (!_ds->reset()) 
    {
      return false; 
    }
      // Commit the 8 bytes above
    _ds->write(0xCC); // Skip ROM
    _ds->write(0x55); // Copy scratchpad

      // Confirmation bytes
    _ds->write(a1);
    _ds->write(a2);
    _ds->write(eaDS, 1); // Pullup!
    delay(5); // Wait 5 ms per spec.
  }
  return true;
}

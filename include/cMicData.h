/*************************************************************
 * BatSpy: Teensy 4.x based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2024 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * the class cMicData provides an interface to save and      *
 * load microphone data to a one wire EEPROM chip
 * ***********************************************************/
#include <cstdint>
#include <OneWire.h>

//   100,   120,   150,   180,   220,   270,   330,   390,   470,   560,   680,   820,
//  1000,  1200,  1500,  1800,  2200,  2700,  3300,  3900,  4700,  5600,  6800,  8200,
// 10000, 12000, 15000, 18000, 22000, 27000, 33000, 39000, 47000, 56000, 68000, 82000,
// 100000, 120000, 150000, 180000

struct stFreqItemFloat
{
  float freq = 0;
  float ampl = 0;  
};

struct stMicInfo
{
  uint16_t crc;          //addr 000 ... 001
  uint16_t pointCnt;     //addr 002 ... 003
  char id[16];           //addr 004 ... 013
  char type[16];         //addr 014 ... 023
  stFreqItemFloat f[40]; //addr 024 ... 163
  char stuff[0x9C];      //addr 164 ... 1FF
};

class cMicData
{
 public:
  cMicData(uint8_t pin)
  :
  _ds(new OneWire(pin))
  {
    memset(&_info, 0, sizeof(_info));
  }

  bool init();
  void setId(const char* id);
  char* getId() {return &_info.id[0]; }
  void setType(const char* type);
  char* getType() {return &_info.type[0]; }
  void setComment(const char* comment);
  char* getComment() {return &_info.stuff[0]; }
  bool setFreqResponsePoint(stFreqItemFloat item, uint32_t index); 
  void setFreqResponsePointCount( uint32_t count) { _info.pointCnt = count; } 
  stFreqItemFloat* getFreqResponsePoint(int i) {return i < _info.pointCnt ? &_info.f[i] : nullptr; }
  int getFreqResponsePointCount() {return _info.pointCnt; }
  bool read() { return flashRead(); }
  bool write() { return flashWrite(0, sizeof(_info)); }
  uint16_t calcCrc() {return _ds->crc16((const uint8_t*)&_info + 2, 510); }

private:
  bool flashRead();
  bool flashWrite(uint16_t startAddr, uint16_t len);
  OneWire* _ds;
  stMicInfo _info;
};
/*************************************************************
 * BatSpy: Teensy 4.x based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2024 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "cBattery.h"
#include "config.h"
struct stPair
{
  float level;
  float voltage;
};

const MEMP stPair charge[] =
{
  {   0,   6.0 },
  {  10,   6.6 },
  {  20,   6.8 },
  {  30,   6.9 },
  {  40,   7.1 },
  {  50,   7.2 },
  {  60,   7.4 },
  {  70,   7.6 },
  {  80,   7.8 },
  {  90,   8.0 },
  { 100,   8.2 }
};

float cBattery::getChargeCondition(float voltage)
{
  float retVal = 0;
  int s = sizeof(charge) / sizeof(charge[0]);
  if(voltage < charge[0].voltage)
    retVal = 0.0;
  else if (voltage > charge[s - 1].voltage)
    retVal =100.0;
  else
  {
    for(int i = 0; i < (s - 1); i++)
    {
      if((voltage >= charge[i].voltage) && (voltage <= charge[i+1].voltage))
      {
        float m = (charge[i+1].level - charge[i].level) /
                  (charge[i+1].voltage - charge[i].voltage);
        float b = charge[i].level - m * charge[i].voltage;
        retVal = m * voltage + b;
      } 
    }
  }
  return retVal;
}

char battSymbol[15];
const char* cBattery::getBatterySymbol(float chargeLevel)
{
  int ful = (int)(chargeLevel / 10.0 + 0.5) + 1;
  battSymbol[0] = 0xEC;
  battSymbol[1] = 0xED;
  for (int i = 2; i <= ful; i++)
    battSymbol[i] = 0xEE;
  for (int i = ful + 1; i <= 11; i++)
    battSymbol[i] = 0xED;
  battSymbol[12] = 0xED;
  battSymbol[13] = 0xEF;
  battSymbol[14] = 0;
  return &battSymbol[0];
}

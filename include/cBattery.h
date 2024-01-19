/*************************************************************
 * BatSpy: Teensy 4.x based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2024 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/
#ifndef _CBATTERY_H_
#define _CBATTERY_H_

class cBattery
{
  public:
  static float getChargeCondition(float voltage);
};

#endif //#ifndef _CBATTERY_H_

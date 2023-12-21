/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * The complete menue system needed to operate BatSpy based
 * on the library MenueSystem
 * ***********************************************************/
#ifndef CMENUE_H
#define CMENUE_H
/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include <Arduino.h>
#include <EEPROM.h>
#include <MenueSystem.h>
#include "types.h"
#include "ctext.h"
#include "cCassette.h"
#include "cAudio.h"
#include "cRtc.h"
#include "cgps.h"
#include "parameters.h"

#define MAX_ADC             0x7FFF

class cMenue : public cMenuesystem
{
 public:
  cMenue(int width, int height, ILI9341_t3* pDisplay);
  virtual ~cMenue();
  virtual void initDialogs();
  virtual void save();
  virtual void load();
  void printPars();
  void printStatus();
  virtual void setFactoryDefaults(enMode mode);
  void initFileRelatedParams();


protected:
  virtual void initPars();

private:
  int initExpertPanels(tCoord lf);
  int initRecorderPanels(tCoord lf);
  int initHandheldPanels(tCoord lf);
};

#endif // CMENUE_H

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

#define LINE_HEIGHT_TFT     13               ///< distance between 2 text lines
#define LINE_HEIGHT_OLED    10               ///< distance between 2 text lines
#define DISP_WIDTH_TFT     320
#define DISP_WIDTH_OLED    128
#define DISP_HEIGHT_TFT    240
#define DISP_HEIGHT_OLED   128
/// colors for color TFT
#define COL_TFT_TEXT            0xFFFF
#define COL_TFT_TEXT_PAR        0xFFC0
#define COL_TFT_TEXT_DIR        0x07E0
#define COL_TFT_TEXTBACK        0x0000
#define COL_TFT_TEXTSEL         0x0000
#define COL_TFT_TEXTSELBACK     0xFFFF
#define COL_TFT_TEXTEDIT        0x0000
#define COL_TFT_TEXTEDITBACK    0x0FC0
#define COL_TFT_MSGSHADOW       0x8410
#define COL_TFT_TEXTHDR         0xFFFF
#define COL_TFT_TEXTHDRBACK     0x000F
#define COL_TFT_TEXTDROPBACK    0x0004
#define COL_TFT_MENULINE        0xFFFF
//0011 1000 1110 0111
#define COL_TFT_GRID            0x79E7
#define COL_TFT_GRAPH           0xFFC0
#define COL_TFT_CURSOR          0x7C1F

/// colors for grayscale OLED
#define COL_OLED_TEXT            0xC
#define COL_OLED_TEXT_PAR        0xF
#define COL_OLED_TEXT_DIR        0xF
#define COL_OLED_TEXTBACK        0x0
#define COL_OLED_TEXTSEL         0x0
#define COL_OLED_TEXTSELBACK     0x8
#define COL_OLED_TEXTEDIT        0x0
#define COL_OLED_TEXTEDITBACK    0xF
#define COL_OLED_MSGSHADOW       0x5
#define COL_OLED_TEXTHDR         0xF
#define COL_OLED_TEXTHDRBACK     0x4
#define COL_OLED_TEXTDROPBACK    0x4
#define COL_OLED_MENULINE        0x0F
//0011 1000 1110 0111
#define COL_OLED_GRID            0x06
#define COL_OLED_GRAPH           0x0C
#define COL_OLED_CURSOR          0x08


class cMenue : public cMenuesystem
{
 public:
  cMenue(int width, int height, Adafruit_GFX* pDisplay);
  virtual ~cMenue();
  virtual void initDialogs();
  virtual void save();
  virtual void load();
  void printPars(size_t parSet);
  void printStatus();
  virtual void setFactoryDefaults(enMode mode);
  void initFileRelatedParams();


protected:
  virtual void initPars();
  virtual void refreshDisplay();

private:
  int initHandheldPanels(tCoord lf);
  int initCompactPanels(tCoord lf);

};

#endif // CMENUE_H

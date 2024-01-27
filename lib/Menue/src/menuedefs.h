/*************************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * custumization of the menue system
 * ***********************************************************/
#ifndef MENUEDEFS_H
#define MENUEDEFS_H

#include "Adafruit_GFX.h"
#include <cSdCard.h>
#include <cstdint>
//#include <ILI9341_t3.h>
#define MMEM FLASHMEM

extern Adafruit_GFX* gpDisplay;

enum enKey {
  NOKEY =      0x8000,
  F1 =         0x8001,
  F2 =         0x8002,
  F3 =         0x8003,
  F4 =         0x8004,
  KEY_OK =     0x8010,
  UP =         0x8011,
  DOWN =       0x8012,
  YES =        0x8013,
  NO =         0x8014,
  LEFT =       0x8015,
  RIGHT =      0x8016,
  TICK =       0x8020,
  TER =        0x8030,
  KEY_RELEASED = 0x8800,
};

typedef uint32_t tCoord;
typedef uint32_t thPanel;
typedef uint32_t thItem;

enum enPanel {
  PNL_DROPDOWN = 1,    ///< dropdown menue
  PNL_PARLIST  = 2,
  PNL_MAIN     = 3,
  PNL_FKEYS    = 4,    ///< F-KEY row
  PNL_HEADER   = 5,    ///< header panel
  PNL_MESSAGE  = 6,    ///< modal message dialog
};

enum enItemType {
  ITEM_TEXT   =  1,     ///< display of plain text
  ITEM_LIST   =  2,     ///< display of list (dynamic list of texts)
  ITEM_ENUM   =  3,     ///< display of enumeration
  ITEM_NR     =  4,     ///< display of number
  ITEM_DATE   =  5,     ///< display of date
  ITEM_TIME   =  6,     ///< display of time
  ITEM_GEO    =  7,     ///< display of geo position
  ITEM_STRING =  8,     ///< display of a string
  ITEM_BUTTON =  9,     ///< display of Button
  ITEM_GRAPH  = 10,     ///< display of x-t-graph
};

#define GRAPH_DIVX_COUNT     5     ///< number of divs in x direction
#define GRAPH_PIXEL_PER_TICK 160    ///< pixel plotted per tick

struct stColors
{
  uint16_t text;
  uint16_t textPar;
  uint16_t textDir;
  uint16_t textBack;
  uint16_t textSel;
  uint16_t textSelBack;
  uint16_t textEdit;
  uint16_t textEditBack;
  uint16_t msgShadow;
  uint16_t textHdr;
  uint16_t textHdrBack;
  uint16_t textDropBack;
  uint16_t menuLine;
  uint16_t grid;
  uint16_t graph;
  uint16_t cursor;
};

 extern const stColors* pColors;

#define PAR_STR_LEN         80      ///< max. ln of string par
#define CNT_PANEL_ITEM      35      ///< max. count of panel items
#define CNT_ENUM_ITEM       20      ///< max. number of enum items
#define LIST_ITEM_LEN       25      ///< max. length of list item text
#define CNT_PANEL_LIST      25      ///< max. number of panels in list


class cParBase {
 public:
  void update(bool u) { m_update = u; }
  bool isUpdate() { return m_update; }
  void automatic(bool a) { m_isAuto = a; }
 private:
  bool m_update = true;
  bool m_isAuto = false; ///< true if destructor of panel shall destroy object
};

#endif // MENUEDEFS_H

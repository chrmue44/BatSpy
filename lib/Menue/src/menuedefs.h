#ifndef MENUEDEFS_H
#define MENUEDEFS_H

#ifdef SIMU_DISPLAY
#include "simulation/cSdCard.h"
#else
#include <cSdCard.h>
#endif
#include <cstdint>
#include <ILI9341_t3.h>

extern ILI9341_t3* gpDisplay;

typedef uint16_t tKey;
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
#define GRAPH_PIXEL_PER_TICK 40    ///< pixel plotted per tick

#define COL_TEXT            0xFFFF
#define COL_TEXTBACK        0x0000
#define COL_TEXTSEL         0x0000
#define COL_TEXTSELBACK     0xFFFF
#define COL_TEXTEDIT        0x0000
#define COL_TEXTEDITBACK    0x0FC0
#define COL_MSGSHADOW       0x8410
#define COL_TEXTHDR         0xFFFF
#define COL_TEXTHDRBACK     0x000F
//0011 1000 1110 0111
#define COL_GRID            0x79E7
#define COL_GRAPH           0xFFC0
#define COL_CURSOR          0x7C1F

#define PAR_STR_LEN         80      ///< max. ln of string par
#define CNT_PANEL_ITEM      35      ///< max. count of panel items
#define CNT_ENUM_ITEM       20      ///< max. number of enum items
#define LIST_ITEM_LEN       20      ///< max. length of list item text

#define DISP_WIDTH          320
#define DISP_HEIGHT         240
#define LINE_HEIGHT         13               ///< distance between 2 text lines
#define FKEYPAN_HEIGHT      (LINE_HEIGHT + 2)  ///< Height of FKEY-Panel
#define HDR_HEIGHT          (LINE_HEIGHT + 2)  ///< height of header panel

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

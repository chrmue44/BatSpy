/*************************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * The panel class and the basic objects that can be placed 
 * on panels.
 * ***********************************************************/
#ifndef CPANEL_H
#define CPANEL_H
#include <TimeLib.h>
#include "menuedefs.h"
#include "my_vector.h"
#include <cstring>
#include "ctext.h"
#include "cpargraph.h"


/**
 * @brief The stParNum struct
 */
class cParNum : public cParBase {
 public:
  cParNum(float v) :
    m_min(0.0),
    m_max(1.0),
    m_step(1),
    m_decimals(0),
    m_leadZeros(0),
    m_val(v) {}
  
  void set(float v) {
    if((v >= m_min) && (v <= m_max))
      m_val = v;
    else if(v < m_min)
      m_val = m_min;
    else
      m_val = m_max;
    update(true);
  }

  float get() { return m_val; }
  uint32_t getDecimals() { return static_cast<uint32_t>(m_decimals); }
  uint32_t getLeadingZeros() { return static_cast<uint32_t>(m_leadZeros); }
  float getMin() { return m_min; }
  float getMax() { return m_max; }
  float getStep() {return m_step; }
  void init(float min, float max, float step, uint16_t dec, uint16_t leadZeros = 0) { m_min = min; m_max = max; m_step = step; m_decimals = dec; m_leadZeros = leadZeros; }

  float m_min;          ///< minimal allowed value
  float m_max;          ///< maximum allowed value
  float m_step;         ///< step for one click on wheel in edit mode
  uint16_t m_decimals;  ///< number of decimals
  uint16_t m_leadZeros; ///< field width for leading zeros (0 no leading zeros)
private:
  float m_val;          ///< actual value
};

class cParStr : public cParBase {
 public:
    cParStr() :
    m_color(0) 
    {
      val[0] = 0;
    }
  cParStr(const char* p) { set(p); }
  char* get() {return val;}
  void set(const char* p) { strncpy(val, p, sizeof(val)); update(true); }
  void setColor(uint16_t col) { m_color = col; }
  uint16_t getColor() { return m_color; }
private:
  char val[PAR_STR_LEN];
  uint16_t m_color;
};

//typedef my_vector<stParStr> tStrList;
class cMenuesystem;
typedef void (* fuFocus)(cMenuesystem*, enKey, cParBase*);


/**
 * @brief represents a date
 */
class cParDate : public cParBase{
 public:
  uint32_t getDay() { return m_day; }
  uint32_t getMonth() { return m_month; }
  uint32_t getYear() { return m_year; }
  void set(uint32_t y, uint32_t m, uint32_t d) { m_day = d; m_month = m; m_year = y; update(true);}
  void set(time_t t) {
     m_day = day(); m_month = month(); m_year = year(); update(true);
  }
 private:
  uint32_t m_day = 1;
  uint32_t m_month = 1;
  uint32_t m_year = 19;
};

/**
 * @brief represents a time
 */
class cParTime : public cParBase{
 public:
  uint32_t getHour() { return m_hour; }
  uint32_t getMin() { return m_min; }
  uint32_t getMinOfDay() { return m_hour * 60 + m_min;}
  uint32_t getSec() { return m_sec; }
  void set(int h, int m, int s) { m_hour = h; m_min = m; m_sec = s; update(true);}
  void set(time_t t) {
     m_hour = hour(); m_min = minute(); m_sec = second(); update(true);
  }
 private:
  uint32_t m_hour = 0;
  uint32_t m_min = 0;
  uint32_t m_sec = 0;
};

/**
 * @brief represents a position
 */
class cParGeoPos : public cParBase {

 public:
  float getLat() {return m_lat;}
  float getLon() {return m_lon;}
  int getDegLat() { return (int)m_lat; }
  int getDegLon() { return (int)m_lon; }
  int getMinLat() { return static_cast<int>((m_lat - (int)m_lat) * 60.0f); }
  float getMinfLat() { return (m_lat - (int)m_lat) * 60.0f; }
  int getMinLon() { return static_cast<int>((m_lon - (int)m_lon) * 60.0f); }
  float getMinfLon() { return (m_lon - (int)m_lon) * 60.0f; }
  int getSecLat() { return static_cast<int>((m_lat - (int)m_lat - (float)getMinLat()/60.0) * 60000.0); }
  int getSecLon() { return static_cast<int>((m_lon - (int)m_lon - (float)getMinLon()/60.0) * 60000.0); }
  void setLat(float lat) {m_lat = lat; update(true);}
  void setLon(float lon) {m_lon = lon; update(true);}
  void set(float lat, float lon) {m_lat = lat; m_lon = lon; update(true);}
 private:
  float m_lat = 0;
  float m_lon = 0;
};


/**
 * @brief represents a button
 */
class cParBtn : public cParBase {
 public:
  cParBtn() {}
  const char* getText() { return m_text; }
  void setText(const char* p) { m_text = p; }
  void setIndex(int idx) { m_index = idx; }
  int getIndex() {return m_index;}
 private:
  const char* m_text = nullptr;
  int m_index = 0;
};


/**
 * @brief The stEnumItem struct one item to display an enum parameter
 */
class cListItem : public cParBase {
 public:
   cListItem() :
     m_id(0)
   {
     m_text[0] = 0;
   }
  uint16_t getId() { return m_id;  }
  void setId(uint16_t id) { m_id = id; }
  tText*  getpText() { return m_text; }
  void setText(tText* p) { strncpy(m_text, p, sizeof(m_text)); m_text[LIST_ITEM_LEN - 1] = 0; }
 private:
  uint16_t m_id;       ///< enum id
  char m_text[LIST_ITEM_LEN];      ///< list text
};

typedef my_vector<cListItem, CNT_ENUM_ITEM> tList;



/**
 * @brief The stEnumItem struct one item to display an enum parameter
 */
class cEnumItem : public cParBase {
 public:
   cEnumItem() :
     m_id(0),
     m_text(nullptr)
   {

   }
  uint16_t getId() { return m_id;  }
  void setId(uint16_t id) { m_id = id; }
  tText*  getpText() { return m_text; }
  void setText(tText* p) { m_text = p; }
 private:
  uint16_t m_id;       ///< enum id
  tText* m_text;        ///< enum text
};

typedef my_vector<cEnumItem, CNT_ENUM_ITEM> tEnum;


class cParEnum;
class cParList;

class cParText : public cParBase {
 public:
  void setText(const char* p) {m_text = p; update(true); }
  const char* getText() {return m_text; }

 private:
  const char* m_text;
};

/**
 * @ brief pointer to different items as union to save precious memory
 */
/*typedef union {
  const char* text;     ///< pt to text item (only valid for ITEM_TEXT)
  cParEnum* parEnum;   ///< pt to enum item (only valid for ITEM_ENUM)
  cParNum* parNum;     ///< pt to number item (only valid for ITEM_NR)
  cParDate* parDate;   ///< pt to date (only valid for ITEM_DATE)
  cParTime* parTime;   ///< pt to time (only valid for ITEM_TIME)
  stParGeoPos* parGeo;  ///< pt to geo position (only valid for ITEM_GEO)
  stParStr* parStr;     ///< pt to string (only valid for ITEM_STRING)
  stParBtn* parBtn;     ///< pt to button (only valid for ITEM_BUTTON)
} tPtItem;
*/
/**
 * @brief item in a panel
 */
struct stPanelItem {
  tCoord x;             ///< x coordinate [px]
  tCoord y;             ///< y coordinate [px]
  tCoord width;         ///< width [px]
  tCoord height;        ///< height [px]
  cParBase *p;          ///< pointer to items
  enItemType type;      ///< item type
  fuFocus f;            ///< function to handle special actions for item
  tCoord textSize = 1;  ///< size of text
  bool isVisible = true;
  bool isEdit = false;
};

typedef my_vector<stPanelItem, CNT_PANEL_ITEM> tItemList;

struct cPanel {
  cPanel();
  virtual ~cPanel() {}

  tCoord x;                  ///< x coordinate [px]
  tCoord y;                  ///< y coordinate [px]
  tCoord width;              ///< width [px]
  tCoord height;             ///< hegth [px]
  enPanel type;
  tItemList itemList;

//  static void setDisp(ILI9341_t3* pDisplay) { m_pDisp = pDisplay; }
  int addTextItem(const char* pText, tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit = false, fuFocus f = NULL, tCoord textSize = 1);
  int addTextItem(thText t, tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit = false, fuFocus f = NULL, tCoord textSize = 1) {
      return addTextItem(Txt::get(t), x, y, w, h, isEdit , f , textSize);
  }

  int addEnumItem(cParEnum* pPar, tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit, fuFocus f = nullptr);
  int addListItem(cParList* pPar, tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit, fuFocus f = nullptr);
  int addNumItem(cParNum* pPar, tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit, fuFocus f = nullptr);
  int addDateItem(cParDate* pPar,tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit= false, fuFocus f = nullptr);
  int addTimeItem(cParTime* pPar,tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit= false, fuFocus f = nullptr);
  int addGeoItem(cParGeoPos* pPar,tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit= false, fuFocus f = nullptr);
  int addStrItem(cParStr* pPar,tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit= false, uint16_t color = COL_TEXT, fuFocus f = nullptr);
  int addBtnItem(thText txt ,tCoord x, tCoord y, tCoord w, tCoord h, fuFocus f, int idx = 0);
  int addcParBtnGraphItem(cParGraph* pPar,tCoord x, tCoord y, tCoord w, tCoord h, fuFocus f  = nullptr);
  int addGraphItem(cParGraph* pPar,tCoord x, tCoord y, tCoord w, tCoord h, fuFocus f = nullptr);
  void refresh();
  bool isRefresh();
  void clear() { itemList.clear(); }

  /**
   * @brief addItem add an item to a panel
   * @param hPanel
   * @param item
   * @param x
   * @param y
   * @param w
   * @param h
   * @param isEdit
   * @param f
   * @return
   */
  int addItem(stPanelItem& item, tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit, fuFocus f);

  thItem findFirstEditItem();
  thItem findLastEditItem();
  thItem findNextEditItem(thItem it);
  thItem findPrevEditItem(thItem it);

  static enLang m_lang;            ///< display language
 // static ILI9341_t3* m_pDisp;
};


/**
 * parameter type enumaration
 */
class cParEnum : public cParBase {
 public:
  cParEnum() :
    m_enumeration(),
    m_val(0)
    {}
  cParEnum(uint32_t v) :
    m_enumeration(),
    m_val(v) {

  }

  int addItem(thText text) {
    cEnumItem item;
    item.setId ((uint16_t)m_enumeration.size());
    item.setText(Txt::get(text));
    return m_enumeration.push_back(item);
  }

  int addItem(const char* text) {
    cEnumItem item;
    item.setId((uint16_t)m_enumeration.size());
    item.setText(text);
    return m_enumeration.push_back(item);
  }

  size_t size() { return m_enumeration.size(); }
  size_t maxSize() { return m_enumeration.maxSize(); }
  tText* getActText() { if(m_val < m_enumeration.size()) return m_enumeration[m_val].getpText(); else return ""; }
  tText* getText(uint32_t i) { if(i < m_enumeration.size()) return m_enumeration[i].getpText(); else return ""; }
  size_t getSize() {return m_enumeration.size(); }
  void clear() { m_enumeration.clear(); m_val = 0; }

  uint32_t get() { return m_val; }
  void set(uint32_t v)
  {
    if (m_val < m_enumeration.size())
      m_val = v;
    else
      m_val = 0;
    update(true);
  }

 private:
  tEnum m_enumeration;  ///< enumeration values
  uint32_t m_val;       ///< actual value
};

/**
 * parameter type list
 */
class cParList : public cParBase {
 public:
  cParList(uint32_t v) :
    m_enumeration(),
    m_val(v) {

  }

  int addItem(thText text) {
    cListItem item;
    item.setId ((uint16_t)m_enumeration.size());
    item.setText(Txt::get(text));
    return m_enumeration.push_back(item);
  }

  int addItem(const char* text) {
    cListItem item;
    item.setId((uint16_t)m_enumeration.size());
    item.setText(text);
    return m_enumeration.push_back(item);
  }

  size_t size() { return m_enumeration.size(); }
  size_t maxSize() { return m_enumeration.maxSize(); }
  tText* getActText() { if(m_val < m_enumeration.size()) return m_enumeration[m_val].getpText(); else return ""; }
  tText* getText(uint32_t i) { if(i < m_enumeration.size()) return m_enumeration[i].getpText(); else return ""; }
  size_t getSize() {return m_enumeration.size(); }
  void clear() { m_enumeration.clear(); m_val = 0; }

  uint32_t get() { return m_val; }
  void set(uint32_t v)
  {
    if (m_val < m_enumeration.size())
      m_val = v;
    else
      m_val = 0;
    update(true);
  }

 private:
  tList m_enumeration;  ///< list values
  uint32_t m_val;       ///< actual value
};


#endif // CPANEL_H

/*************************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * A generic menue system 
 * TODO: explantion 
 * ***********************************************************/
#ifndef CMENUEYSTEM_H
#define CMENUEYSTEM_H

#include <stdint.h>
#include <stdlib.h>
#include "ctext.h"
#include <cstring>

#include "cpanel.h"

class Adafruit_GFX;


enum enStatFocus {
  FOC_DISPLAY = 1,           ///< item in focus is displayed
  FOC_SELECT  = 2,           ///< item in focus is selected for editing
  FOC_EDIT    = 3,           ///< item in focus is currently edited
};

enum enFocusState {
  DISP    = 1,
  SELECT  = 2,
  EDIT    = 3
};

enum enMsg {
  INFO    = 1,
  YESNO   = 2,
};

typedef my_vector<cPanel, CNT_PANEL_LIST> tPanelList;

struct stFocus {
  thPanel panel;
  thItem item;
  enFocusState state;
};


/**
 * @brief realization of a simple menue system
 */
class cMenuesystem
{
public:
    /**
   * @brief cMenuesystem constructor
   * @param width width of the used display [px]
   * @param height height of the used display [px]
   * @param pDisplay pointer to base class of display
   */
  cMenuesystem(int width, int height, Adafruit_GFX* pDisplay);
  virtual ~cMenuesystem();
  virtual void initDialogs() = 0;
  virtual void save() = 0;
  virtual void load() = 0;
  void setPdisplay(int width, int height, Adafruit_GFX* pDisplay, int lineHeight, 
                   int nrFkeys, const stColors* pColors, int fontOffset);

  /**
   * @brief initialize menue system
   */
  void init(bool withDialogs = true);

  /**
   * @brief handle a keystroke and pass it to the item in focus
   * @param key
   * @return
   */
  int32_t handleKey(enKey key);

  /**
   * @brief create a panel
   * @param type
   * @param x x position [px]
   * @param y y position [px]
   * @param width width
   * @param height height
   * @return id of panel
   */
  thPanel createPanel(enPanel type, tCoord x, tCoord y, tCoord width, tCoord height);

  void refreshMainPanel();
  void refreshFkeyPanel();
  void refreshHdrPanel();

  void refreshAll();

  /**
   * @brief show modal dialog
   * @param dialog type
   * @param f callback function
   * @param str   display message
   * @param str2  display message 2nd line (optional)
   * @param str3  display message 3rd line (optional)
   * @param str4  display message 4th line (optional)
   */
  void showMsg(enMsg type, fuFocus f, const char* str, const char* str2 = NULL, const char* str3 = NULL, const char* str4 = NULL);

  /**
   * @brief destroy message if displayed
   */
  void destroyMsg();

  /**
   * @brief create drop down menue
   * @param list
   * @param x
   * @param y
   * @param width
   * @param height
   * @param f
   */
  thPanel createDropDown(stPanelItem item, tCoord x, tCoord y, tCoord width, fuFocus f);

  void setMainPanel(thPanel pan);

  /**
   * @brief get pointer to panel
   * @param i handle to item
   * @return pointer to panel, NULL in case of invalid handle
   */
  cPanel* getPan(thPanel i) { if (i < m_panelList.size()) return &m_panelList[i]; else return NULL;}
  thPanel getMainPanel() { return m_mainPanel; }
  void setFkeyPanel(thPanel pan) { m_fKeyPanel = pan; refreshFkeyPanel(); }
  thPanel getFkeyPanel() { return m_fKeyPanel; }
  void setHdrPanel(thPanel pan) { m_hHeadrPanel = pan; refreshHdrPanel(); }
  thPanel getHdrPanel() { return m_hHeadrPanel; }
  thPanel getFocusPanel() {return m_focus.panel; }
  thItem getFocusItem() { return m_focus.item; }
  enFocusState getFocusState() { return m_focus.state; }
  bool isDropDownInFocus() { return m_dropDownPan == m_focus.panel; }
  bool keyPauseLongEnough(uint32_t ms = 200);
  uint32_t getTimeAfterKeyPress();
  void resetTimer();
  void clearPanelList() { m_panelList.clear(); }
  void initFocusOnPanel(cPanel* pan) {m_focus.item = pan->findFirstEditItem(); m_focus.state = enFocusState::DISP; };
  virtual void setFactoryDefaults() {}
  int getFkeypanHeight() { return m_lineHeight + 2; }  ///< Height of FKEY-Panel
  int getHdrHeight() { return m_lineHeight + 1; }  ///< height of header panel
  int getWidth() { return m_width; }
  int getHeight() { return m_height; }
 

protected:
/**
   * @brief set focus
   * @param pan handle to focus panel
   * @param item handle to focus item
   * @param state target state for focus item
   */
  void setFocus(thPanel pan, thItem item, enFocusState state);
  
  void destroyDropDown();

  virtual void initPars() = 0;

  /**
   * @brief callback function to set selected enum itm from PNL_DROPDOWN
   * @param pThis
   * @param key
   * @return
   */
  static void setEnumDropDown(cMenuesystem* pThis, enKey key, cParBase* pItem);

  /**
   * @brief callback function to set selected enum itm from PNL_DROPDOWN
   * @param pThis
   * @param key
   * @return
   */
  static void setListDropDown(cMenuesystem* pThis, enKey key, cParBase* pItem);
  
  /**
   * draw grid for graph
   */
  void drawGrid(stPanelItem* p);

  void printText(const char* txt, stPanelItem& item);
  void printText(const char* txt, int16_t x, int16_t y, int16_t w, int16_t h);
  /**
   *  refresh the display if something has changed 
   */
  virtual void refreshDisplay() = 0;   

private:
  static void msgYesFunc(cMenuesystem* pThis, enKey key, cParBase* pItem);
  static void msgNoFunc(cMenuesystem* pThis, enKey key, cParBase* pItem);

  /**
   * @brief draw complete panel consisting of header main-panel and FKEY bar
   */
  bool drawPanels();

  /**
   * @brief draw a sub panel (hdr, main, fkey)
   * @param pan
   * @param hPanel
   */
  bool drawSubPanel(cPanel* pan, thPanel hPanel);

  /**
   * @brief draw a single item of a 
   * @param item reference to panel item
   * @param hPanel handle to the panel the item consists to
   * @param itemId id if the item
   */
  bool drawItem( stPanelItem& item, thPanel hPanel, thItem itemId, enPanel panType);

  void handleEditMode(cPanel& pan, enKey key);
  void editPar(stPanelItem& item, enKey key);
  void reInitDropDownItems();

  stFocus m_focus;                 ///< focus for cursor
  stFocus m_focusSaveDrop;         ///< save of focus for dropdown
  stFocus m_focusSaveMsg;          ///< save of focus for Msg
  thPanel m_hHeadrPanel = 9999;    ///< handle to header panel
  thPanel m_mainPanel = 9999;      ///< handle to main panel
  thPanel m_fKeyPanel = 9999;      ///< handle to fKey panel
  thPanel m_MsgPan = 9999;         ///< handle to Message panel
  int m_width;                     ///< widht of panel in px
  int m_height;                    ///< height of panel in px
  tPanelList m_panelList;          ///< list of all panels

  bool m_refreshMain = true;
  bool m_refreshFkey = true;
  bool m_refreshHdr = true;
  bool m_msgActive = false;
  cParBtn* m_pMsgOkBtn = NULL;
  fuFocus m_msgCallBack = NULL;

  thPanel m_dropDownPan;          ///< handle to drop down panel
  thItem m_dropDownMaxSize;       ///< max. nr of items in drop down menu (geometrical constraint)
  stPanelItem m_pDropDownEnum;    ///< enumeration to define dropdown panel
  thItem m_firstDropDownItem;     ///< first item actually displayed in drop down menue
  uint32_t m_lastKeyTime = 0;     ///< last time key was pressed
  bool m_isInitialized = false;   ///< true if menue system is initialized
  int m_lineHeight = 8;           ///< distance between 2 text lines
  uint32_t m_nrFkeys = 2;         ///< nr of Fkeys
  int m_fontOffset = 0;           ///< offset in Y pixels for setting cursor to print character
};


#endif // CMENUEYSTEM_H

#include <stdint.h>
#include "MenueSystem.h"
#include "cutils.h"
//#include "errors.h"
#include <ILI9341_t3.h>
#include <Arduino.h>

ILI9341_t3* gpDisplay;

cMenuesystem::cMenuesystem(int width, int height, ILI9341_t3* pDisplay) :
    m_width(width),
    m_height(height),
    m_panelList()
 {
    m_focus.item = 0;
    m_focus.panel = 0;
    m_focus.state = FST_DISP;
    gpDisplay = pDisplay;
}

cMenuesystem::~cMenuesystem() {
  for(uint i = 0; i < m_panelList.size(); i++) {
     m_panelList[i].itemList.clear();
  }
  m_panelList.clear();
}

thPanel cMenuesystem::createPanel(enPanel type, tCoord x, tCoord y, tCoord width, tCoord height) {
  thPanel i;
  cPanel item;
  item.x = x;
  item.y = y;
  item.width = width;
  item.height = height;
  item.type = type;
  i = m_panelList.size();
  m_panelList.push_back(item);
  return i;
}


void cMenuesystem::drawItem( stPanelItem& item, thPanel hPanel, uint32_t itemId, enPanel panType) {
    char str[32];
    char fmt[6];
    char lon, lat;
    uint16_t colTxt, colTxtBack;
    if(!item.isVisible)
      return;
    if(!item.p->isUpdate())
      return;
    switch(panType) {
      case PNL_HEADER:
      case PNL_FKEYS:
        colTxt =COL_TEXTHDR;
        colTxtBack = COL_TEXTHDRBACK;
        break;
      default:
        colTxt =COL_TEXT;
        colTxtBack = COL_TEXTBACK;
        break;
    }
    gpDisplay->setTextColor(colTxt, colTxtBack);

    if((m_focus.panel == hPanel) && (m_focus.item == itemId)) {
      if (m_focus.state == FST_SELECT) {
        gpDisplay->setTextColor(COL_TEXTSEL, COL_TEXTSELBACK);
        gpDisplay->fillRect(item.x, item.y, item.width, item.height, COL_TEXTSELBACK);
      }
      else if (m_focus.state == FST_EDIT) {
        gpDisplay->setTextColor(COL_TEXTEDIT, COL_TEXTEDITBACK);
        gpDisplay->fillRect(item.x, item.y, item.width, item.height, COL_TEXTEDITBACK);
      }
    }
    else {
      if((item.type != ITEM_GRAPH))
        gpDisplay->fillRect(item.x, item.y, item.width, item.height, colTxtBack);
      else {
        cParGraph* p = reinterpret_cast<cParGraph*>(item.p);
        if (p->getInitPlot())
          gpDisplay->fillRect(item.x, item.y, item.width, item.height, colTxtBack);
      }

    }
    gpDisplay->setCursor(item.x + 1, item.y + 1);
    gpDisplay->setTextSize(item.textSize);

    item.p->update(false);
    switch (item.type) {
      case ITEM_TEXT: {
          cParText* p = reinterpret_cast<cParText*>(item.p);
          if(p)
            printText(p->getText());
        }
        break;

      case ITEM_ENUM: {
          cParEnum* p = reinterpret_cast<cParEnum*>(item.p);
         // gpDisplay->fillRect(item.x, item.y, item.width, item.height, COL_TEXTBACK);
          printText(p->getActText());
        }
        break;

      case ITEM_LIST: {
          cParList* p = reinterpret_cast<cParList*>(item.p);
         // gpDisplay->fillRect(item.x, item.y, item.width, item.height, COL_TEXTBACK);
          printText(p->getActText());
        }
        break;


      case ITEM_NR: {
          cParNum* p = reinterpret_cast<cParNum*>(item.p);
          if(p->getDecimals() < 10) {
            snprintf(fmt, sizeof(fmt), "%%.%luf",p->getDecimals());
            snprintf(str, sizeof(str), fmt,p->get());
            printText(str);
          }
        }
        break;

      case ITEM_DATE: {
          cParDate* p = reinterpret_cast<cParDate*>(item.p);
          unsigned int y = p->getYear();
          if(y > 2000)
            y -= 2000;
          if(y > 100)
            y-= 100;
          snprintf(str, sizeof(str), "%02u.%02u.%02u", p->getDay(), p->getMonth() + 1, y);
          printText(str);
        }
        break;

     case ITEM_TIME: {
          cParTime* p = reinterpret_cast<cParTime*>(item.p);
          snprintf(str, sizeof(str), "%02u:%02u:%02u", p->getHour(), p->getMin(), p->getSec());
          printText(str);
        }
        break;

     case ITEM_GEO: {
          cParGeoPos* p = reinterpret_cast<cParGeoPos*>(item.p);
          if(p->getDegLat() >= 0)
            lat = 'N';
          else
            lat = 'S';
          if(p->getDegLong() >= 0)
            lon = 'E';
          else
            lon = 'W';
          snprintf(str, sizeof(str), "%c %02i\xf7 %.3f  %c %03i\xf7 %.3f",
                   lat, p->getDegLat(), p->getMinLat(),
                   lon, p->getDegLong(), p->getMinLong());
          printText(str);
        }
        break;

     case ITEM_STRING: {
          cParStr* p = reinterpret_cast<cParStr*>(item.p);
          printText(p->get());
        }
        break;

     case ITEM_BUTTON: {
          cParBtn* p = reinterpret_cast<cParBtn*>(item.p);
          printText(p->getText());
          gpDisplay->drawRect(item.x - 1, item.y - 1, item.width + 2, item.height + 2, COL_TEXT);
          gpDisplay->fillRect(item.x + item.width + 1, item.y + 2, 2, item.height + 1, COL_MSGSHADOW);
          gpDisplay->fillRect(item.x + 2, item.y + item.height + 1, item.width + 1, 2, COL_MSGSHADOW);
        }
        break;

     case ITEM_GRAPH: {
          cParGraph* p = reinterpret_cast<cParGraph*>(item.p);
          p->initDiagram();
          if(item.f)
            item.f(this, DEV_KEY_NO);
          p->plotGraph();
        }
        break;

     default:
        break;
    }

}

void cMenuesystem::drawSubPanel(cPanel& pan, thPanel hPanel) {
  size_t size = pan.itemList.size();
  switch(pan.type) {
    case PNL_FKEYS:
      if(m_refreshFkey) {
        gpDisplay->drawLine(pan.x, pan.y, m_width - 1, pan.y, COL_TEXT);
        for(uint32_t i = 1; i < 4; i++) {
           int x = m_width * i / 4;
           gpDisplay->drawLine(x, pan.y, x, m_height - 1, COL_TEXT);
        }
      }
      break;
    case PNL_HEADER:
      gpDisplay->drawLine(pan.x, pan.height-1, m_width - 1, pan.height-1, COL_TEXT);
      break;

    case PNL_DROPDOWN:
      if(pan.isRefresh()) {
        pan.refresh();
        gpDisplay->fillRect(pan.x, pan.y, pan.width, pan.height, COL_TEXTBACK);
        gpDisplay->drawRect(pan.x, pan.y, pan.width, pan.height, COL_TEXT);
      }
      break;

    case PNL_MAIN:
    case PNL_PARLIST:
      break;
    case PNL_MESSAGE:
      if(pan.isRefresh()) {
        pan.refresh();
        gpDisplay->fillRect(pan.x, pan.y, pan.width, 15, COL_TEXTHDRBACK);
        gpDisplay->setCursor(pan.x + pan.width/2 - 40, pan.y + 4);
        gpDisplay->setTextColor(COL_TEXTHDR, COL_TEXTHDRBACK);
        printText(Txt::get(11));
        gpDisplay->fillRect(pan.x, pan.y + 15, pan.width, pan.height, COL_TEXTBACK);
        gpDisplay->drawRect(pan.x, pan.y, pan.width, pan.height, COL_TEXT);
        gpDisplay->fillRect(pan.x + pan.width, pan.y + 3, 3, pan.height, COL_MSGSHADOW);
        gpDisplay->fillRect(pan.x + 3, pan.y + pan.height, pan.width, 3, COL_MSGSHADOW);
      }
      break;
  }
  for(uint32_t i = 0; i < size; i++) {
     drawItem(pan.itemList[i], hPanel, i, pan.type);
  }
}

void cMenuesystem::drawPanels() {
  // draw header panel
  if(m_hHeadrPanel < m_panelList.size()) {
    if(m_refreshHdr) {
      m_refreshHdr = false;
      cPanel& pan = m_panelList[m_hHeadrPanel];
      gpDisplay->fillRect(pan.x,pan.y,pan.width, pan.height, COL_TEXTHDRBACK);
      drawSubPanel(pan, m_fKeyPanel);

    }
  }

  // draw fKey panel
  if(m_fKeyPanel < m_panelList.size()) {
    if(m_refreshFkey) {
      cPanel& pan = m_panelList[m_fKeyPanel];
      gpDisplay->fillRect(pan.x,pan.y,pan.width, pan.height, COL_TEXTHDRBACK);
      drawSubPanel(pan, m_fKeyPanel);
      m_refreshFkey = false;
    }
  }
  // draw main panel
  if(m_refreshMain) {
    m_refreshMain = false;
    cPanel& p = m_panelList[m_mainPanel];
    gpDisplay->fillRect(p.x, p.y, p.width, p.height, COL_TEXTBACK);
  }

  drawSubPanel(m_panelList[m_mainPanel], m_mainPanel);

  // draw focus panel
  if ((m_focus.panel < m_panelList.size() ) && (m_focus.panel != m_hHeadrPanel)){
    cPanel& pan = m_panelList[m_focus.panel];
    drawSubPanel(pan, m_focus.panel);
  }

}


void cMenuesystem::init() {
   initPars();
   initDialogs();
   drawPanels();
   refreshAll();
}

void cMenuesystem::setMainPanel(thPanel pan) {
  m_mainPanel = pan;
  m_refreshMain = true;
  setFocus(pan, 0, FST_DISP);
  m_focusSaveMsg = m_focus;
  m_focusSaveDrop = m_focus;
}

int32_t cMenuesystem::handleKey(tKey key) {
  int32_t retVal = 0;
  cPanel* pan;
  // check if F-Key was pressed
  if (m_fKeyPanel < m_panelList.size()) {
    bool pressed = false;
    pan = &m_panelList[m_fKeyPanel];
    switch(key) {
      case DEV_KEY_F1:
        pressed = true;
        if(pan->itemList[0].f)
          pan->itemList[0].f(this, key);
        break;

      case DEV_KEY_F2:
        pressed = true;
        if(pan->itemList[1].f)
          pan->itemList[1].f(this, key);
        break;

     case DEV_KEY_F3:
        pressed = true;
       if(pan->itemList[2].f)
          pan->itemList[2].f(this, key);
        break;

     case DEV_KEY_F4:
        pressed = true;
       if(pan->itemList[3].f)
          pan->itemList[3].f(this, key);
       break;

     case DEV_KEY_NOKEY:
       return 0;
    }
    if (pressed) {
#ifndef SIMU_DISPLAY
      m_lastKeyTime = millis();
#endif
      drawPanels();
      return retVal;
    }
  }

  // check key strokes for focus panel
#ifndef SIMU_DISPLAY
  if(key != DEV_KEY_TICK)
    m_lastKeyTime = millis();
#endif
  if (m_focus.panel < m_panelList.size()) {
    pan = &m_panelList[m_focus.panel];
    handleEditMode(*pan, key);
  }
  drawPanels();
  return retVal;
}

void cMenuesystem::resetTimer() {
#ifndef SIMU_DISPLAY
  m_lastKeyTime = millis();
#endif
}

void cMenuesystem::setEnumDropDown(cMenuesystem* pThis, tKey key) {
  cParEnum* p = reinterpret_cast<cParEnum*>(pThis->m_pDropDownEnum.p);
  uint32_t val = pThis->getFocusItem() + pThis->m_firstDropDownItem;
  //  Serial.printf("set enum to %u\n",val);
  p->set(val);

  if (pThis->m_pDropDownEnum.f)
    pThis->m_pDropDownEnum.f(pThis, key);  
}


void cMenuesystem::setListDropDown(cMenuesystem* pThis, tKey key) {
  cParList* p = reinterpret_cast<cParList*>(pThis->m_pDropDownEnum.p);
  uint32_t val = pThis->getFocusItem() + pThis->m_firstDropDownItem;
  //  Serial.printf("set enum to %u\n",val);
  p->set(val);

  if (pThis->m_pDropDownEnum.f)
    pThis->m_pDropDownEnum.f(pThis, key);  
}


void cMenuesystem::handleEditMode(cPanel& pan, tKey key) {
  stPanelItem& item = pan.itemList[m_focus.item];

  cParEnum* pEnum = NULL;
  if (pan.type == PNL_DROPDOWN)
    pEnum = reinterpret_cast<cParEnum*>(m_pDropDownEnum.p);

  switch(key) {
    case DEV_KEY_OK:
      switch(m_focus.state) {
        case FST_DISP:
          m_focus.item = pan.findFirstEditItem();
          if(m_focus.item < pan.itemList.size()) {
            m_focus.state = FST_SELECT;
            pan.itemList[m_focus.item].p->update(true);
          }
          break;

        case FST_EDIT:
          if((item.type == ITEM_BUTTON) && item.f)
            item.f(this, key);
          m_focus.state = FST_DISP;
          gpDisplay->fillRect(item.x, item.y, item.width, item.height, COL_TEXTBACK);
          pan.itemList[m_focus.item].p->update(true);
          if(pan.type == PNL_MESSAGE)
            destroyMsg();
          break;

        case FST_SELECT:
          if(pan.type == PNL_FKEYS) {
            refreshFkeyPanel();
            if(pan.itemList[m_focus.item].f)
              pan.itemList[m_focus.item].f(this, key);
          }
          else if (pan.type == PNL_DROPDOWN) {
            gpDisplay->fillRect(pan.x, pan.y, pan.width, pan.height, COL_TEXTBACK);
            if(item.f) {
              item.f(this, key);
            } 
            destroyDropDown();
          }
          else if (item.type == ITEM_ENUM) {
            if(item.f) {
              item.f(this, key);
            }
            createDropDown(item, item.x, item.y, item.width, setEnumDropDown);
          }
          else if (item.type == ITEM_LIST) {
            if(item.f) {
              item.f(this, key);
            }
            createDropDown(item, item.x, item.y, item.width, setListDropDown);
          }
          else
            m_focus.state = FST_EDIT;
          pan.itemList[m_focus.item].p->update(true);
          break;

        default:
          m_focus.state = FST_DISP;
          break;
      }
      break;

    case DEV_KEY_UP:
      switch(m_focus.state) {
        case FST_DISP:
//          m_focus.item = pan.findFirstEditItem();
          m_focus.state = FST_SELECT;
          // fall through intended

        case FST_SELECT:
          gpDisplay->fillRect(item.x, item.y, item.width, item.height, COL_TEXTBACK);
          pan.itemList[m_focus.item].p->update(true);
          if (pan.type != PNL_DROPDOWN) {
            m_focus.item = pan.findPrevEditItem(m_focus.item);
            if (m_focus.item > pan.itemList.size()) {
              if(pan.type == PNL_MESSAGE)
                m_focus.item = pan.findLastEditItem();
              else {
                refreshMainPanel();
                refreshFkeyPanel();
                if(m_focus.panel == m_fKeyPanel)
                  m_focus.panel = m_mainPanel;
                else
                  m_focus.panel = m_fKeyPanel;
                m_focus.item = m_panelList[m_focus.panel].findLastEditItem();
              }
            }
          }
          else {
            if(m_focus.item <= 1) {
              if(m_firstDropDownItem > 0)
                m_firstDropDownItem--;
              else {
                if(m_focus.item > 0)
                  m_focus.item--;
                else {
                  m_firstDropDownItem = pEnum->size() - pan.itemList.size();
                  m_focus.item = pan.itemList.size() - 1;
                }
              }
              reInitDropDownItems();
            }
            else
              m_focus.item--;
          }
          pan.itemList[m_focus.item].p->update(true);
          break;

      case FST_EDIT:
          editPar(item, key);
          break;
      }
      break;

    case DEV_KEY_DOWN:
      switch(m_focus.state) {
        case FST_DISP:
       //   m_focus.item = pan.findFirstEditItem();
          m_focus.state = FST_SELECT;
          // fall through intended

        case FST_SELECT:
          gpDisplay->fillRect(item.x, item.y, item.width, item.height, COL_TEXTBACK);
          pan.itemList[m_focus.item].p->update(true);
          if(pan.type != PNL_DROPDOWN) {
            m_focus.item = pan.findNextEditItem(m_focus.item);
            if (m_focus.item > pan.itemList.size()) {
              if(pan.type == PNL_MESSAGE)
                m_focus.item = pan.findFirstEditItem();
              else {
                refreshMainPanel();
                refreshFkeyPanel();
                if(m_focus.panel == m_fKeyPanel)
                  m_focus.panel = m_mainPanel;
                else
                  m_focus.panel = m_fKeyPanel;
                m_focus.item = m_panelList[m_focus.panel].findFirstEditItem();
              }
            }
          }
          else {
            if(m_focus.item < (pan.itemList.size() - 1))
              m_focus.item++;
            else {
              m_firstDropDownItem++;
              if(m_firstDropDownItem > (pEnum->size() - pan.itemList.size())) {
                 m_firstDropDownItem = 0;
                 m_focus.item = 0;
              }
              reInitDropDownItems();
            }
          }
          pan.itemList[m_focus.item].p->update(true);
          break;

        case FST_EDIT:
          editPar(item, key);
          break;
      }
      break;
  }
}

void cMenuesystem::editPar(stPanelItem &item, tKey key) {
    switch (item.type) {
      case ITEM_ENUM:
        break;

      case ITEM_NR: {
          cParNum* p = reinterpret_cast<cParNum*>(item.p);
          if(key == DEV_KEY_DOWN) {
            p->set(p->get() + p->getStep());
            if(p->get() > p->getMax())
              p->set(p->getMax());
          }
          if(key == DEV_KEY_UP) {
            p->set(p->get() - p->getStep());
            if(p->get() < p->getMin())
              p->set(p->getMin());
          }
        }
        break;

    case ITEM_TEXT:
    default:
        break;
    }
    if(item.f)
      item.f(this, key);
}

void cMenuesystem::reInitDropDownItems() {
  cPanel& pan = m_panelList[m_dropDownPan];
  size_t s = pan.itemList.size() - 1;
  uint firstItem = 0;
  if(m_firstDropDownItem > 0) {
    firstItem = 1;
    cParText* p = reinterpret_cast<cParText*>(pan.itemList[0].p);
    p->setText("    \x1E");
  }
  cParEnum* pDr = reinterpret_cast<cParEnum*>(m_pDropDownEnum.p);
  for (uint i = firstItem; i < s; i++) {
    cParText* p = reinterpret_cast<cParText*>(pan.itemList[i].p);
    p->setText(pDr->getText(i + m_firstDropDownItem));
  }

  cParText* p1 = reinterpret_cast<cParText*>(pan.itemList[s].p);
  if((s + m_firstDropDownItem) < (pDr->size() - 1))
    p1->setText("    \x1F");

  if((s + m_firstDropDownItem) == (pDr->size() - 1))
    p1->setText(pDr->getText(s + m_firstDropDownItem));
}


void msgFunc(cMenuesystem* pThis, tKey key) {
   pThis->destroyMsg();
}


void cMenuesystem::msgYesFunc(cMenuesystem* pThis, tKey key) {
  if(pThis->m_msgCallBack)
    pThis->m_msgCallBack(pThis, DEV_KEY_YES);
}


void cMenuesystem::msgNoFunc(cMenuesystem* pThis, tKey key) {
    if(pThis->m_msgCallBack)
      pThis->m_msgCallBack(pThis, DEV_KEY_YES);
}


void cMenuesystem::showMsg(enMsg type, fuFocus f, const char *str, const char* str2,
                                                  const char* str3, const char* str4) {
  m_MsgPan = createPanel(PNL_MESSAGE, 30, 30, 260, 50);

  cParStr* parStr = new cParStr(str);
  tCoord x = 40;
  tCoord y = 60;
  tCoord w = 240;
  m_msgCallBack = f;
  m_panelList[m_MsgPan].addStrItem(parStr, x, y, w, 12);
  if(str2) {
    y += 12;
    parStr = new cParStr(str2);
    m_panelList[m_MsgPan].addStrItem(parStr, x, y, w, 12);
    if(str3) {
      y += 12;
      parStr = new cParStr(str3);
      m_panelList[m_MsgPan].addStrItem(parStr, x, y, w, 12);
      if(str4) {
        y += 12;
        parStr = new cParStr(str4);
        m_panelList[m_MsgPan].addStrItem(parStr, x, y, w, 12);
      }
    }
  }
  y += 20;
  thItem btn = m_panelList[m_MsgPan].itemList.size();
  switch (type) {
  case MSG_INFO: {
      cParBtn* pMsgOkBtn = new cParBtn(Txt::get(10));
      m_panelList[m_MsgPan].addBtnItem(pMsgOkBtn, 130, y, 60, 14, msgFunc);
      setFocus(m_MsgPan,btn,FST_EDIT);
      }
      break;
  case MSG_YESNO: {
      cParBtn* pMsgYesBtn = new cParBtn(Txt::get(15));
      cParBtn* pMsgNoBtn = new cParBtn(Txt::get(16));
      m_panelList[m_MsgPan].addBtnItem(pMsgYesBtn, 60, y, 60, 14, msgYesFunc);
      m_panelList[m_MsgPan].addBtnItem(pMsgNoBtn, 180, y, 60, 14, msgNoFunc);
      setFocus(m_MsgPan,btn,FST_SELECT);
      }
      break;
  }
  m_panelList[m_MsgPan].height = y - m_panelList[m_MsgPan].y + 14 + 10;
  m_focusSaveMsg = m_focus;
}


void cMenuesystem::destroyMsg() {
  if (m_MsgPan < m_panelList.size()) {
    size_t s = m_panelList[m_MsgPan].itemList.size();
    for (size_t i = 0; i < s; i++) {
      if(m_panelList[m_MsgPan].itemList[i].type == ITEM_STRING) {
         delete m_panelList[m_MsgPan].itemList[i].p;
         m_panelList[m_MsgPan].itemList[i].p = NULL;
      }
      if(m_panelList[m_MsgPan].itemList[i].type == ITEM_BUTTON) {
         delete m_panelList[m_MsgPan].itemList[i].p;
         m_panelList[m_MsgPan].itemList[i].p = NULL;
      }
    }
    m_panelList[m_MsgPan].itemList.clear();
    m_panelList.pop_back();
    m_MsgPan = 9999;
    m_focus = m_focusSaveMsg;
    m_msgCallBack = NULL;
    refreshMainPanel();
  }
}



thPanel cMenuesystem::createDropDown(stPanelItem item, tCoord x, tCoord y, tCoord width, fuFocus f) {

  cParList* pL = NULL;
  cParEnum* pE = NULL;
  size_t s = 0;

  if(item.type == ITEM_ENUM)
  {
    pE = reinterpret_cast<cParEnum*>(item.p);
    s = pE->size();
  }
  if(item.type == ITEM_LIST)
  {
    pL = reinterpret_cast<cParList*>(item.p);
    s = pL->size();
  }

  if(s > 0) {
//    Serial.printf("Dropdown, size: %u\n", s);
    m_pDropDownEnum = item;
    m_dropDownMaxSize = (m_height - FKEYPAN_HEIGHT - y) / LINE_HEIGHT;
    m_firstDropDownItem = 0;
    bool shorted = false;
    if(m_dropDownMaxSize < s) {
      shorted = true;
      s = m_dropDownMaxSize - 1;
      m_dropDownPan = createPanel(PNL_DROPDOWN, x - 1, y - 1, width + 2, (s + 1) * LINE_HEIGHT + 2);
    }
    else
      m_dropDownPan = createPanel(PNL_DROPDOWN, x - 1, y - 1, width + 2, s * LINE_HEIGHT + 2);
    tCoord yi = y;

    for(size_t i = 0; i < s; i++) {
      if(item.type == ITEM_ENUM)
        m_panelList[m_dropDownPan].addTextItem(pE->getText(i), x,yi,width, LINE_HEIGHT, true, f);
      else if(item.type == ITEM_LIST)
        m_panelList[m_dropDownPan].addTextItem(pL->getText(i), x,yi,width, LINE_HEIGHT, true, f);
      yi += LINE_HEIGHT;
    }

    if(shorted) {
      m_panelList[m_dropDownPan].addTextItem("   \x1f", x,yi,width, LINE_HEIGHT, true, f);
    }

    m_focusSaveDrop = m_focus;
    setFocus(m_dropDownPan,0,FST_SELECT);
    return m_dropDownPan;
  }
  else
    return 9999;
}

void cMenuesystem::destroyDropDown() {
  if (m_dropDownPan < m_panelList.size()) {
    m_panelList[m_dropDownPan].itemList.clear();
    m_panelList.erase(m_panelList.begin() + m_dropDownPan);
    if((m_MsgPan <= m_panelList.size()) && (m_MsgPan > m_dropDownPan)) {
      m_MsgPan--;      
      m_focus.panel = m_MsgPan;
      m_focusSaveMsg = m_focusSaveDrop;
    }
    else
      m_focus = m_focusSaveDrop;
    m_dropDownPan = 9999;
    refreshMainPanel();
  }
}

void cMenuesystem::refreshMainPanel() {
  m_refreshMain = true;
  if(m_mainPanel < m_panelList.size())
    m_panelList[m_mainPanel].refresh();
}

void cMenuesystem::refreshFkeyPanel() {
  m_refreshFkey = true;
  if(m_fKeyPanel < m_panelList.size())
    m_panelList[m_fKeyPanel].refresh();
}

void cMenuesystem::refreshHdrPanel() {
  m_refreshHdr = true;
  m_panelList[m_hHeadrPanel].refresh();
}

void cMenuesystem::refreshAll() {
  refreshFkeyPanel();
  refreshHdrPanel();
  refreshMainPanel();
  drawPanels();
}

uint32_t cMenuesystem::getTimeAfterKeyPress() {
  uint32_t actTim;
#ifndef SIMU_DISPLAY
  actTim = millis();
#else
  actTim = m_lastKeyTime + 500;
#endif
  uint32_t delta;
  if (actTim < m_lastKeyTime)
    delta = actTim + (0xFFFFFFFF - m_lastKeyTime);
  else
    delta = actTim - m_lastKeyTime;
  return delta;
}

bool cMenuesystem::keyPauseLongEnough(uint32_t ms) {
  return getTimeAfterKeyPress() > ms;
}


void cMenuesystem::printText(const char *txt) {
  char buf[512];

  cUtils::replaceUTF8withInternalCoding(txt, buf, sizeof(buf));
  gpDisplay->print(buf);
}

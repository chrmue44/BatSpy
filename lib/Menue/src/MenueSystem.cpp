/*************************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/
#include <stdint.h>
#include "MenueSystem.h"
#include "cutils.h"
//#include <ILI9341_t3.h>
#include "Adafruit_GFX.h"
#include <Arduino.h>

//#define DEBUG_LEVEL 1
#include <debug.h>

Adafruit_GFX* gpDisplay;

cMenuesystem::cMenuesystem(int width, int height, Adafruit_GFX* pDisplay) :
    m_width(width),
    m_height(height),
    m_panelList()
 {
    m_focus.item = 0;
    m_focus.panel = 0;
    m_focus.state = enFocusState::DISP;
    gpDisplay = pDisplay;
}

cMenuesystem::~cMenuesystem() 
{
  for(uint32_t i = 0; i < m_panelList.size(); i++) 
  {
     m_panelList[i].itemList.clear();
  }
  m_panelList.clear();
}

void cMenuesystem::setPdisplay(int width, int height, Adafruit_GFX* pDisplay, int lineHeight, 
                               int nrFkeys, const stColors* colors, int fontOffset)
{
  gpDisplay = pDisplay;
  m_width = width;
  m_height = height;
  m_lineHeight = lineHeight;
  m_nrFkeys = nrFkeys;
  pColors = colors;
  m_fontOffset = fontOffset;
}

const stColors* pColors = nullptr;   ///< set of colors


thPanel MMEM cMenuesystem::createPanel(enPanel type, tCoord x, tCoord y, tCoord width, tCoord height)
{
  thPanel i;
  cPanel item;
  item.x = x;
  item.y = y;
  item.width = width;
  item.height = height;
  item.type = type;
  i = (thPanel)m_panelList.size();
  m_panelList.push_back(item);
  return i;
}


bool MMEM cMenuesystem::drawItem( stPanelItem& item, thPanel hPanel, uint32_t itemId, enPanel panType)
{
    char str[32];
    char fmt[12];
    char lon, lat;
    uint16_t colTxt, colTxtBack;
    bool retVal = false;
    if(!item.isVisible)
    {
      gpDisplay->fillRect(item.x, item.y, item.width, item.height, pColors->textBack);
      return true;
    }
    if(!item.p->isUpdate())
      return false;
    switch(panType) {
      case PNL_HEADER:
      case PNL_FKEYS:
        colTxt = pColors->textHdr;
        colTxtBack = pColors->textHdrBack;
        break;
    case PNL_DROPDOWN:
        colTxt = pColors->text;;
        colTxtBack = pColors->textDropBack;
        break;
      default:
        if(item.isEdit)
          colTxt = pColors->textPar;
        else
          colTxt = pColors->text;
        colTxtBack = pColors->textBack;
        break;
    }
    gpDisplay->setTextColor(colTxt, colTxtBack);

    if((m_focus.panel == hPanel) && (m_focus.item == itemId))
    {
      if (m_focus.state == enFocusState::SELECT)
      {
        gpDisplay->setTextColor(pColors->textSel, pColors->textSelBack);
        gpDisplay->fillRect(item.x, item.y, item.width, item.height, pColors->textSelBack);
        retVal = true;
      }
      else if (m_focus.state == enFocusState::EDIT)
      {
        gpDisplay->setTextColor(pColors->textEdit, pColors->textEditBack);
        gpDisplay->fillRect(item.x, item.y, item.width, item.height, pColors->textEditBack);
        retVal = true;
      }
    }
    else
    {
      if((item.type != ITEM_GRAPH))
      {
        gpDisplay->fillRect(item.x, item.y, item.width, item.height, colTxtBack);
        retVal = true;
      }
      else
      {
        cParGraph* p = reinterpret_cast<cParGraph*>(item.p);
        if (p->getInitPlot())
        {
          gpDisplay->fillRect(item.x, item.y, item.width, item.height, colTxtBack);
          retVal = true;
        }
      }
    }
    
    gpDisplay->setCursor(item.x + 1, item.y + 1 + m_fontOffset);
    gpDisplay->setTextSize(item.textSize);

    item.p->update(false);
    switch (item.type)
    {
      case ITEM_TEXT:
        {
          cParText* p = reinterpret_cast<cParText*>(item.p);
          if(p)
          {
            printText(p->getText(), item);
            retVal = true;
          }
        }
        break;

      case ITEM_ENUM:
        {
          cParEnum* p = reinterpret_cast<cParEnum*>(item.p);
       // gpDisplay->fillRect(item.x, item.y, item.width, item.height, COL_TEXTBACK);
          printText(p->getActText(), item);
          retVal = true;
        }
        break;

      case ITEM_LIST:
        {
          cParList* p = reinterpret_cast<cParList*>(item.p);
         // gpDisplay->fillRect(item.x, item.y, item.width, item.height, COL_TEXTBACK);
          printText(p->getActText(), item);
          retVal = true;
        }
        break;


      case ITEM_NR:
        {
          cParNum* p = reinterpret_cast<cParNum*>(item.p);
          if(p->getDecimals() < 10)
          {
            if(p->getLeadingZeros() == 0)
              snprintf(fmt, sizeof(fmt), "%%.%luf",p->getDecimals());
            else
              snprintf(fmt, sizeof(fmt), "%%0%lu.%luf",p->getLeadingZeros(), p->getDecimals());
            snprintf(str, sizeof(str), fmt,p->get());
            printText(str, item);
            retVal = true;
          }
        }
        break;

      case ITEM_DATE:
        {
          cParDate* p = reinterpret_cast<cParDate*>(item.p);
          unsigned int y = p->getYear();
          if(y > 2000)
            y -= 2000;
          if(y > 100)
            y-= 100;
          snprintf(str, sizeof(str), "%02lu.%02lu.%02u", p->getDay(), p->getMonth(), y);
          printText(str, item);
          retVal = true;
        }
        break;

     case ITEM_TIME:
       {
         cParTime* p = reinterpret_cast<cParTime*>(item.p);
         snprintf(str, sizeof(str), "%02lu:%02lu:%02lu", p->getHour(), p->getMin(), p->getSec());
         printText(str, item);
         retVal = true;
       }
       break;

     case ITEM_GEO:
       {
         cParGeoPos* p = reinterpret_cast<cParGeoPos*>(item.p);
         if(p->getDegLat() >= 0)
           lat = 'N';
         else
           lat = 'S';
         if(p->getDegLon() >= 0)
           lon = 'E';
         else
           lon = 'W';
         snprintf(str, sizeof(str), "%c %02i\x87 %.3f  %c %03i\x87 %.3f",
                  lat, p->getDegLat(), p->getMinfLat(),
                  lon, p->getDegLon(), p->getMinfLon());
         printText(str, item);
         retVal = true;
       }
       break;

     case ITEM_STRING:
        {
          cParStr* p = reinterpret_cast<cParStr*>(item.p);
          gpDisplay->setTextColor(pColors->text, pColors->textBack);
          printText(p->get(), item);
          retVal = true;
        }
        break;

     case ITEM_BUTTON:
        {
          int yb = item.y;
          cParBtn* p = reinterpret_cast<cParBtn*>(item.p);
          gpDisplay->setCursor(item.x + 2, item.y + 2 + m_fontOffset);
          printText(p->getText(), item);
          gpDisplay->drawRect(item.x, yb, item.width - 2, item.height - 1, pColors->text);
          gpDisplay->fillRect(item.x + item.width - 2, yb + 2, 2, item.height  - 1, pColors->msgShadow);
          gpDisplay->fillRect(item.x + 2, yb + item.height - 1, item.width -2 , 2, pColors->msgShadow);
          retVal = true;
        }
        break;

     case ITEM_GRAPH:
        {
          cParGraph* p = reinterpret_cast<cParGraph*>(item.p);
          p->initDiagram();
          if(item.f)
            item.f(this, NO, item.p);
          p->plotGraph();
          retVal = true;
        }
        break;

     default:
        break;
    }
  return retVal;
}

bool MMEM cMenuesystem::drawSubPanel(cPanel* pan, thPanel hPanel)
{
  bool retVal = false;
  if(!m_isInitialized)
    return false;
  size_t size = pan->itemList.size();
  switch(pan->type)
  {
    case PNL_FKEYS:
      if(m_refreshFkey)
      {
        gpDisplay->drawLine(pan->x, pan->y, m_width - 1, pan->y, pColors->menuLine);
        for(uint32_t i = 1; i < m_nrFkeys; i++)
        {
           int x = m_width * i / m_nrFkeys;
           gpDisplay->drawLine(x, pan->y, x, m_height - 1, pColors->menuLine);
        }
        retVal = true;
      }
      break;
    case PNL_HEADER:
      gpDisplay->drawLine(pan->x, pan->height - 1, m_width - 1, pan->height - 1, pColors->menuLine);
      retVal = true;
      break;

    case PNL_DROPDOWN:
      if(pan->isRefresh())
      {
        pan->refresh();
        gpDisplay->fillRect(pan->x, pan->y, pan->width, pan->height, pColors->text);
        gpDisplay->drawRect(pan->x, pan->y, pan->width, pan->height, pColors->text);
        retVal = true;
      }
      break;

    case PNL_MAIN:
    case PNL_PARLIST:
      break;
    case PNL_MESSAGE:
      if(pan->isRefresh())
      {
        pan->refresh();
        gpDisplay->fillRect(pan->x, pan->y, pan->width, getHdrHeight(), pColors->textHdrBack);
        gpDisplay->setCursor(pan->x + pan->width/2 - 40, pan->y + 2 + m_fontOffset);
        gpDisplay->setTextColor(pColors->textHdr, pColors->textHdrBack);
        printText(Txt::get(11), pan->x, pan->y + m_fontOffset, pan->width, getHdrHeight());
        gpDisplay->fillRect(pan->x, pan->y + getHdrHeight(), pan->width, pan->height - getHdrHeight(), pColors->textBack);
        gpDisplay->drawRect(pan->x, pan->y, pan->width, pan->height, pColors->text);
        gpDisplay->fillRect(pan->x + pan->width, pan->y + 3, 3, pan->height, pColors->msgShadow);
        gpDisplay->fillRect(pan->x + 3, pan->y + pan->height, pan->width, 3, pColors->msgShadow);
        retVal = true;
      }
      break;
  }
  for(uint32_t i = 0; i < size; i++)
     retVal |= drawItem(pan->itemList[i], hPanel, i, pan->type);
  return retVal;
}

bool MMEM cMenuesystem::drawPanels()
{
  bool retVal = false;
  if(!m_isInitialized || !m_enabled)
    return false;
  
  // draw header panel
  if(m_hHeadrPanel < m_panelList.size())
  {
    if(m_refreshHdr)
    {
      m_refreshHdr = false;
      cPanel* pan = &m_panelList[m_hHeadrPanel];
      gpDisplay->fillRect(pan->x,pan->y,pan->width, pan->height - 1, pColors->textHdrBack);
      drawSubPanel(pan, m_fKeyPanel);
      retVal = true;
    }
  }

  // draw fKey panel
  if(m_fKeyPanel < m_panelList.size())
  {
    if(m_refreshFkey)
    {
      cPanel* pan = &m_panelList[m_fKeyPanel];
      gpDisplay->fillRect(pan->x,pan->y,pan->width, pan->height, pColors->textHdrBack);
      retVal |= drawSubPanel(pan, m_fKeyPanel);
      m_refreshFkey = false;
      retVal = true;
    }
  }

  // draw main panel
  if(m_refreshMain)
  {
    m_refreshMain = false;
    cPanel& p = m_panelList[m_mainPanel];
    gpDisplay->fillRect(p.x, p.y, p.width, p.height, pColors->textBack);

    if(!m_msgActive)
      drawSubPanel(&m_panelList[m_mainPanel], m_mainPanel);
    retVal = true;
  }

  // draw focus panel
  if ((m_focus.panel < m_panelList.size() ) && (m_focus.panel != m_hHeadrPanel))
  {
    cPanel* pan = &m_panelList[m_focus.panel];
    retVal |= drawSubPanel(pan, m_focus.panel);
  }

  if(retVal)
    refreshDisplay();
  return retVal;
}



void MMEM cMenuesystem::init(bool withDialogs)
{
  initPars();
  if(withDialogs)
  {
    initDialogs();
    drawPanels();
    refreshAll();
    Serial.println("init with dialogs");
    m_isInitialized = true;
  }
}

void MMEM cMenuesystem::setFocus(thPanel pan, thItem item, enFocusState state)
{
  m_focus.panel = pan;
  m_focus.item = item;
  m_focus.state = state;
}

void MMEM cMenuesystem::setMainPanel(thPanel pan)
{
  m_mainPanel = pan;
  m_refreshMain = true;
  setFocus(pan, 0, enFocusState::DISP);
  m_focusSaveMsg = m_focus;
  m_focusSaveDrop = m_focus;
}

int32_t MMEM cMenuesystem::handleKey(enKey key)
{
  DPRINTF1("handleKey(%i)", key);

  int32_t retVal = 0;
  cPanel* pan;
  // check if F-Key was pressed
  if (m_fKeyPanel < m_panelList.size())
  {
    bool pressed = false;
    pan = &m_panelList[m_fKeyPanel];
    switch(key) {
      case enKey::F1:
        pressed = true;
        if(pan->itemList[0].f)
          pan->itemList[0].f(this, key, pan->itemList[0].p);
        break;

      case enKey::F2:
        pressed = true;
        if(pan->itemList[1].f)
          pan->itemList[1].f(this, key, pan->itemList[1].p);
        break;

     case enKey::F3:
        pressed = true;
       if(pan->itemList[2].f)
          pan->itemList[2].f(this, key, pan->itemList[2].p);
        break;

     case enKey::F4:
        pressed = true;
       if(pan->itemList[3].f)
          pan->itemList[3].f(this, key, pan->itemList[3].p);
       break;

     case NOKEY:
       return 0;

     default:
       break;
    }
    if (pressed)
    {
      m_lastKeyTime = millis();
      drawPanels();
      return retVal;
    }
  }

  // check key strokes for focus panel
  if(key != TICK)
    m_lastKeyTime = millis();
  if (m_focus.panel < m_panelList.size())
  {
    pan = &m_panelList[m_focus.panel];
    handleEditMode(*pan, key);
  }
  drawPanels();
  return retVal;
}

void MMEM cMenuesystem::resetTimer()
{
  m_lastKeyTime = millis();
}

void MMEM cMenuesystem::setEnumDropDown(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  cParEnum* p = reinterpret_cast<cParEnum*>(pThis->m_pDropDownEnum.p);
  uint32_t val = pThis->getFocusItem() + pThis->m_firstDropDownItem;
  //  Serial.printf("set enum to %u\n",val);
  p->set(val);

  if (pThis->m_pDropDownEnum.f)
    pThis->m_pDropDownEnum.f(pThis, key, pItem);
}


void MMEM cMenuesystem::setListDropDown(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  cParList* p = reinterpret_cast<cParList*>(pThis->m_pDropDownEnum.p);
  uint32_t val = pThis->getFocusItem() + pThis->m_firstDropDownItem;
  //  Serial.printf("set enum to %u\n",val);
  p->set(val);

  if (pThis->m_pDropDownEnum.f)
    pThis->m_pDropDownEnum.f(pThis, key, pItem);
}


void MMEM cMenuesystem::handleEditMode(cPanel& pan, enKey key)
{
  stPanelItem& item = pan.itemList[m_focus.item];

  cParEnum* pEnum = nullptr;
  if (pan.type == PNL_DROPDOWN)
    pEnum = reinterpret_cast<cParEnum*>(m_pDropDownEnum.p);

 // DPRINTF1("handleEditMode(%i, %i)", pan., key);
  switch(key) 
  {
    case enKey::KEY_OK:
      switch(m_focus.state)
      {
        case enFocusState::DISP:
          m_focus.item = pan.findFirstEditItem();
          if(m_focus.item < pan.itemList.size())
          {
            m_focus.state = enFocusState::SELECT;
            pan.itemList[m_focus.item].p->update(true);
          }
          break;

        case enFocusState::EDIT:
          m_focus.state = enFocusState::SELECT;
          gpDisplay->fillRect(item.x, item.y, item.width, item.height, pColors->textBack);
          pan.itemList[m_focus.item].p->update(true);
          if(pan.type == PNL_MESSAGE)
            destroyMsg();
          break;

        case enFocusState::SELECT:
          if(pan.type == PNL_FKEYS)
          {
            refreshFkeyPanel();
            if(pan.itemList[m_focus.item].f)
              pan.itemList[m_focus.item].f(this, key, item.p);
          }
          else if (pan.type == PNL_DROPDOWN)
          {
            gpDisplay->fillRect(pan.x, pan.y, pan.width, pan.height, pColors->textBack);
            if(item.f)
              item.f(this, key, item.p);
            destroyDropDown();
          }
          else if (item.type == ITEM_ENUM)
          {
            if(item.f)
              item.f(this, key, item.p);
            createDropDown(item, item.x, item.y, item.width, setEnumDropDown);
          }
          else if (item.type == ITEM_LIST)
          {
            if(item.f)
              item.f(this, key, item.p);
            createDropDown(item, item.x, item.y, item.width, setListDropDown);
          }
          else if(item.type == ITEM_BUTTON)
          {
            if(item.f)
              item.f(this, key, item.p);
            if(pan.type == PNL_MESSAGE)
              destroyMsg();
          }
          else if(item.type == ITEM_NR)
          {
            m_focus.state = enFocusState::EDIT;
          }
          if(pan.type != PNL_MESSAGE)
            pan.itemList[m_focus.item].p->update(true);
          break;

        default:
          m_focus.state = enFocusState::DISP;
          break;
      }
      break;

    case enKey::UP:
      DPRINTLN1("Key up\n");
      switch(m_focus.state)
      {
        case enFocusState::DISP:
          DPRINTLN1("enFocusState::FST_DISP\n");
//          m_focus.item = pan.findFirstEditItem();
          m_focus.state = enFocusState::SELECT;
          // fall through intended

        case enFocusState::SELECT:
          DPRINTLN1("FST_SELECT\n");
          gpDisplay->fillRect(item.x, item.y, item.width, item.height, pColors->textBack);
          pan.itemList[m_focus.item].p->update(true);
          if (pan.type != PNL_DROPDOWN)
          {
            m_focus.item = pan.findPrevEditItem(m_focus.item);
            if (m_focus.item > pan.itemList.size())
            {
              if(pan.type == PNL_MESSAGE)
                m_focus.item = pan.findLastEditItem();
              else
              {
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
          else
          {
            if(m_focus.item <= 1)
            {
              if(m_firstDropDownItem > 0)
                m_firstDropDownItem--;
              else
              {
                if(m_focus.item > 0)
                  m_focus.item--;
                else
                {
                  m_firstDropDownItem = (thItem)(pEnum->size() - pan.itemList.size());
                  m_focus.item = (thItem)(pan.itemList.size() - 1);
                }
              }
              reInitDropDownItems();
            }
            else
              m_focus.item--;
          }
          pan.itemList[m_focus.item].p->update(true);
          DPRINTLN1("update done");
          break;

      case enFocusState::EDIT:
          DPRINTLN1("EDIT\n");
          editPar(item, key);
          break;
      }
      break;

    case enKey::DOWN:
      switch(m_focus.state)
      {
        case enFocusState::DISP:
       //   m_focus.item = pan.findFirstEditItem();
          m_focus.state = enFocusState::SELECT;
          // fall through intended

        case enFocusState::SELECT:
          gpDisplay->fillRect(item.x, item.y, item.width, item.height, pColors->textBack);
          pan.itemList[m_focus.item].p->update(true);
          if(pan.type != PNL_DROPDOWN)
          {
            m_focus.item = pan.findNextEditItem(m_focus.item);
            if (m_focus.item > pan.itemList.size())
            {
              if(pan.type == PNL_MESSAGE)
                m_focus.item = pan.findFirstEditItem();
              else
              {
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
          else
          {
            if(m_focus.item < (pan.itemList.size() - 1))
              m_focus.item++;
            else
            {
              m_firstDropDownItem++;
              if(m_firstDropDownItem > (pEnum->size() - pan.itemList.size()))
              {
                 m_firstDropDownItem = 0;
                 m_focus.item = 0;
              }
              reInitDropDownItems();
            }
          }
          pan.itemList[m_focus.item].p->update(true);
          break;

        case enFocusState::EDIT:
          editPar(item, key);
          break;
      }
      break;

    default:
      break;
  }
}

void MMEM cMenuesystem::editPar(stPanelItem &item, enKey key)
{
    switch (item.type)
    {
      case ITEM_ENUM:
        break;

      case ITEM_NR:
        {
          cParNum* p = reinterpret_cast<cParNum*>(item.p);
          if(key == UP)
          {
            float newVal = p->get() + p->getStep();
            if (newVal > p->getMax())
              p->set(p->getMin());
            else
              p->set(newVal);
          }
          if(key == DOWN)
          {
            float newVal = p->get() - p->getStep();
            if(newVal < p->getMin())
              p->set(p->getMax());
            else
              p->set(newVal);
          }
        }
        break;

    case ITEM_TEXT:
    default:
        break;
    }
    if(item.f)
      item.f(this, key, item.p);
}

void MMEM cMenuesystem::reInitDropDownItems()
{
  cPanel& pan = m_panelList[m_dropDownPan];
  size_t s = pan.itemList.size() - 1;
  uint32_t firstItem = 0;
  if(m_firstDropDownItem > 0)
  {
    firstItem = 1;
    cParText* p = reinterpret_cast<cParText*>(pan.itemList[0].p);
    p->setText("    \x1E");
  }

  if(m_pDropDownEnum.type == ITEM_ENUM)
  {
    cParEnum* pDr = reinterpret_cast<cParEnum*>(m_pDropDownEnum.p);
    for (uint32_t i = firstItem; i < s; i++)
    {
      cParText* p = reinterpret_cast<cParText*>(pan.itemList[i].p);
      p->setText(pDr->getText(i + m_firstDropDownItem));
    }
    cParText* p1 = reinterpret_cast<cParText*>(pan.itemList[s].p);
    if((s + m_firstDropDownItem) < (pDr->size() - 1))
      p1->setText("    \x1F");
    if((s + m_firstDropDownItem) == (pDr->size() - 1))
      p1->setText(pDr->getText(s + m_firstDropDownItem));
  }
  else if (m_pDropDownEnum.type == ITEM_LIST)
  {
    cParList* pDr = reinterpret_cast<cParList*>(m_pDropDownEnum.p);
    for (uint32_t i = firstItem; i < s; i++)
    {
      cParText* p = reinterpret_cast<cParText*>(pan.itemList[i].p);
      p->setText(pDr->getText(i + m_firstDropDownItem));
    }
    cParText* p1 = reinterpret_cast<cParText*>(pan.itemList[s].p);
    if((s + m_firstDropDownItem) < (pDr->size() - 1))
      p1->setText("    \x1F");
    if((s + m_firstDropDownItem) == (pDr->size() - 1))
      p1->setText(pDr->getText(s + m_firstDropDownItem));
  }
}


void MMEM msgFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
   pThis->destroyMsg();
}


void MMEM cMenuesystem::msgYesFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  if(pThis->m_msgCallBack)
    pThis->m_msgCallBack(pThis, YES, pItem);
}


void MMEM cMenuesystem::msgNoFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  if(pThis->m_msgCallBack)
    pThis->m_msgCallBack(pThis, enKey::NO, pItem);
}


void MMEM cMenuesystem::showMsg(enMsg type, fuFocus f, bool isSmall, const char *str, const char* str2,
                                                  const char* str3, const char* str4)
{
  if(!m_isInitialized)
    return;
  int xp, yp, wp, hp, xb1, xb21, xb22, wb, hb, lf;
  if (isSmall)
  {
    xp = 5;
    yp = 25;
    wp = 118;
    hp = 60;
    xb1 = 64;
    xb21 = 20;
    xb22 = 68;
      wb = 40;
      hb = 12;
      lf = 9;
  }
  else
  {
    xp = 30;
    yp = 30;
    wp = 260;
    hp = 50;
    xb1 = 130;
    xb21 = 60;
    xb22 = 180;
    wb = 60;
    hb = 16;
    lf = 12;
  }


  m_MsgPan = createPanel(PNL_MESSAGE, xp, yp, wp, hp);

  cParStr* parStr = new cParStr(str);
  tCoord x = xp + 5;
  tCoord y = yp + 20;
  tCoord w = wp - 20;
  m_msgCallBack = f;
  m_panelList[m_MsgPan].addStrItem(parStr, x, y, w, lf);
  if(str2)
  {
    y += lf;
    parStr = new cParStr(str2);
    m_panelList[m_MsgPan].addStrItem(parStr, x, y, w, lf);
    if(str3) {
      y += lf;
      parStr = new cParStr(str3);
      m_panelList[m_MsgPan].addStrItem(parStr, x, y, w, lf);
      if(str4) {
        y += lf;
        parStr = new cParStr(str4);
        m_panelList[m_MsgPan].addStrItem(parStr, x, y, w, lf);
      }
    }
  }
  y += 15;
  thItem btn = m_panelList[m_MsgPan].itemList.size();
  m_focusSaveMsg = m_focus;
  if(getPan(m_focus.panel)->itemList[m_focus.item].type == enItemType::ITEM_BUTTON)
     setFocus(m_focus.panel, m_focus.item, enFocusState::SELECT);

  switch (type)
  {
    case enMsg::INFO:
      m_panelList[m_MsgPan].addBtnItem(10, xb1, y, wb, 16, msgFunc);
      setFocus(m_MsgPan,btn, enFocusState::EDIT);
      break;
    case enMsg::YESNO:
      m_panelList[m_MsgPan].addBtnItem(15, xb21, y, wb, hb, msgYesFunc);
      m_panelList[m_MsgPan].addBtnItem(16, xb22, y, wb, hb, msgNoFunc);
      setFocus(m_MsgPan,btn, enFocusState::SELECT);
      break;
  }
  m_panelList[m_MsgPan].height = y - m_panelList[m_MsgPan].y + 14 + 10;
  m_msgActive = true;
}


void MMEM cMenuesystem::destroyMsg()
{
  if (m_MsgPan < m_panelList.size())
  {
    size_t s = m_panelList[m_MsgPan].itemList.size();
    for (size_t i = 0; i < s; i++) {
      if(m_panelList[m_MsgPan].itemList[i].type == ITEM_STRING)
      {
         delete m_panelList[m_MsgPan].itemList[i].p;
         m_panelList[m_MsgPan].itemList[i].p = nullptr;
      }
      if(m_panelList[m_MsgPan].itemList[i].type == ITEM_BUTTON)
      {
         delete m_panelList[m_MsgPan].itemList[i].p;
         m_panelList[m_MsgPan].itemList[i].p = nullptr;
      }
    }
    m_panelList[m_MsgPan].itemList.clear();
    m_panelList.pop_back();
    m_MsgPan = 9999;
    m_focus = m_focusSaveMsg;
    m_msgCallBack = nullptr;
    m_msgActive = false;
    refreshMainPanel();
  }
}



thPanel MMEM cMenuesystem::createDropDown(stPanelItem item, tCoord x, tCoord y, tCoord width, fuFocus f)
{
  cParList* pL = nullptr;
  cParEnum* pE = nullptr;
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

  if(s > 0)
  {
//    Serial.printf("Dropdown, size: %u\n", s);
    m_pDropDownEnum = item;
    m_dropDownMaxSize = (m_height - getFkeypanHeight() - y) / m_lineHeight;
    m_firstDropDownItem = 0;
    bool shorted = false;
    if(m_dropDownMaxSize < s)
    {
      if(y > (tCoord)(m_height / 2))
      {
        if(y > ((s - m_dropDownMaxSize) * m_lineHeight))
          y -= (s - m_dropDownMaxSize) * m_lineHeight;
        else
          shorted = true;
      }
      else
        shorted = true;
    }
    if(shorted)
    {
      m_dropDownPan = createPanel(PNL_DROPDOWN, x - 1, y - 1, width + 2, m_dropDownMaxSize * m_lineHeight+ 2);
      s = m_dropDownMaxSize -1;
    }
    else
      m_dropDownPan = createPanel(PNL_DROPDOWN, x - 1, y - 1, width + 2, s * m_lineHeight + 2);

    tCoord yi = y;

    for(size_t i = 0; i < s; i++)
    {
      if(item.type == ITEM_ENUM)
        m_panelList[m_dropDownPan].addTextItem(pE->getText(i), x,yi,width, m_lineHeight, true, f);
      else if(item.type == ITEM_LIST)
        m_panelList[m_dropDownPan].addTextItem(pL->getText(i), x,yi,width, m_lineHeight, true, f);
      yi += m_lineHeight;
    }

    if(shorted)
      m_panelList[m_dropDownPan].addTextItem("   \x1f", x,yi,width, m_lineHeight, true, f);

    m_focusSaveDrop = m_focus;
    setFocus(m_dropDownPan,0,enFocusState::SELECT);
    return m_dropDownPan;
  }
  else
    return 9999;
}

void MMEM cMenuesystem::destroyDropDown()
{
  if (m_dropDownPan < m_panelList.size())
  {
    m_panelList[m_dropDownPan].itemList.clear();
    m_panelList.erase(m_panelList.begin() + m_dropDownPan);
    if((m_MsgPan <= m_panelList.size()) && (m_MsgPan > m_dropDownPan))
    {
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

void MMEM cMenuesystem::refreshMainPanel()
{
  if(!m_isInitialized)
    return;
  m_refreshMain = true;
  if(m_mainPanel < m_panelList.size())
    m_panelList[m_mainPanel].refresh();
}

void MMEM cMenuesystem::refreshFkeyPanel()
{
  m_refreshFkey = true;
  if(m_fKeyPanel < m_panelList.size())
    m_panelList[m_fKeyPanel].refresh();
}

void MMEM cMenuesystem::refreshHdrPanel()
{
  m_refreshHdr = true;
  m_panelList[m_hHeadrPanel].refresh();
}

void MMEM cMenuesystem::refreshAll()
{
  if(!m_isInitialized)
    return;
  refreshFkeyPanel();
  refreshHdrPanel();
  refreshMainPanel();
  drawPanels();
}

void MMEM cMenuesystem::enable(bool on)
{
  if(!m_enabled && on)
    resetTimer();
  m_enabled = on; 
}

uint32_t MMEM cMenuesystem::getTimeAfterKeyPress()
{
  uint32_t actTim;
  actTim = millis();
  uint32_t delta;
  if (actTim < m_lastKeyTime)
    delta = actTim + (0xFFFFFFFF - m_lastKeyTime);
  else
    delta = actTim - m_lastKeyTime;
  return delta;
}

bool MMEM cMenuesystem::keyPauseLongEnough(uint32_t ms)
{
  return getTimeAfterKeyPress() > ms;
}


void MMEM cMenuesystem::printText(const char* txt, int16_t x, int16_t y, int16_t w, int16_t h)
{
  char buf[512];
  cUtils::replaceUTF8withInternalCoding(txt, buf, sizeof(buf));
 // gpDisplay->setClipFrame(x, y, w, h);  //TODO
  gpDisplay->print(buf);
}

void MMEM cMenuesystem::printText(const char *txt, stPanelItem& item)
{
  printText(txt, item.x, item.y, item.width, item.height);
}

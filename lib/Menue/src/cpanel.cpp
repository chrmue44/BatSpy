/*************************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/
#include "cpanel.h"

cPanel::cPanel() :
  itemList() {
}

int MMEM cPanel::addTextItem(const char *pText, tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit, fuFocus f, tCoord textSize) {
  stPanelItem item;

  cParText* p = new cParText();
  p->automatic(true);
  p->setText(pText);
  item.p = p;
  item.type = ITEM_TEXT;
  item.textSize = textSize;
  return addItem(item, x, y, w, h, isEdit, f);
}

int MMEM cPanel::addBtnItem(thText txt, tCoord x, tCoord y, tCoord w, tCoord h, fuFocus f, int idx)
{
  stPanelItem item;
  cParBtn* p = new cParBtn();
  p->setText(Txt::get(txt));
  p->setIndex(idx);
  item.type = ITEM_BUTTON;
  item.p = p;
  return addItem(item, x, y, w, h,true, f);
}

int MMEM cPanel::addItem(stPanelItem& item, tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit, fuFocus f) 
{
  item.x = x;
  item.y = y;
  item.width = w;
  item.height = h;
  item.f = f;
  item.isEdit = isEdit;
  item.isVisible = true;
  return itemList.push_back(item);
}

int MMEM cPanel::addEnumItem(cParEnum* pPar, tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit, fuFocus f) 
{
  stPanelItem item;
  item.p = pPar;
  item.type = ITEM_ENUM;
  return addItem(item, x, y, w, h, isEdit, f);
}

int MMEM cPanel::addListItem(cParList* pPar, tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit, fuFocus f) 
{
  stPanelItem item;
  item.p = pPar;
  item.type = ITEM_LIST;
  return addItem(item, x, y, w, h, isEdit, f);
}

int MMEM cPanel::addNumItem(cParNum* pPar, tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit, fuFocus f) 
{
  stPanelItem item;
  item.p = pPar;
  item.type = ITEM_NR;
  return addItem(item, x, y, w, h,isEdit, f);
}

int MMEM cPanel::addDateItem(cParDate* pPar,tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit, fuFocus f) 
{
  stPanelItem item;
  item.p = pPar;
  item.type = ITEM_DATE;
  return addItem(item, x, y, w, h,isEdit, f);
}


int MMEM cPanel::addTimeItem(cParTime* pPar,tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit, fuFocus f)
{
  stPanelItem item;
  item.p = pPar;
  item.type = ITEM_TIME;
  return addItem(item, x, y, w, h,isEdit, f);
}

int MMEM cPanel::addGeoItem(cParGeoPos* pPar,tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit, fuFocus f) 
{
  stPanelItem item;
  item.p = pPar;
  item.type = ITEM_GEO;
  return addItem(item, x, y, w, h,isEdit, f);
}

int MMEM cPanel::addStrItem(cParStr* pPar,tCoord x, tCoord y, tCoord w, tCoord h, bool isEdit, fuFocus f)
{
  stPanelItem item;
  item.p = pPar;
  item.type = ITEM_STRING;
  pPar->setColor(pColors->text);
  return addItem(item, x, y, w, h,isEdit, f);
}

int MMEM cPanel::addGraphItem(cParGraph* pPar,tCoord x, tCoord y, tCoord w, tCoord h, fuFocus f)
{
  stPanelItem item;
  item.p = pPar;
  item.type = ITEM_GRAPH;
  int ret = addItem(item, x, y, w, h,false, f);
  pPar->setSize(x, y, w, h);
  pPar->setAmplitude(100);
  return ret;
}

thItem MMEM cPanel::findFirstEditItem() 
{
  thItem retVal = 9999;
  for(uint32_t i  = 0; i < itemList.size(); i++) 
  {
    if(itemList[i].isEdit && itemList[i].isVisible) 
    {
       retVal = i;
       break;
    }
  }
  return retVal;
}


thItem MMEM cPanel::findLastEditItem() 
{
  thItem retVal = 9999;
  thItem i = itemList.size() - 1;
  for(;;i--) {
    if(itemList[i].isEdit && itemList[i].isVisible) 
    {
      retVal = i;
      break;
    }
    if(i == 0)
      break;
  }
  return retVal;
}


thItem MMEM cPanel::findNextEditItem(thItem it) 
{
  thItem retVal = 9999;
  for(uint32_t i  = it + 1; i < itemList.size(); i++) 
  {
    if(itemList[i].isEdit && itemList[i].isVisible) 
    {
       retVal = i;
       break;
    }
  }
  return retVal;
}

thItem MMEM cPanel::findPrevEditItem(thItem it) 
{
  thItem retVal = 9999;
  if ( it > 0) 
  {
    thItem i = it - 1;
    for(;;i--) 
    {
      if(itemList[i].isEdit && itemList[i].isVisible) 
      {
        retVal = i;
        break;
      }
      if(i == 0)
        break;
    }
  }
  return retVal;
}

void MMEM cPanel::refresh() 
{
  for(size_t i = 0; i < itemList.size(); i++) 
  {
    itemList[i].p->update(true);
    if (itemList[i].type == ITEM_GRAPH) 
    {
      cParGraph* p = reinterpret_cast<cParGraph*>(itemList[i].p);
      p->initPlot(true);
    }
  }
}

bool MMEM cPanel::isRefresh() 
{
  bool retVal = false;
  for(size_t i = 0; i < itemList.size(); i++) 
  {
    if (itemList[i].p->isUpdate()) 
    {
      retVal = true;
      break;
    }
  }
  return retVal;
}

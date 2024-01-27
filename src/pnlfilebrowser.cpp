/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "pnlfilebrowser.h"
#include "pnlmain.h"
#include "debug.h"
#include "cutils.h"
#include "globals.h"

int getItemIndexFile(int i) {  return FILE_OFFS + (i * (BTN_COUNT + 1)); }
int getItemIndexSel(int i) {   return getItemIndexFile(i) + 1; }
int getItemIndexDel(int i) {   return getItemIndexFile(i) + 2; }

bool MEMP isDir(const char* name)
{
  bool retVal = false;
  for(size_t i = 0; i < devStatus.dir.size(); i++)
  {
    if(strncmp(name, devStatus.dir[i].name, sizeof (devStatus.dir[i].name)) == 0)
    {
      retVal = devStatus.dir[i].isDir;
      break;
    }
  }
  return retVal;
}

void MEMP fillFileList(cPanel* pan)
{
  size_t idxPnl = 0;
  size_t idxDir = devStatus.fileIndex;
  pan->itemList[2].isVisible = devStatus.fileIndex > 0;

  do
  {
    if(idxDir < devStatus.dir.size())
    {
      const char* pFile = devStatus.dir[idxDir].name;
      const char* pMatch = devStatus.dirFilter.getActText();
      if(cUtils::match(pMatch, pFile) || devStatus.dir[idxDir].isDir)
      {
        devStatus.dirFiles[idxPnl].set(pFile);
        pan->itemList[getItemIndexSel(idxPnl)].isVisible = true;
        pan->itemList[getItemIndexDel(idxPnl)].isVisible = true;
        cParStr* p = static_cast<cParStr*>(pan->itemList[getItemIndexFile(idxPnl)].p);
        if(devStatus.dir[idxDir].isDir)
          p->setColor(pColors->textDir);
        else
          p->setColor(pColors->text); 
        idxPnl++;
      }
    }
    else
    {
      devStatus.dirFiles[idxPnl].set("");
      pan->itemList[getItemIndexSel(idxPnl)].isVisible = false;
      pan->itemList[getItemIndexDel(idxPnl)].isVisible = false;
      idxPnl++;
    }
    idxDir++;
  } while(idxPnl < DIR_PAN_SIZE);
  pan->itemList[getItemIndexFile(DIR_PAN_SIZE)].isVisible = idxDir < devStatus.dir.size();
  pan->refresh();
}

void MEMP initFileBrowser(cPanel* pan, const char* dir)
{
  cSdCard& sd = cSdCard::inst();
  sd.chdir(dir);
  devPars.fileName.set(sd.getActDir());
  sd.dir(devStatus.dir);
  devStatus.fileIndex = 0;
  menue.refreshHdrPanel();
  fillFileList(pan);
  menue.initFocusOnPanel(pan);
}


void MEMP fuFilter(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  cPanel* pnl = pThis->getPan(panFileBrowser);
  initFileBrowser(pnl, cSdCard::inst().getActDir());
}


void MEMP funcSel(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  cParBtn* p = reinterpret_cast<cParBtn*>(pItem);
  int index = getItemIndexFile(p->getIndex());
  cPanel* pnl = pThis->getPan(panFileBrowser);
  cParStr* pStr = reinterpret_cast<cParStr*>(pnl->itemList[index].p);
  if(isDir(pStr->get()))
    initFileBrowser(pnl, pStr->get());
  else
  {
    char buf[FILENAME_LEN];
    strncpy(buf, cSdCard::inst().getActDir(), sizeof (buf));
    strcat(buf,"/");
    strcat(buf,pStr->get());
    devPars.fileName.set(buf);
    pThis->refreshHdrPanel();
    setFileToDisplay(buf);
  }
}

char buf[FILENAME_LEN];

void MEMP fuDelete(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  if(key == enKey::YES)
  {
    cSdCard& sd = cSdCard::inst();
    if(isDir(buf))
      sd.deldir(buf);
    else
      sd.del(buf);
    initFileBrowser(pThis->getPan(panFileBrowser), sd.getActDir());
  }
}

void MEMP fuFormat(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  switch(key)
  {
    case enKey::YES:
      {
        /*enSdRes ret = */cSdCard::inst().format();
        cSdCard::inst().mkDir("rec");
        cSdCard::inst().mkDir("log");
        cSdCard::inst().mkDir("info");
        cSdCard::inst().mkDir("prj");
/*        if(ret == enSdRes::OK)
          pThis->showMsg(enMsg::INFO, nullptr, Txt::get(1043));
        else
          pThis->showMsg(enMsg::INFO, nullptr, Txt::get(1042)); */
      }
      break;

    default:
      break;
  }
}

void MEMP f2FormatFunc(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  pThis->showMsg(enMsg::YESNO, fuFormat, Txt::get(1040), Txt::get(1041));
}

void MEMP funcDel(cMenuesystem* pThis, enKey key, cParBase* pItem)
{  
  cParBtn* p = reinterpret_cast<cParBtn*>(pItem);
  int index = getItemIndexFile(p->getIndex());
  cPanel* pnl = pThis->getPan(panFileBrowser);
  cParStr* pStr = reinterpret_cast<cParStr*>(pnl->itemList[index].p);
  strncpy(buf, pStr->get(), sizeof (buf));
  pThis->showMsg(enMsg::YESNO, fuDelete, Txt::get(1600), pStr->get());
}

void MEMP funcUp(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  if(devStatus.fileIndex > 0)
    devStatus.fileIndex--;
  fillFileList(pThis->getPan(panFileBrowser));
}

void MEMP funcDown(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
   devStatus.fileIndex++;
   fillFileList(pThis->getPan(panFileBrowser));
}

int MEMP initFkeyFilePanel(cPanel* pan, tCoord lf)
{
  int retVal;
  // Serial.println("initDialogs2");
  retVal  = pan->addTextItem(1,   0, 227, 80, lf, true, f1Func);
  retVal |= pan->addTextItem(5,  81, 227, 79, lf, true, f2FormatFunc);
  retVal |= pan->addTextItem(3, 161, 227, 79, lf, false);
  retVal |= pan->addTextItem(4, 241, 227, 79, lf, true, f4Func);
  return retVal;
}
int MEMP initFileBrowserPan(cPanel* pan, tCoord lf)
{
  int err = 0;
  err |= pan->addTextItem(220,                     3, 20         ,  80, lf);
  err |= pan->addEnumItem(&devStatus.dirFilter,   90, 20         ,  50, lf, true, fuFilter);
  err |= pan->addBtnItem(230,   190, 20,          35, lf + 4, funcUp);
  for(int i = 0; i< DIR_PAN_SIZE; i++)
  {
    tCoord y = 31 + (i+1) * (lf + 2);
    err |= pan->addStrItem(&devStatus.dirFiles[i], 3, y, 160, lf + 2, false);
    err |= pan->addBtnItem(500, 190, y, 35, lf + 2 , funcSel, i);
    err |= pan->addBtnItem(510, 230, y, 35, lf + 2 , funcDel, i);
  }
  err |= pan->addBtnItem(235,   190, 40 + (DIR_PAN_SIZE + 1) * (lf + 2), 35, lf + 4, funcDown);
  return err;
}

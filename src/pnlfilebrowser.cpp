#include "pnlfilebrowser.h"
#include "pnlmain.h"
#include "debug.h"

int getItemIndexFile(int i) {  return FILE_OFFS + (i * (BTN_COUNT + 1)); }
int getItemIndexSel(int i) {   return getItemIndexFile(i) + 1; }
int getItemIndexDel(int i) {   return getItemIndexFile(i) + 2; }


void fillFileList(cPanel* pan)
{
  size_t idxPnl = 0;
  size_t idxDir = devStatus.fileIndex;
  pan->itemList[2].isVisible = devStatus.fileIndex > 1;

  do
  {
    if(idxDir < devStatus.dir.size())
    {
      if(strcmp(devStatus.dir[idxDir].name, "..") == 0)
      {
        idxDir++;
        continue;
      }
      else
      {
        devStatus.dirFiles[idxPnl].set(devStatus.dir[idxDir].name);
        pan->itemList[getItemIndexSel(idxPnl)].isVisible = true;
        pan->itemList[getItemIndexDel(idxPnl)].isVisible = true;
      }
    }
    else
    {
      devStatus.dirFiles[idxPnl].set("");
      pan->itemList[getItemIndexSel(idxPnl)].isVisible = false;
      pan->itemList[getItemIndexDel(idxPnl)].isVisible = false;
    }
    idxPnl++;
    idxDir++;
  } while(idxPnl < DIR_PAN_SIZE);
  pan->refresh();
}

void initFileBrowser(cPanel* pan, const char* dir)
{
  cSdCard& sd = cSdCard::inst();
  sd.chdir(dir);
  devPars.fileName.set(sd.getActDir());
  sd.dir(devStatus.dir);
  devStatus.fileIndex = 1;
  menue.refreshHdrPanel();
  fillFileList(pan);
  menue.initFocusOnPanel(pan);
}


void dirBrowseFunc(cMenuesystem* pThis, enKey key) {
  enSdRes rc = enSdRes::OK;
  cSdCard& sd = cSdCard::inst();
  enFocusState state = pThis->getFocusState();
  // leaving dropdown
  if (pThis->isDropDownInFocus())
  {
    sd.chdir(devPars.dirSel.getActText());
    devPars.fileName.set(sd.getActDir());
    sd.dir(devStatus.dir);
    devStatus.fileIndex = 1;
    pThis->refreshHdrPanel();
    fillFileList(pThis->getPan(panFileBrowser));
  }
  // before opening dropdown
  else
  {
    if ((state == SELECT) && (key = enKey::KEY_OK))
    {
      tDirInfo p;
      rc = sd.dir(p);
      devPars.dirSel.clear();
      if (rc == OK)
      {
        tDirInfo& dir = p;
        for (size_t i = 0; i < dir.size(); i++)
        {
          if (dir[i].isDir)
          {
            devPars.dirSel.addItem(dir[i].name);
            DPRINTF2("dir name: %s\n", dir[i].name);
          }
        }
      }
      DPRINTF2("devPars.dirSel.size: %u\n", devPars.dirSel.size());
    }
  }
}

void fuFilter(cMenuesystem* pThis, enKey key, cParBase* pItem)
{

}

void funcSel(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  cParBtn* p = reinterpret_cast<cParBtn*>(pItem);
  int index = getItemIndexFile(p->getIndex());
  cPanel* pnl = pThis->getPan(panFileBrowser);
  cParStr* pStr = reinterpret_cast<cParStr*>(pnl->itemList[index].p);
  initFileBrowser(pnl, pStr->get());
}

char buf[FILENAME_LEN];

void fuDelete(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  cSdCard& sd = cSdCard::inst();
  sd.del(buf);
  initFileBrowser(pThis->getPan(panFileBrowser), sd.getActDir());
}

void funcDel(cMenuesystem* pThis, enKey key, cParBase* pItem)
{  
  cParBtn* p = reinterpret_cast<cParBtn*>(pItem);
  int index = getItemIndexFile(p->getIndex());
  cPanel* pnl = pThis->getPan(panFileBrowser);
  cParStr* pStr = reinterpret_cast<cParStr*>(pnl->itemList[index].p);
  strncpy(buf, pStr->get(), sizeof (buf));
  pThis->showMsg(enMsg::YESNO, fuDelete, Txt::get(1600), pStr->get());
}

void funcUp(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
  if(devStatus.fileIndex > 1)
    devStatus.fileIndex--;
  fillFileList(pThis->getPan(panFileBrowser));
}

void funcDown(cMenuesystem* pThis, enKey key, cParBase* pItem)
{
   devStatus.fileIndex++;
   fillFileList(pThis->getPan(panFileBrowser));
}

int initFileBrowserPan(cPanel* pan, tCoord lf)
{
  int err = 0;
  err |= pan->addTextItem(220,                     3, 20         ,  80, lf);
  err |= pan->addEnumItem(&devStatus.dirFilter,   90, 20         ,  50, lf, true, fuFilter);
  err |= pan->addBtnItem(230,   190, 20,          40, lf, funcUp);
  for(int i = 0; i< DIR_PAN_SIZE; i++)
  {
    err |= pan->addStrItem(&devStatus.dirFiles[i], 3, 33 + (i+1) * lf, 160, lf, false);
    err |= pan->addBtnItem(500, 190, 33 + (i+1) * lf, 35, lf , funcSel, i);
    err |= pan->addBtnItem(510, 230, 33 + (i+1) * lf, 35, lf , funcDel);
  }
  err |= pan->addBtnItem(235,   190, 40 + (DIR_PAN_SIZE + 1) * lf, 40, lf, funcDown);
  return err;
}

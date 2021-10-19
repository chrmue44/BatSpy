#include "cproject.h"
#include "cxmlhelper.h"
#include "cutils.h"
#include "cfileinfo.h"
#include "cmenue.h"

cPrjoject::cPrjoject() :
m_isOpen(false)
{

}

//function will return total number of days
int  getNumberOfDays(int month, int year)
{
  //leap year condition, if month is 2
  if( month == 2)
  {
    if((year%400==0) || (year%4==0 && year%100!=0))
      return 29;
    else
      return 28;
  }
  //months which has 31 days
  else if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8
  ||month == 10 || month==12)
    return 31;
  else
    return 30;
}



void cPrjoject::openPrjFile()
{
  char fName[FILENAME_LEN];

  m_startY = year();
  m_startM = month();
  m_startD = day();
  snprintf(m_prjName, sizeof(m_prjName),"%04i%02i%02i", m_startY, m_startM, m_startD);
  snprintf(fName, sizeof (fName), "/prj/%04i%02i%02i", m_startY, m_startM, m_startD);
  cSdCard::inst().mkDir("/prj");
  cSdCard::inst().mkDir(fName);
  cSdCard::inst().chdir(fName);
  cSdCard::inst().mkDir("Records");
  cSdCard::inst().chdir("/");

  strcat(fName, "/");
  strcat(fName, m_prjName);
  strcat(fName, ".bpr");

  tAttrList attr;
  m_xml.openFile(fName);
  m_xml.initXml();
  stAttr item;
  strncpy(item.name, "xmlns:xsi", sizeof(item.name));
  strncpy(item.value, "http://www.w3.org/2001/XMLSchema-instance", sizeof(item.value));
  attr.push_back(item);
  strncpy(item.name, "FileVersion", sizeof(item.name)); 
  strncpy(item.value, "6", sizeof(item.value));
  attr.push_back(item);
  strncpy(item.name, "Originator", sizeof(item.name));
  strncpy(item.value, "BatSpy, Ver" __DATE__  "/"  __TIME__ , sizeof(item.value));
  attr.push_back(item);
  m_xml.openTag("BatExplorerProjectFile", &attr);

  m_xml.simpleTag("Name", m_prjName);
  m_xml.simpleTag("Type","Elekon");
  m_xml.openTag("Records");
  m_counter = 1;
  m_isOpen = true;
}

void cPrjoject::saveStartTime()
{
  m_fy = year();
  m_fMo = month();
  m_fDay = day();
  m_fh = hour();
  m_fMin = minute();
  m_fSec = second();
}

const char* cPrjoject::createElekonFileName()
{
  snprintf(m_wavFile, sizeof (m_wavFile),"/prj/%s/Records/%04i%02i%02i_%04i.wav", m_prjName, m_startY, m_startM, m_startD, m_counter);
  snprintf(m_name, sizeof (m_name),"%04i%02i%02i_%04i", m_startY, m_startM, m_startD, m_counter);
  saveStartTime();
  m_counter++;
  return m_wavFile;
}

void cPrjoject::addFile()
{
  tAttrList attr;
  stAttr item;
  attr.clear();
  strncpy(item.name, "File", sizeof(item.name));
  strncpy(item.value, m_wavFile, sizeof(item.value));
  attr.push_back(item);
  strncpy(item.name, "Name",sizeof(item.name));
  strncpy(item.value, m_name, sizeof(item.value));
  attr.push_back(item);
  m_xml.simpleTagNoValue("Record", &attr);
}

void cPrjoject::closePrjFile()
{
  if(m_isOpen)
  {
    m_xml.closeTag("Records");
    m_xml.closeTag("BatExplorerProjectFile");

//  std::cout << "writing prj file " << name << std::endl;
    m_xml.closeFile();
    m_isOpen = false;
  }
}


enSdRes cPrjoject::createRecordingDir(char* out, size_t outBufSize)
{
  char buf[80];

  int y = year();
  int mo = month();
  int d = day();
  int h = hour();

  snprintf(out, outBufSize,"/rec/%02i/%02i/%02i/%02i",y, mo, d, h);
  enSdRes ret = cSdCard::inst().chdir(out);
  if(ret != OK)
  {
    ret = cSdCard::inst().chdir("/rec");
    if(ret != OK)
    {
      ret = cSdCard::inst().mkDir("/rec");
    }
    snprintf(buf, sizeof(buf),"/rec/%02i",y);
    ret = cSdCard::inst().chdir(buf);
    if(ret != OK)
    {
      ret = cSdCard::inst().mkDir(buf);
    }
    snprintf(buf, sizeof(buf),"/rec/%02i/%02i",y, mo);
    ret = cSdCard::inst().chdir(buf);
    if(ret != OK)
    {
      ret = cSdCard::inst().mkDir(buf);
    }
    snprintf(buf, sizeof(buf),"/rec/%02i/%02i/%02i",y ,mo ,d);
    ret = cSdCard::inst().chdir(buf);
    if(ret != OK)
    {
      ret = cSdCard::inst().mkDir(buf);
    }
    snprintf(buf, sizeof(buf),"/rec/%02i/%02i/%02i/%02i",y ,mo, d, h);
    ret = cSdCard::inst().chdir(buf);
    if(ret != OK)
    {
      ret = cSdCard::inst().mkDir(buf);
    }
    ret = cSdCard::inst().chdir(out);
  }
  return ret;
}

enSdRes cPrjoject::createTimeFileName(enRecFmt recFmt)
{
  enSdRes ret = createRecordingDir(m_wavFile, sizeof (m_wavFile));
  if(ret == OK)
  {
    char buf[16];
    char ext[5];
    switch(recFmt)
    {
      case enRecFmt::RAW:
        strncpy(ext, "raw", sizeof(ext));
        break;
      case enRecFmt::WAV:
        strncpy(ext, "wav", sizeof(ext));
        break;
    }
    saveStartTime();
    snprintf(buf,sizeof(buf),"/%02i%02i.%s",m_fMin, m_fSec, ext);
    strcat(m_wavFile, buf);
  }
  return ret;
}


void cPrjoject::writeInfoFile(float peakVal, size_t sampleCnt, enRecFmt recFmt)
{
  cFileInfo info;
  char date [32];
  char infoFile[80];

 // strncpy(infoFile, m_wavFile, sizeof(infoFile));
  if (recFmt == enRecFmt::RAW)
    cUtils::replace(m_wavFile, ".raw", ".xml", infoFile, sizeof(infoFile));
  if (recFmt == enRecFmt::WAV)
    cUtils::replace(m_wavFile, ".wav", ".xml", infoFile, sizeof(infoFile));

  snprintf(date,sizeof(date),"%02i.%02i.%02i %02i:%02i:%02i",m_fDay, m_fMo, m_fy, m_fh, m_fMin, m_fSec);
  float duration = (float)sampleCnt/ cAudio::getSampleRateHz((enSampleRate) devPars.sampleRate.get());
  info.write(infoFile, duration, date, cUtils::getFileName(m_wavFile),
              devStatus.geoPos.getLat(), devStatus.geoPos.getLon(), peakVal);
}

enRecFmt cPrjoject::getFileFmt()
{
  if(devPars.projectType.get() == enProjType::ELEKON)
    return enRecFmt::WAV;
  else
    return (enRecFmt)devPars.recFmt.get();
}

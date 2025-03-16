#include "cproject.h"
#include "cxmlhelper.h"
#include "cutils.h"
#include "cfileinfo.h"
#include "globals.h"
//#define DEBUG_LEVEL  4
#include "debug.h"
cProject::cProject() :
m_isOpen(false)
{

}

//function will return total number of days
int MEMF getNumberOfDays(int month, int year)
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

const char* PROGMEM MSG_OPEN_PRJ  = "successfully loaded project file with %i records\n";
const char* PROGMEM MSG_CLOSE_PRJ = "closed project file\n";


void MEMF cProject::createPrjFile(const char* pNotes, bool night)
{
  char buf[FILENAME_LEN];

  int startY = year();
  int startM = month();
  int startD = day();
  if (night)
  {
    snprintf(m_prjName, sizeof(m_prjName), "%04i%02i%02i_BAT", startY, startM, startD);
    snprintf(buf, sizeof(buf), "/prj/%04i%02i%02i_BAT", startY, startM, startD);
  }
  else
  {
    snprintf(m_prjName, sizeof(m_prjName), "%04i%02i%02i_BIRD", startY, startM, startD);
    snprintf(buf, sizeof(buf), "/prj/%04i%02i%02i_BIRD", startY, startM, startD);
  }
  cSdCard::inst().mkDir("/prj");
  cSdCard::inst().mkDir(buf);
  cSdCard::inst().chdir(buf);
  if(devPars.checkDebugLevel(DBG_TRIGGER))
    audio.openTrigLog(cSdCard::inst().getActDir());
  cSdCard::inst().mkDir("Records");
  cSdCard::inst().chdir("/");

  strcat(buf, "/");
  strcat(buf, m_prjName);
  strcat(buf, ".batspy");
  
  DPRINTF4("open project %s \n", buf);
  if(!cSdCard::inst().fileExists(buf))
  {
    DPRINTF4("Create new project file %s\n", buf);
    initializePrjFile(buf, pNotes, startY, startM, startD, night);
    m_recCount = 0;
  }
  m_isOpen = true;
}

void MEMF cProject::initMicInfo(cXmlHelper& xml)
{
  xml.openTag("Microphone");
  xml.simpleTag("Id", micInfo.getId());
  xml.simpleTag("Type", micInfo.getType());
  xml.simpleTag("Comment", micInfo.getComment());
  xml.openTag("FrequencyResponse");
  for(int i = 0; i < micInfo.getFreqResponsePointCount(); i++)
  {
    tAttrList attr;
    stAttr item;
    attr.clear();
    strncpy(item.name, "Frequency", sizeof(item.name));
    snprintf(item.value, sizeof(item.value),"%.1f", micInfo.getFreqResponsePoint(i)->freq);
    attr.push_back(item);
    strncpy(item.name, "Amplitude",sizeof(item.name));
    snprintf(item.value, sizeof(item.value),"%.1f", micInfo.getFreqResponsePoint(i)->ampl);
    attr.push_back(item);
    xml.simpleTagNoValue("Point", &attr);
  }
  xml.closeTag("FrequencyResponse");
  xml.closeTag("Microphone");
}

void MEMF cProject::initializePrjFile(const char* fName, const char* pNotes, int startY, int startM, int startD, bool  night)
{
  tAttrList attr;
  cXmlHelper xml;

  xml.openFile(fName, false);
  xml.initXml();
  stAttr item;
  strncpy(item.name, "xmlns:xsi", sizeof(item.name));
  strncpy(item.value, "http://www.w3.org/2001/XMLSchema-instance", sizeof(item.value));
  attr.push_back(item);
  strncpy(item.name, "FileVersion", sizeof(item.name)); 
  strncpy(item.value, "3", sizeof(item.value));
  attr.push_back(item);
  strncpy(item.name, "Originator", sizeof(item.name));
  strncpy(item.value, "BatSpy, Ver" __DATE__  "/"  __TIME__ , sizeof(item.value));
  attr.push_back(item);
  xml.openTag("BatExplorerProjectFile", &attr);

  xml.simpleTag("Name", m_prjName);
  char buf[128];
  snprintf(buf, sizeof(buf),"%04i-%02i-%02iT%02i:%02i:%02i", startY, startM, startD, hour(),  minute(), m_fSec);
  xml.simpleTag("Created", buf);
  if(night)
    xml.simpleTag("ProjectType", "Bats");
  else
    xml.simpleTag("ProjectType", "Birds");
  xml.simpleTag("Notes",pNotes);
  xml.simpleTag("AutoProcess", "true");
  initMicInfo(xml);
  xml.closeTag("BatExplorerProjectFile");
  xml.closeFile();
}

void MEMF cProject::saveStartTime()
{
  m_fy = year();
  m_fMo = month();
  m_fDay = day();
  m_fh = hour();
  m_fMin = minute();
  m_fSec = second();
}

const char* MEMF cProject::createElekonFileName()
{
  saveStartTime();
  snprintf(m_wavFile, sizeof (m_wavFile),"/prj/%s/Records/%04i%02i%02i_%02i%02i%02i.wav", m_prjName, m_fy, m_fMo, m_fDay, m_fh, m_fMin ,m_fSec);
  snprintf(m_name, sizeof (m_name),"%04i%02i%02i_%02i%02i%02i", m_fy, m_fMo, m_fDay, m_fh, m_fMin ,m_fSec);
  return m_wavFile;
}

void MEMF cProject::addFile()
{
  m_recCount++;
}

void MEMF cProject::closePrjFile()
{
  m_recCount = 0;
  m_isOpen = false;
  sysLog.log("close project");
  trigLog.close();
}

void MEMF cProject::reset()
{
  m_isOpen = false;
}

void MEMF cProject::writeInfoFile(float peakVal, size_t sampleCnt, size_t parSet)
{
  cFileInfo info;
  char date [32];
  char infoFile[80];

 // strncpy(infoFile, m_wavFile, sizeof(infoFile));
  cUtils::replace(m_wavFile, ".wav", ".xml", infoFile, sizeof(infoFile));

  snprintf(date,sizeof(date),"%02i.%02i.%02i %02i:%02i:%02i",m_fDay, m_fMo, m_fy, m_fh, m_fMin, m_fSec);
  float sampleRate = cAudio::getSampleRateHz((enSampleRate) devPars.sampleRate[parSet].get());
  float duration = 0;
  if(sampleRate > 0.1)
    duration = (float)sampleCnt/ sampleRate;
  info.write(infoFile, duration, date, cUtils::getFileName(m_wavFile),
              devStatus.geoPos.getLat(), devStatus.geoPos.getLon(), peakVal,
               devStatus.temperature.get(), devStatus.humidity.get(), parSet);
}

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

const char* PROGMEM MSG_OPEN_PRJ = "successfully loaded project file with %i records\n";

void MEMF cProject::openExistingPrjFile(const char* fName, int startY, int startM, int startD)
{
  DPRINTF4("open existing file %s\n", fName);
  cSdCard& sd = cSdCard::inst();
  tFILE file;
  char buf[256];
  char tmpFileName[64];
  size_t bytesRead;
  m_recCount = 0;
  cUtils::replace(fName, ".bpr",".tmp", tmpFileName, sizeof(tmpFileName));

  DPRINTF4("rename file: old: %s, new: %s\n", fName, tmpFileName);
  enSdRes res = sd.rename(fName, tmpFileName);
  if(res != enSdRes::OK)
    DPRINTF4("error renaming file: old: %s, new: %s\n",fName, tmpFileName);

  res = sd.openFile(tmpFileName, file, enMode::READ);

  char notes[128];
  notes[0] = 0;

  while(res == enSdRes::OK)
  {
    enSdRes res = sd.readLine(file, buf, sizeof(buf), bytesRead);
//    DPRINTF4("line: %s", buf);
    if(res != enSdRes::OK)
    {
      DPRINTLN4("end of file reached");
      break;
    }
    char* p = strstr(buf, "<Record ");
    if(p != nullptr)
    {
      DPRINTLN4("found tag <Record>");
      m_xml.writeLineToFile(buf);
      m_recCount++;
      continue;
    }
    p = strstr(buf, "</Records>");
    if(p != nullptr)
      break;
    p = strstr(buf, "<Notes>");
    if (p != nullptr)
    {
      p += 7;
      char* p2 = strstr(buf, "</Notes>");
      if (p2 != nullptr) // closing tag in same line
      {
        int len = strlen(notes);
        char* pDst = notes + len;
        while ((p < p2) && (p < notes + len - 1))
          *pDst++ = *p++;
        *pDst = 0;
      }
      else
        strncpy(notes, p, sizeof(notes) - 1);
      DPRINTLN4("found tag <Notes>");
      continue;
    }
    p = strstr(buf, "</Notes>");
    if (p != nullptr)
    {
      *p = 0;
      strcat(notes, buf);
      continue;
    }
    p = strstr(buf, "<Records>");
    if(p != nullptr)
    {
      DPRINTLN4("found tag <Records>");
      initializePrjFile(fName, notes, startY, startM, startD);
    }
  }
  DPRINTF4("found %i records\n", m_recCount);
  if(devPars.debugLevel.get() > 0)
    sysLog.logf(MSG_OPEN_PRJ, m_recCount);
  sd.closeFile(file);
  sd.del(tmpFileName);
}


void MEMF cProject::createPrjFile(const char* pNotes)
{
  char buf[FILENAME_LEN];

  int startY = year();
  int startM = month();
  int startD = day();
  snprintf(m_prjName, sizeof(m_prjName),"%04i%02i%02i", startY, startM, startD);
  snprintf(buf, sizeof (buf), "/prj/%04i%02i%02i", startY, startM, startD);
  cSdCard::inst().mkDir("/prj");
  cSdCard::inst().mkDir(buf);
  cSdCard::inst().chdir(buf);
  cSdCard::inst().mkDir("Records");
  cSdCard::inst().chdir("/");

  strcat(buf, "/");
  strcat(buf, m_prjName);
  strcat(buf, ".bpr");
  //trigLog.log("fileName,count,ampl,freq,avg, avgPeak,bandwidth");
  DPRINTF4("open project %s \n", buf);
  if(cSdCard::inst().fileExists(buf))
    openExistingPrjFile(buf, startY, startM, startD);
  else
  {
    DPRINTF4("Create new project file %s\n", buf);
    initializePrjFile(buf, pNotes, startY, startM, startD);
    m_recCount = 0;
  }
  m_isOpen = true;
}

void MEMF cProject::initializePrjFile(const char* fName, const char* pNotes, int startY, int startM, int startD)
{
  tAttrList attr;
  m_xml.openFile(fName, false);
  m_xml.initXml();
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
  m_xml.openTag("BatExplorerProjectFile", &attr);

  m_xml.simpleTag("Name", m_prjName);
  char buf[128];
  snprintf(buf, sizeof(buf),"%04i-%02i-%02iT%02i:%02i:%02i", startY, startM, startD, hour(),  minute(), m_fSec);
  m_xml.simpleTag("Created", buf);
  m_xml.simpleTag("Notes",pNotes);
  m_xml.simpleTag("AutoProcess", "true");
  m_xml.openTag("Records");
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
  tAttrList attr;
  stAttr item;
  attr.clear();
  strncpy(item.name, "File", sizeof(item.name));
  strncpy(item.value, m_name, sizeof(item.value));
  strcat(item.value, ".wav");
  attr.push_back(item);
  strncpy(item.name, "Name",sizeof(item.name));
  strncpy(item.value, m_name, sizeof(item.value));
  attr.push_back(item);
  m_xml.simpleTagNoValue("Record", &attr);
  m_recCount++;
}

void MEMF cProject::closePrjFile()
{
  if(m_isOpen)
  {
    DPRINTLN4("close prj file");
    m_xml.closeTag("Records");
    m_xml.closeTag("BatExplorerProjectFile");
    m_xml.closeFile();
    m_isOpen = false;
    //trigLog.close();
  }
}


void MEMF cProject::writeInfoFile(float peakVal, size_t sampleCnt)
{
  cFileInfo info;
  char date [32];
  char infoFile[80];

 // strncpy(infoFile, m_wavFile, sizeof(infoFile));
  cUtils::replace(m_wavFile, ".wav", ".xml", infoFile, sizeof(infoFile));

  snprintf(date,sizeof(date),"%02i.%02i.%02i %02i:%02i:%02i",m_fDay, m_fMo, m_fy, m_fh, m_fMin, m_fSec);
  float sampleRate = cAudio::getSampleRateHz((enSampleRate) devPars.sampleRate.get());
  float duration = 0;
  if(sampleRate > 0.1)
    duration = (float)sampleCnt/ sampleRate;
  info.write(infoFile, duration, date, cUtils::getFileName(m_wavFile),
              devStatus.geoPos.getLat(), devStatus.geoPos.getLon(), peakVal,
               devStatus.temperature.get(), devStatus.humidity.get());
}

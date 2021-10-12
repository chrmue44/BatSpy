#include "cproject.h"
#include "cxmlhelper.h"
#include "cutils.h"




cProject::cProject()
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



void cProject::openPrjFile(const char* name)
{
  tAttrList attr;
  m_xml.openFile(name);
  m_xml.initXml();
  stAttr item;
  strncpy(item.name, "xmlns:xsi", sizeof(item.name));
  strncpy(item.value, "http://www.w3.org/2001/XMLSchema-instance", sizeof(item.value));
  attr.push_back(item);
  strncpy(item.name, "FileVersion", sizeof(item.name)); 
  strncpy(item.value, "6", sizeof(item.value));
  attr.push_back(item);
  strncpy(item.name, "Originator", sizeof(item.name));
  strncpy(item.value, "BatExplorer_WPF,2.1.7017.34360", sizeof(item.value));
  attr.push_back(item);
  m_xml.openTag("BatExplorerProjectFile", &attr);

  char buf[32];
  snprintf(buf, sizeof(buf),"%04i%02i%02i", m_startY, m_startM, m_startD);
  m_xml.simpleTag("Name", buf);
  m_xml.simpleTag("Type","Elekon");
  m_xml.openTag("Records");
}

void cProject::addFile(const char* wavFile, const char* name)
{
  tAttrList attr;
  stAttr item;

    attr.clear();
    strncpy(item.name, "File", sizeof(item.name));
    strncpy(item.value, wavFile, sizeof(item.value));
    attr.push_back(item);
    strncpy(item.name, "Name",sizeof(item.name));
    strncpy(item.value, name, sizeof(item.value));
    attr.push_back(item);
    m_xml.simpleTagNoValue("Record", &attr);
  }


void cProject::closePrjFile()
{
  m_xml.closeTag("Records");
  m_xml.closeTag("BatExplorerProjectFile");

//  std::cout << "writing prj file " << name << std::endl;
  m_xml.closeFile();
}

#include "cxmlhelper.h"
#include <string.h>

cXmlHelper::cXmlHelper() :
m_fileIsOpen(false)
{

}

enSdRes cXmlHelper::openFile(const char* name)
{
  enSdRes res = cSdCard::inst().openFile(name, m_file, enMode::WRITE);
  if(res == enSdRes::OK)
    m_fileIsOpen = true;
  return res;
}

void cXmlHelper::closeFile()
{
  if(m_fileIsOpen)
    cSdCard::inst().closeFile(m_file);
  m_fileIsOpen = false;
}
void  cXmlHelper::writeString(const char* str)
{
  if(m_fileIsOpen)
  {
    size_t strLen = strlen(str);
    size_t written;
    cSdCard::inst().writeFile(m_file, str, written, strLen);
  }
}


void cXmlHelper::initXml() 
{
  m_indent = 0;
  writeString("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
}

void cXmlHelper::closeTag(const char* tagName)
{
  m_indent -= INDENT;
  writeString("</");
  writeString(tagName);
  writeString(">");
  newLine();
}

void cXmlHelper::openTag(const char* tagName, tAttrList* attr, bool close) {
  if(!close)
    m_indent += INDENT;
  writeString("<");
  writeString(tagName);
  if(attr != NULL)
  {
    for(size_t i = 0; i < attr->size(); i++)
    {
      writeString(" ");
      writeString((*attr)[i].name);
      writeString("=\"");
      writeString((*attr)[i].value);
      writeString(+"\"");
    }
    if(close)
      writeString(" ");
  }
  if(close)
    writeString("/");
  writeString(">");
  newLine();
}

void cXmlHelper::value(const char* value)
{
  writeString(value);
  m_indent -= INDENT;
  newLine();
}

void cXmlHelper::value(double value) 
{
  char buf[32];
  snprintf(buf, sizeof(buf),"%f",value);
  writeString(buf);
  m_indent -= INDENT;
  newLine();
}

void cXmlHelper::value(int value) 
{
  char buf[32];
  snprintf(buf, sizeof(buf),"%i",value);
  writeString(buf);
  m_indent -= INDENT;
  newLine();
}


void cXmlHelper::simpleTag(const char* tagName, const char* val, tAttrList* attr) 
{
  openTag(tagName, attr);
  value(val);
  closeTag(tagName);
}

void cXmlHelper::simpleTagNoValue(const char* tagName, tAttrList *attr )
{
  openTag(tagName, attr, true);
}

void cXmlHelper::newLine()
{
  if(m_indent < 0)
    m_indent = 0;
  writeString("\n");
  for(int i = 0; i < m_indent; i++)
  {
    writeString(" ");
  }
}

/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "cxmlhelper.h"
#include "config.h"
#include <string.h>

cXmlHelper::cXmlHelper() :
m_fileIsOpen(false),
m_lastWasClose(false),
m_lastWasOpen(false)

{

}

enSdRes MEMF cXmlHelper::openFile(const char* name)
{
  enSdRes res = cSdCard::inst().openFile(name, m_file, enMode::WRITE);
  if(res == enSdRes::OK)
    m_fileIsOpen = true;
  return res;
}

void MEMF cXmlHelper::closeFile()
{
  if(m_fileIsOpen)
    cSdCard::inst().closeFile(m_file);
  m_fileIsOpen = false;
}
void MEMF cXmlHelper::writeString(const char* str)
{
  if(m_fileIsOpen)
  {
    size_t strLen = strlen(str);
    size_t written;
    cSdCard::inst().writeFile(m_file, str, written, strLen);
  }
}


void MEMF cXmlHelper::initXml() 
{
  m_indent = 0;
  writeString("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
}

void MEMF cXmlHelper::closeTag(const char* tagName)
{
  indent(false);
  writeString("</");
  writeString(tagName);
  writeString(">");
  newLine();
  m_lastWasClose = true;
}

void MEMF cXmlHelper::indent(bool openTag)
{
  if(openTag)
  {
    if(m_lastWasOpen)
    {
      m_indent += INDENT;
    }
  }
  else
  {
    if(m_lastWasClose)
      m_indent -= INDENT;
  }
  if(m_newLine)
  {
    for(int i = 0; i < m_indent; i++)
      writeString(" ");
    m_newLine = false;
  }
}

void MEMF cXmlHelper::openTag(const char* tagName, tAttrList* attr, bool nl, bool close, bool withIndent)
{
  indent(withIndent);
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
  newLine(nl);
  m_lastWasOpen = true;
}

void MEMF cXmlHelper::value(const char* val, bool newL)
{
  indent(false);
  writeString(val);
  newLine(newL);
}

void MEMF cXmlHelper::value(double val, bool newL)
{
  char buf[32];
  snprintf(buf, sizeof(buf),"%f",val);
  value(&buf[0], newL);
}

void MEMF cXmlHelper::value(int val, bool newL)
{
  char buf[32];
  snprintf(buf, sizeof(buf),"%i",val);
  value(&buf[0], newL);
}


void MEMF cXmlHelper::simpleTag(const char* tagName, const char* val, tAttrList* attr) 
{
  openTag(tagName, attr, false);
  value(val, false);
  closeTag(tagName);
}

void MEMF cXmlHelper::simpleTag(const char* tagName, float val, tAttrList* attr)
{
  openTag(tagName, attr, false);
  value(val, false);
  closeTag(tagName);
}

void MEMF cXmlHelper::simpleTagNoValue(const char* tagName, tAttrList *attr )
{
  openTag(tagName, attr, true, true, false);
}

void MEMF cXmlHelper::newLine(bool nl)
{
  if(nl)
  {
    if(m_indent < 0)
      m_indent = 0;
    writeString("\n");
    m_newLine = true;
  }
  m_lastWasClose = false;
  m_lastWasOpen = false;
}

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
//#define DEBUG_LEVEL 4
#include "debug.h"

cXmlHelper::cXmlHelper() :
  m_keepOpen(false),
  m_fileIsOpen(false),
  m_lastWasClose(false),
  m_lastWasOpen(false)

{

}

enSdRes MEMF cXmlHelper::openFile(const char* name, bool keepOpen)
{
  m_keepOpen = keepOpen;
  strncpy(m_fileName, name, sizeof(m_fileName));
  enSdRes res = cSdCard::inst().openFile(name, m_file, enMode::WRITE);
  if (res == enSdRes::OK)
  {
    m_fileIsOpen = true;
  }
  else
    DPRINTF4("error opening file '%s', error: %i\n", name, (int)res);
  return res;
}


void MEMF cXmlHelper::intermediateOpen()
{
  if (!m_keepOpen && !m_fileIsOpen)
  {
    enSdRes res = cSdCard::inst().openFile(m_fileName, m_file, enMode::APPEND);
    DPRINTF1("open log file: %s, res %i\n", m_fileName, res);
    if (res == enSdRes::OK)
      m_fileIsOpen = true;
    else
      DPRINTLN1("failed to open file");
  }
}
void MEMF cXmlHelper::intermediateClose()
{
  if (!m_keepOpen && m_fileIsOpen)
  {
    DPRINTLN1("closed xml");
    cSdCard::inst().closeFile(m_file);
    m_fileIsOpen = false;
  }
}

void MEMF cXmlHelper::closeFile()
{
  if(m_fileIsOpen)
    cSdCard::inst().closeFile(m_file);
  m_fileIsOpen = false;
}

void MEMF cXmlHelper::writeLineToFile(const char* str)
{
  intermediateOpen();
  writeString(str);
  intermediateClose();
}

void MEMF cXmlHelper::writeString(const char* str)
{
  if(m_fileIsOpen)
  {
    size_t strLen = strlen(str);
    size_t written;
    enSdRes res = cSdCard::inst().writeFile(m_file, str, written, strLen);
    if(res != enSdRes::OK)
      DPRINTF4("error writing string: %i\n", (int) res);
  }
}


void MEMF cXmlHelper::initXml()
{
  intermediateOpen();
  m_indent = 0;
  writeString("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
  intermediateClose();
}

void MEMF cXmlHelper::closeTag(const char* tagName)
{
  intermediateOpen();
  indent(false);
  writeString("</");
  writeString(tagName);
  writeString(">");
  newLine();
  m_lastWasClose = true;
  intermediateClose();
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
  intermediateOpen();
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
  intermediateClose();
}

void MEMF cXmlHelper::value(const char* val, bool newL)
{
  intermediateOpen();
  indent(false);
  writeString(val);
  newLine(newL);
  intermediateClose();
}

void MEMF cXmlHelper::value(double val, bool newL)
{
  intermediateOpen();
  char buf[32];
  snprintf(buf, sizeof(buf),"%f",val);
  value(&buf[0], newL);
  intermediateClose();
}

void MEMF cXmlHelper::value(int val, bool newL)
{
  intermediateOpen();
  char buf[32];
  snprintf(buf, sizeof(buf),"%i",val);
  value(&buf[0], newL);
  intermediateClose();
}


void MEMF cXmlHelper::simpleTag(const char* tagName, const char* val, tAttrList* attr) 
{
  intermediateOpen();
  openTag(tagName, attr, false);
  value(val, false);
  closeTag(tagName);
}

void MEMF cXmlHelper::simpleTag(const char* tagName, float val, tAttrList* attr)
{
  intermediateOpen();
  openTag(tagName, attr, false);
  value(val, false);
  closeTag(tagName);
  intermediateClose();
}

void MEMF cXmlHelper::simpleTagNoValue(const char* tagName, tAttrList *attr )
{
  intermediateOpen();
  openTag(tagName, attr, true, true, false);
  intermediateClose();
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

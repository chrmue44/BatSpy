/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include <Arduino.h>
#include <cstring>
#include <TimeLib.h>
#include "cgpx.h"

cGpx::cGpx() :
 m_open(false)
{

}


void cGpx::open()
{
  if(!m_open)
  {
    if(cSdCard::inst().chdir(GPX_LOG_PATH) == enSdRes::OPEN_DIR)
      cSdCard::inst().mkDir(GPX_LOG_PATH);
    openLog();

    tAttrList attr;
    stAttr a;
    a.setName("version");
    a.setVal("1.1");
    attr.push_back(a);
    a.setName("creator");
    a.setVal("BatSpy");
    attr.push_back(a);
    m_xml.openTag("gpx", &attr);
    m_xml.openTag("trk");

    char buf[256];
    snprintf(buf, sizeof (buf), "Track %04i-%02i-%02i %02i:%02i", year(), month(), day(), hour(), minute());
    m_xml.simpleTag("name", buf);
    m_xml.openTag("trkseg");
    m_open = true;
  }
}


void cGpx::openLog()
{
  char name[256];
  snprintf(name, sizeof(name),"%s/%04i%02i%02i_%02i%02i.gpx", GPX_LOG_PATH, year(), month(), day(), hour(), minute());
  m_xml.openFile(name);
  m_xml.initXml();
}


void cGpx::close()
{
  if(m_open)
  {
    m_xml.closeTag("trkseg");
    m_xml.closeTag("trk");
    m_xml.closeTag("gpx");
    m_xml.closeFile();
    m_open = false;
  }
}


void cGpx::log(float lat, float lon, float ele)
{
  tAttrList attr;
  stAttr a;
  a.setName("lat");
  snprintf(a.value, sizeof(a.value), "%f", lat);
  attr.push_back(a);
  a.setName("lon");
  snprintf(a.value, sizeof(a.value), "%f", lon);
  attr.push_back(a);
  m_xml.openTag("trkpt", &attr);
  m_xml.simpleTag("ele", ele);
  char buf[80];
  snprintf(buf, sizeof (buf),"%04i-%02i-%02iT%02i:%02i:%02iZ", year(), month(), day(), hour(), minute(), second());
  m_xml.simpleTag("time", buf);
  m_xml.closeTag("trkpt");
}

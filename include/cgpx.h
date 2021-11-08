/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#ifndef CGPX_H
#define CGPX_H

#include "cSdCard.h"
#include "cxmlhelper.h"

#define GPX_LOG_PATH "/gpx"

class cGpx
{
 public:
  cGpx();
  void open();
  void log(float lat, float lon, float ele);
  void close();
  bool isOpen() {return m_open; }

 private:
  void openLog();
  cXmlHelper m_xml;
  bool m_open;
};

extern cGpx gpx;

#endif // CGPX_H

/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * this panel provides some internal information of the 
 * hardware and software
 * ***********************************************************/
#ifndef PNLINFO_H
#define PNLINFO_H

#include "cmenue.h"
int initInfoPanExpert(cPanel* pan, tCoord lf);
int initInfoPanRecorder(cPanel* pan, tCoord lf);
int initInfoPanCompact(cPanel* pan, tCoord lf);

#endif // PNLBATS_H

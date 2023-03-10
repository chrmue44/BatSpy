/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * The panels to display recorded files as waterfall diagram
 * or time domain diagram
 * ***********************************************************/
#ifndef PNLWATERFALL_H
#define PNLWATERFALL_H
#include "cmenue.h"
int initFkeysWaterPan(cPanel* pan, tCoord lf);
int initWaterPan(cPanel* pan, tCoord lf);
int initTimePan(cPanel* pan, tCoord lf);
#endif // PNLWATERFALL_H

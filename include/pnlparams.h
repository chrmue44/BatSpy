/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * The panels for the parametrization of BatSpy
 * ***********************************************************/
#ifndef PNLPARAMS_H
#define PNLPARAMS_H
#include "cmenue.h"

int initParPan(cPanel* pan, tCoord lf);
int initParPanCompact(cPanel* pan, tCoord lf);
int initParRec(cPanel* pan, tCoord lf);
int initParRecCompact(cPanel* pan, tCoord lf, size_t parSet);
int initParTriggerCompact(cPanel* pan, tCoord lf, size_t parSet);
int initDateTimePan(cPanel* pan, tCoord lf);
int initDateTimePanCompact(cPanel* pan, tCoord lf);
int initPositionPan(cPanel* pan, tCoord lf);
void setPosFunc(cMenuesystem* pThis, enKey key, cParBase* pItem);
void displayModeFunc(cMenuesystem* pThis, enKey key, cParBase* pItem);
bool checkTwilight(enRecAuto r);
void calcSunrise();

#endif // PNLPARAMS_H

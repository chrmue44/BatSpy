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
int initParRec(cPanel* pan, tCoord lf);
int initDateTimePan(cPanel* pan, tCoord lf);
int initPositionPan(cPanel* pan, tCoord lf);

#endif // PNLPARAMS_H

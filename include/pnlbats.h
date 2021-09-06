/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}.com             *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * The panel that provides information about bat species
 * ***********************************************************/
#ifndef PNLBATS_H
#define PNLBATS_H

#include "cmenue.h"
void initBats();
int initBatPan(cPanel* pan, tCoord lf);

#endif // PNLBATS_H

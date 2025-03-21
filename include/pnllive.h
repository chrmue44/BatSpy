/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * The live display panel
 * ***********************************************************/
#ifndef PNLLIVE_H
#define PNLLIVE_H

#include "cmenue.h"
#include "pnlmain.h"

int initLivePanHandheld(cPanel* pan, tCoord lf);
int initLivePanRecorder(cPanel* pan, tCoord lf);
void setSampleRateLivePan(size_t parSet);
cParGraph* getLiveFft();

#endif // PNLLIVE_H

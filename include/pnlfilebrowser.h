/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * The panel that provides a file browser
 * ***********************************************************/
#ifndef PNLFILEBROWSER_H
#define PNLFILEBROWSER_H

#include "cmenue.h"

#define FILE_OFFS  3  //index offset of first file name
#define BTN_COUNT 2  //nr of buttons per file entry

int initFileBrowserPan(cPanel* pan, tCoord lf);
void initFileBrowser(cPanel* pan, const char* dir);
int initFkeyFilePanel(cPanel* pan, tCoord lf);


#endif // PNLFILEBROWSER_H

/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * The main panel
 * ***********************************************************/
#ifndef PANMAIN_H
#define PANMAIN_H
#include "cmenue.h"
#ifdef OWN_H
#define extern                 // trick to automatically declare vars in cpp file
#endif
extern thPanel fkeyMainPan;    ///< f-key panel for main screen
extern thPanel f2pan;
extern thPanel f3pan;
extern thPanel f4MainPan;
extern thPanel hdrMainPanel;
extern thPanel panFont;
extern thPanel panTime;        ///< panel to display time diagram
extern thPanel pnlLive;
extern thPanel panGeo;
extern thPanel panInfo;
extern thPanel fkeyWaterPan;   ///< f-key panel for waterfall screen
extern thPanel panWaterfall;
extern thPanel hdrPanWaterfall;
extern thPanel hdrBatInfo;
extern thPanel panFileBrowser;
extern thPanel fkeyFilePan;
extern thPanel panParams;      ///< panel for general parameter settings
extern thPanel panParRec;      ///< panel for recording settings
extern thPanel panParTrig;     ///< pnael for trigger settings
extern thPanel panPosition;    ///< panel for setting of position
extern thPanel panBats;        ///< panel for bat infos
extern thPanel panDateTime;    ///< panel to set time and date

extern cParEnum f1MainItems;   ///< pop up menu F1
extern cParEnum f4MainItems;   ///< pop up menu F4

#ifdef OWN_H
#undef extern
#endif

void setFileToDisplay(const char* buf);
void initFunctionItemsExpert();
void initFunctionItemsRecorder();
void initFunctionItemsHandheld();
void initFunctionsCompact();

int initFkeyPanel(cPanel* pan, tCoord lf);
int initCompactFkeyPanel(cPanel* pan, tCoord lf);
int initMainPanelExpert(cPanel* pan, tCoord lf);
int initMainPanelRecorder(cPanel* pan, tCoord lf);
int initMainPanelHandheld(cPanel* pan, tCoord lf);
int initMainPanelCompact(cPanel* pan, tCoord lf);

void f1Func(cMenuesystem* pThis, enKey key, cParBase* pItem);
void f4Func(cMenuesystem* pThis, enKey key, cParBase* pItem);
void setVisibilityRecCount(cMenuesystem* pThis);
void setGpsLog(cMenuesystem* pThis, bool on);
void setHeaderPanelText(cMenuesystem* pThis, thText t);
void enableEditPosition(cMenuesystem* pThis, bool on);
void enableEditTimes(cMenuesystem* pThis, bool on);

#endif // PANMAIN_H

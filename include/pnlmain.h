#ifndef PANMAIN_H
#define PANMAIN_H
#include "cmenue.h"

extern thPanel fkeyMainPan;
extern thPanel panGeo;
extern thPanel hdrMainPanel;
extern thPanel fkeyWaterPan;
extern thPanel panWaterfall;
extern thPanel hdrPanWaterfall;
extern thPanel panTime;
extern thPanel panFont;
extern thPanel panInfo;
extern thPanel panParams;
extern thPanel panBats;
extern thPanel panDateTime;

void setFileToDisplay(const char* buf);
void initFunctionItems();
int initFkeyPanel(cPanel* pan, tCoord lf);
int initMainPanel(cPanel* pan, tCoord lf);
void f1Func(cMenuesystem* pThis, tKey key);
void f4Func(cMenuesystem* pThis, tKey key);

#endif // PANMAIN_H

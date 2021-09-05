#ifndef PANMAIN_H
#define PANMAIN_H
#include "cmenue.h"
#ifdef OWN_H
#define extern
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
extern thPanel hdrParams;

extern thPanel panParams;      ///< panel for general parameter settings
extern thPanel panParRec;      ///< panel for recording settings
extern thPanel panPosition;    ///< panel for setting of position
extern thPanel panBats;        ///< panel for bat infos
extern thPanel panDateTime;    ///< panel to set time and date
#ifdef OWN_H
#undef extern
#endif

void setFileToDisplay(const char* buf);
void initFunctionItems();
int initFkeyPanel(cPanel* pan, tCoord lf);
int initMainPanel(cPanel* pan, tCoord lf);
void f1Func(cMenuesystem* pThis, enKey key);
void f4Func(cMenuesystem* pThis, enKey key);
void setVisibilityRecCount(cMenuesystem* pThis);


#endif // PANMAIN_H

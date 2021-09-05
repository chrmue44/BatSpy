#ifndef PNLLIVE_H
#define PNLLIVE_H

#include "cmenue.h"
#include "pnlmain.h"

int initLivePan(cPanel* pan, tCoord lf);
void setSampleRateLivePan();
cParGraph* getLiveFft();

#endif // PNLLIVE_H

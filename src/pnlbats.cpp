/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "pnlbats.h"
#include "cutils.h"
#include "debug.h"
#include "pnlmain.h"
#include "cmenue.h"

void batFunc(cMenuesystem* pThis, enKey key) {
  char line[512];
  char line2[512];
  size_t byteCount;
  cSdCard& sd = cSdCard::inst();
  tFILE file;
  enSdRes res = sd.openFile("/info/bat_info.tsv", file, READ);
  bool found = false;
  if(res == 0)
  {
    for(;;) {
      res = sd.readLine(file, line, sizeof(line), byteCount);
      line[byteCount] = 0;
      cUtils::replaceUTF8withInternalCoding(line, line2, sizeof(line2));
      if(res == 0) {
        if(strstr(line2, devStatus.bats.name.getActText()) != nullptr) {
          found = true;
          break;
        }
      }
      else
        break;
    }
    sd.closeFile(file);
    if(found)
    {
      char* token = strtok(line2, "\t");  //name
      token = strtok(nullptr, "\t");
      devStatus.bats.nameLat.set(token);
      token = strtok(nullptr, "\t");   //Kürzel
      token = strtok(nullptr, "\t");   //vorkommen
      devStatus.bats.occurrence.set(token);
      token = strtok(nullptr, "\t");   //Hauptfrequenz
      devStatus.bats.freq.set(token);
      token = strtok(nullptr, "\t");   //Ruflaenge
      devStatus.bats.callLen.set(token);
      token = strtok(nullptr, "\t");   //Rufabstand
      devStatus.bats.callDist.set(token);
      token = strtok(nullptr, "\t");   //Characteristik
      devStatus.bats.characteristic.set(token);
      token = strtok(nullptr, "\t");   //Comments
      devStatus.bats.comment.set(token);
      token = strtok(nullptr, "\t");   //Skiba
      token = strtok(nullptr, "\t");   //Bild
      token = strtok(nullptr, "\t");   //Groesse
      devStatus.bats.size.set(token);
      token = strtok(nullptr, "\t");   //Spannweite
      devStatus.bats.wingSpan.set(token);
      token = strtok(nullptr, "\t");   //Gewicht
      devStatus.bats.weight.set(token);
      token = strtok(nullptr, "\t");   //Lebensraum
      devStatus.bats.habitat.set(token);
      menue.refreshHdrPanel();
    }
  }
}


int initBatPan(cPanel* pan, tCoord lf){
  int err = pan->addTextItem(1200,                        5,  20,           60, lf);
  err |= pan->addListItem(&devStatus.bats.name,      120,  20,          180, lf, true, batFunc);
  err |= pan->addTextItem(1202,                        5,  20 + 1 * lf,  60, lf);
  err |= pan->addStrItem(&devStatus.bats.nameLat,    120,  20 + 1 * lf, 180, lf);
  err |= pan->addTextItem(1204,                        5,  20 + 2 * lf,  60, lf);
  err |= pan->addStrItem(&devStatus.bats.freq,       120,  20 + 2 * lf,  70, lf);
  err |= pan->addTextItem(1210,                        5,  20 + 3 * lf,  60, lf);
  err |= pan->addStrItem(&devStatus.bats.callLen,    120,  20 + 3 * lf,  70, lf);
  err |= pan->addTextItem(1212,                        5,  20 + 4 * lf,  60, lf);
  err |= pan->addStrItem(&devStatus.bats.callDist,   120,  20 + 4 * lf,  70, lf);
  err |= pan->addTextItem(1215,                        5,  20 + 5 * lf,  60, lf);
  err |= pan->addStrItem(&devStatus.bats.characteristic,120,  20 + 5 * lf, 200, lf);
  err |= pan->addTextItem(1217,                        5,  20 + 6 * lf,  60, lf);
  err |= pan->addStrItem(&devStatus.bats.occurrence, 120,  20 + 6 * lf, 200, lf);
  err |= pan->addTextItem(1220,                        5,  20 + 7 * lf,  60, lf);
  err |= pan->addStrItem(&devStatus.bats.wingSpan,   120,  20 + 7 * lf, 200, lf);
  err |= pan->addTextItem(1225,                        5,  20 + 8 * lf,  60, lf);
  err |= pan->addStrItem(&devStatus.bats.weight,     120,  20 + 8 * lf, 200, lf);
  err |= pan->addTextItem(1230,                        5,  20 + 9 * lf,  60, lf);
  err |= pan->addStrItem(&devStatus.bats.habitat,    120,  20 + 9 * lf, 200, lf);
  err |= pan->addTextItem(1235,                        5,  20 + 11 * lf,  60, lf);
  err |= pan->addStrItem(&devStatus.bats.comment,    120,  20 + 11 * lf, 200, lf);
  return err;
}

void initBats() {
  char line[512];
  char line2[512];
  size_t byteCount;
  cSdCard& sd = cSdCard::inst();
  tFILE file;
  enSdRes res = sd.openFile("/info/bat_info.tsv", file, READ);
  if(res == 0)
  {
    devStatus.bats.name.clear();
    // read and forget first two header
    res = sd.readLine(file, line, sizeof(line), byteCount);
    if(res == 0) {
      for(;;) {
        line[0] = 0;
        res = sd.readLine(file, line, sizeof(line), byteCount);
        if(res == 0) {
          line[byteCount] = 0;
          cUtils::replaceUTF8withInternalCoding(line, line2, sizeof(line2));
          char* token = strtok(line2, "\t");
          DPRINTF1("token: %s", token);
          devStatus.bats.name.addItem(token);
        }
        else
          break;
      }
    }
    sd.closeFile(file);
  }
}

/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * https://www.astronomie.info/zeitgleichung/neu.html        *
 * ***********************************************************/
#ifndef CSUNRISE_H
#define CSUNRISE_H


class cSunRise
{
public:
  static const double pi2;
  static const double pi;
  static const double RAD;
  static void getSunSetSunRise(double lat, double lon, int year, int month, int day, int&srH, int&srM, int& ssH, int&ssM);

private:
  static double JulianischesDatum ( int Jahr, int Monat, int Tag, int Stunde, int Minuten, double Sekunden );
  static double InPi(double x);
   // Neigung der Erdachse
  static double eps(double T);
  static double BerechneZeitgleichung(double &DK ,double T);
  static void adjustHm(int& h, int& min);
  static void adjustT(double& t);

  //https://github.com/simsum/DTutils/blob/master/DTutils.cpp
  // Osterkennzahl berechnen, wird für weitere Berechnungen benötigt
  // gültig bis 2199
  // siehe auch http://manfred.wilzeck.de/Datum_berechnen.html
  static int EasterCode(int Year);
  // TRUE/HIGH wenn Sommerzeit
  static bool DaylightSaving (int Year, int Month, int Day, int Hour, int Minute);
  // ( 05.01.2012 = 5 .... 31.12.2012 = 366)
  static int DayOfYear (int Year, int Month, int Day);
  static int StartDateDaylightSaving(int Year);
  static int EndDateDaylightSaving(int Year);
  // Schaltjahr aktiv (1 = Ja)
  static int LeapYear (int Year);


};

#endif // CSUNRISE_H

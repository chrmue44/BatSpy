/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * https://www.astronomie.info/zeitgleichung/neu.html        *
 * ***********************************************************/

#include "csunrise.h"
#include <math.h>

const double cSunRise::pi2=6.283185307179586476925286766559;
const double cSunRise::pi=3.1415926535897932384626433832795;
const double cSunRise::RAD = 0.017453292519943295769236907684886;


// Gregorianischer Kalender
double cSunRise::JulianischesDatum (int Jahr, int Monat, int Tag, int Stunde, int Minuten, double Sekunden )
{
  int   Gregor;
  if (Monat<=2)
  {
    Monat=Monat +12;
    Jahr = Jahr -1;
  }

  Gregor = (Jahr/400)-(Jahr/100)+(Jahr/4);  // Gregorianischer Kalender

  return 2400000.5+365.0*Jahr - 679004.0 + Gregor
  + int(30.6001*(Monat+1)) + Tag + Stunde/24.0
  + Minuten/1440.0 + Sekunden/86400.0;
}

double cSunRise::InPi(double x)
{
  int n = (int)(x/pi2);
  x = x - n*pi2;
  if (x<0)
    x +=pi2;
  return x;
}


double cSunRise::eps(double T) // Neigung der Erdachse
{
  return RAD*(23.43929111 + (-46.8150*T - 0.00059*T*T + 0.001813*T*T*T)/3600.0);
}



double cSunRise::BerechneZeitgleichung(double &DK ,double T)
{
  double RA_Mittel = 18.71506921 + 2400.0513369*T +(2.5862e-5 - 1.72e-9*T)*T*T;
  double M  = InPi(pi2 * (0.993133 + 99.997361*T));
  double L  = InPi(pi2 * (  0.7859453 + M/pi2
  + (6893.0*sin(M)+72.0*sin(2.0*M)+6191.2*T) / 1296.0e3));
  double e = eps(T);
  double RA = atan(tan(L)*cos(e));
  if (RA < 0.0)
    RA+=pi;
  if (L > pi) RA+=pi;

  RA = 24.0*RA/pi2;
  DK = asin(sin(e)*sin(L));

  // Damit 0<=RA_Mittel<24
  RA_Mittel = 24.0*InPi(pi2*RA_Mittel/24.0)/pi2;

  double dRA = RA_Mittel - RA;

  if (dRA < -12.0)
    dRA+=24.0;

  if (dRA > 12.0)
    dRA-=24.0;

  dRA = dRA* 1.0027379;
  return dRA;
}



void cSunRise::getSunSetSunRise(double lat, double lon, int year, int month, int day, int&srH, int&srM, int& ssH, int&ssM)
{
  double JD2000 = 2451545.0;
  double JD;
  JD = JulianischesDatum(year,month,day,12,0,0); // Testdatum

  double T = (JD - JD2000)/36525.0;
  double DK;
  double h = -50.0/60.0 * RAD;
  double B = lat * RAD; // geographische Breite

//double Zeitzone = 0; //Weltzeit

  double Zeitzone = 1 + DaylightSaving(year, month, day, 12, 0);
//  double Zeitzone = 2.0;   //Sommerzeit
  double Zeitgleichung = BerechneZeitgleichung(DK,T);
  double Minuten = Zeitgleichung*60.0;
  double Zeitdifferenz = 12.0*acos((sin(h) - sin(B)*sin(DK)) / (cos(B)*cos(DK)))/pi;
  double AufgangOrtszeit = 12.0 - Zeitdifferenz - Zeitgleichung;
  double UntergangOrtszeit = 12.0 + Zeitdifferenz - Zeitgleichung;
  double AufgangWeltzeit = AufgangOrtszeit - lon /15.0;
  double UntergangWeltzeit = UntergangOrtszeit - lon /15.0;
  double Aufgang = AufgangWeltzeit + Zeitzone;         // In Stunden
  adjustT(Aufgang);

  double Untergang = UntergangWeltzeit + Zeitzone;
  adjustT(Untergang);

  srM = int(60.0*(Aufgang - (int)Aufgang)+0.5);
  srH = (int)Aufgang;
  adjustHm(srH, srM);

  ssM = int(60.0*(Untergang - (int)Untergang)+0.5);
  ssH = (int)Untergang;
  adjustHm(ssH, ssM);
}

void cSunRise::adjustT(double& t)
{
  if (t < 0.0)
    t +=24.0;
  else if (t >= 24.0)
    t -=24.0;
}

void cSunRise::adjustHm(int& h, int& min)
{
  if (min >= 60)
  {
    min-=60;
    h++;
  }
  else if (min < 0)
  {
    min+=60;
    h--;
    if (min < 0)
      h+=24;
  }
}

int cSunRise::EasterCode(int Year)
{
  int a;    //Hilfvariable
  int b;    //Hilfvariable
  int c;    //Hilfvariable
  int e;    //Osterkennzahl
  a = (Year % 19 * 19 + 24) % 30;
  b = 120 + a - (a / 27);
  c = (b + (Year * 5 / 4) - (Year / 2100)) % 7;
  e = b - c;
  return e;
}

//  Die Funktion DaylightSaving überprüft, ob im Augenblick Sommerzeit herrscht, oder nicht.
//  Sommerzeit wird aufgrund von UTC (Weltzeit) berechnet.

bool cSunRise::DaylightSaving (int Year, int Month, int Day, int Hour, int Minute)                    // TRUE/HIGH wenn Sommerzeit
{
    bool active;
    long StartDSInM   = (DayOfYear(Year, 3, StartDateDaylightSaving(Year)) *1440) + 120;    // Start Sommerzeit in Minuten
    long EndDSInM     = (DayOfYear(Year, 10, EndDateDaylightSaving(Year)) * 1440) + 180;    // Ende Sommerzeit in Minuten
    long NowInMinute  = (DayOfYear(Year, Month, Day) * 1440) + (Hour * 60) + Minute;        // aktuelle Minuten im Jahr

    active = NowInMinute >= StartDSInM && NowInMinute <= EndDSInM;                          // Ausgang TRUE wenn Zeitraum innerhalb Sommerzeit
    return active;
}

//  Die Funktion DAYOFYEAR berechnet den Tag des Jahres aus dem Eingangsdatum.

int cSunRise::DayOfYear (int Year, int Month, int Day)      // ( 05.01.2012 = 5 .... 31.12.2012 = 366)
{
    int sj = LeapYear(Year);
    int DayNbr = Day + 489 * Month / 16 - (7 + Month) / 10 * (2 - sj) -30 ;
    return DayNbr;
}


// Tag Start Sommerzeit (02:00 Uhr/März) - Schaltjahre werden berücksichtigt

int cSunRise::StartDateDaylightSaving(int Year)
{
    int e = EasterCode(Year);
    int Day;
    Day = 25 + (e + 2) % 7;
    return Day;
}

// Tag Ende Sommerzeit (03:00 Uhr/Oktober)

int cSunRise::EndDateDaylightSaving(int Year)
{
    int e = EasterCode(Year);
    int Day;
    Day = 25 + (e + 5) % 7;
    return Day;
}

//  Die Funktion LEAPYEAR testet, ob das Eingangsjahr ein Schaltjahr ist und gibt gegebenenfalls TRUE bzw. "1" aus.
//  Gültig für 1901 bis 2099
int cSunRise::LeapYear (int Year)                     // Schaltjahr aktiv (1 = Ja)
{
    int sj = (1/(1 + Year % 4)) - (1/(1 + Year % 100)) + (1/(1 + Year % 400));
    return sj;
}
// Vergleich mit CalSky.com

// Aufgang        :  7h18.4m Untergang      : 19h00.6m

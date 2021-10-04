/*********************************************************
 *         project: ProcessMon                           *
 *                                                       *
 *              (C) 2010 Christian Mueller               *
 *                       Odenwaldstrasse 134g            *
 *                       D-64372 Ober-Ramstadt           *
 *                                                       *
 *********************************************************/

#ifndef clTimeUtils_H_HEADER_INCLUDED_BDB6E3D8
#define clTimeUtils_H_HEADER_INCLUDED_BDB6E3D8

#include "typedef.h"

#include <sstream>
#include <chrono>

 #define PTW32_TIMESPEC_TO_FILETIME_OFFSET \
	  ( ((LONGLONG) 27111902 << 32) + (LONGLONG) 3577643008 )

 #include <sys/time.h>


// Liefert Systemzeiten fuer Wartezeiten bei cond_timedwait, Logging etc.
class clTimeUtils
{
public:
  // schlaeft fuer x ms
  static void sleep_ms(uint32_t Time);
  // schlaeft fuer x sec
  static void sleep_s(uint32_t Time);

  enum tTimeCompare { SYSTIME_EQUAL = 0, SYSTIME_LATER = 1, SYSTIME_EARLIER = 2 };

  // Liefert die Systemzeit in Form einer timespec-Struktur z.B. fuer
  // pthreads_cond_timedwait Aufrufe.
  // Unter Linux wird hierzu die Funktion gettimeofday verwendet, unter Windows
  // spezielle WINAPI Funktionen (entlehnt aus pthreads-Implemetierung auf
  // Windows), so dass es auch unter Win-CE funktionieren wuerde.
  inline static int32_t getSysTime(struct timespec* Time)
  {
      struct timeval timev;
      struct timezone timez;

      gettimeofday(&timev,&timez);
      
      PM_ASSERT(timev.tv_usec<1000000,"us greater than 1000000");
      Time->tv_sec = timev.tv_sec;
      Time->tv_nsec = timev.tv_usec * 1000;
      return 0;
  }

  // Liefert die Systemzeit von jetzt an gerechnet in der uebergebenen Anzahl an
  // Millisekunden  in Form einer timespec-Struktur z.B. fuer
  // pthreads_cond_timedwait Aufrufe.
  inline static int32_t getFutureSysTime(
    // Zeiger auf die Struktur, in die die ermittelte Zeit abgelegt werden
    // soll.
    struct timespec* futureTime, 
    // Wie lange soll der gelieferte Zeitpunkt in der Zukunft liegen?
    tTime_ms millisOffset)
  {
    int32_t err;
    

    err = getSysTime(futureTime);

    if(err == 0)
    {
      //Sekunden zu den Sekunden addieren
      if(millisOffset >= 1000) 
      {
        uint32_t secOffset;
        secOffset = millisOffset / 1000; //Sekunden hinzuzaehlen
        millisOffset %= 1000; //restliche Millisekunden
        if((MAX_UINT32 - futureTime->tv_sec) >= secOffset) //Achtung �berlauf!
          futureTime->tv_sec += secOffset;
        else
        {
          err = -1;
          PM_ASSERT(false,"clTimeUtils: Overrun adding seconds");
        }
      }
      //Millisekunden zu den Nanosekunden addieren
      int32_t nanosOffset = millisOffset * 1000000;
      if((1000000000 - futureTime->tv_nsec) <= nanosOffset) //Achtung �berlauf!)
      {
        futureTime->tv_sec += 1; //eine Sekunde weiterdrehen => �bertrag
        futureTime->tv_nsec -= 1000000000 - nanosOffset;  //nanos �berhang
      }
      else
      {  
        futureTime->tv_nsec += nanosOffset; //millis in nanos umgerechnet addieren
      }
    }
    return err;
  }
  // Vergleich zweier System-Zeiten.
  // 
  // Rueckgabe:
  // SYSTIME_EQUAL: Zeiten sind gleich
  // SYSTIME_LATER: time1 ist spaeter als time2
  // SYSTEM_EARLIER: time1 ist frueher als time2
  static inline tTimeCompare compareSysTimes(
    // Zeiger auf die Struktur, in die die ermittelte Zeit abgelegt werden
    // soll.
    const struct timespec* time1, 
    // Zeiger auf die Struktur, in die die ermittelte Zeit abgelegt werden
    // soll.
    const struct timespec* time2)
  {
    if(time1->tv_sec > time2->tv_sec)
      return SYSTIME_LATER;
    else if(time1->tv_sec < time2->tv_sec)
      return SYSTIME_EARLIER;
    else //in der gleichen Sekunde!
    {
      if(time1->tv_nsec > time2->tv_nsec)
        return SYSTIME_LATER;
      else if(time1->tv_nsec < time2->tv_nsec)
        return SYSTIME_EARLIER;
      else //wirklich gleich!!
        return SYSTIME_EQUAL;
    }
  }
  // Liefert die Systemzeit in Form einer timespec-Struktur z.B. fuer
  // pthreads_cond_timedwait Aufrufe.
  // Unter Linux wird hierzu die Funktion gettimeofday verwendet, unter Windows
  // spezielle WINAPI Funktionen (entlehnt aus pthreads-Implemetierung auf
  // Windows), so dass es auch unter Win-CE funktionieren wuerde.
  static inline void getTimeString(bool withDate,char *Str, size_t size)
  {
      struct timeval timev;
      struct timezone timez;

      gettimeofday(&timev,&timez);
      auto now = chrono::system_clock::now();
      std::time_t end_time = std::chrono::system_clock::to_time_t(now);
      strncpy(Str, std::ctime(&end_time), size);

      snprintf(Str, sizeof (Str),"%l:%l",timev.tv_sec, timev.tv_usec);
/*      ultoa(timev.tv_sec,Str,10);
      strcat(Str,":");
      ultoa(timev.tv_usec,Str+strlen(Str),10); */
  }

  // Liefert die Systemzeit in ms als Double-Wert
  static int32_t getSysTime(double& Time);

  // Rechnet einen Sekundenwert mit Nachkomma ins tTimeStamp-Format um
  //  return 0, wenn seconds < 0
  static tTimeStamp calcDoubleToTimeStamp(double Seconds);

};






#endif /* clTimeUtils_H_HEADER_INCLUDED_BDB6E3D8 */


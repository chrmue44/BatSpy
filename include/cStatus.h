#include "stdint.h"

struct stStatusItem 
{
  bool on;
  int ms;
}; 

enum enStatLed1
{
  IDLE = 0,
  ON,
  ON_100,
  ON_75,
  ON_50,
  ON_25  
};

class cStatus
{
  private:
   enStatLed1 _statusLed1;
   int _subStateLed1;
   uint32_t _lastT = 0;
   uint32_t _nextEndTime;
   stStatusItem* _psubStateLed1;
   stStatusItem* _psubStateStart1;

  public:
   cStatus() {}
   void show();
   void setStateLed1(enStatLed1 state);

  private:
   void setNewSubState();
};
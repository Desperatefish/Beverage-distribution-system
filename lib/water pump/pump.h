#ifndef __PUMP__H__
#define __PUMP__H__

#include <Arduino.h>

#define relay1  D5
#define relay2  D6
#define relay3  D7
#define relay4  D8

extern void Init_Relay();
extern void Relay1_Run();

#endif

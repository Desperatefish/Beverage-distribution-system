#include "pump.h"

void Init_Relay()
{
pinMode(relay1, OUTPUT);	
pinMode(relay2, OUTPUT);
pinMode(relay3, OUTPUT);
pinMode(relay4, OUTPUT);
}

void Relay1_Run()
{
digitalWrite(relay1, HIGH);
delay(2000);
digitalWrite(relay1, LOW);
}	
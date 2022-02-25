#ifndef __OLED__H__
#define __OLED__H__

#include <Arduino.h>

#define HX711_DT D3
#define HX711_SCK D4

extern void Init_Hx711();
extern unsigned long HX711_Read(void);
extern long Get_Weight();
extern void Get_Maopi();

#endif

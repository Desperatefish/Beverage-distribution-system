#ifndef __OLED__H__
#define __OLED__H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <time.h>                       
#include <sys/time.h>                  
#include <coredecls.h>      

//#include "SH1106Wire.h"   //1.3寸用这个
#include "SSD1306Wire.h"    //0.96寸用这个
#include "OLEDDisplayUi.h"
#include "HeFeng.h"
#include "WeatherStationFonts.h"
#include "WeatherStationImages.h"

void oled_init();
void oled_loop();

#endif

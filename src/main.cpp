#include <Arduino.h>
//#include "HX711.h"
#include "pump.h"
#include "oled.h"
#include <Wire.h>
#include <Adafruit_MLX90614.h>


float Weight = 0;     //称重值初始化
Adafruit_MLX90614 mlx = Adafruit_MLX90614();


#define BLINKER_PRINT Serial
#define BLINKER_WIFI

#include <Blinker.h>

char auth[] = "66b44e42cba1";
//char ssid[] = "TP-LINK_2011";
//char pswd[] = "20112011";

char ssid[] = "9Pro";
char pswd[] = "13451812385";

// 新建组件对象
BlinkerButton Button1("btn-abc");
BlinkerNumber Number1("num-abc");

int counter = 0;

// 按下按键即会执行该函数
void button1_callback(const String &state)
{
    BLINKER_LOG("get button state: ", state);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

// 如果未绑定的组件被触发，则会执行其中内容
void dataRead(const String &data)
{
    BLINKER_LOG("Blinker readString: ", data);
    counter++;
    Number1.print(counter);
}

    void setup()
    {
        // 初始化串口
     	Serial.begin(9600);	
	Serial.print("Welcome to use!\n");
//  Init_Hx711();				//初始化HX711模块连接的IO设置
//    Init_Relay();
	mlx.begin();  
    oled_init();
        BLINKER_DEBUG.stream(Serial);
        BLINKER_DEBUG.debugAll();

        // 初始化有LED的IO
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, HIGH);
        // 初始化blinker
        Blinker.begin(auth, ssid, pswd);
        Blinker.attachData(dataRead);

        Button1.attach(button1_callback);

    
    }

    void loop()
    {
        Blinker.run();
        oled_loop();
        /*	Weight = Get_Weight();	//计算放在传感器上的重物重量
    Serial.print("物重：");
	Serial.print(float(Weight/1000),3);	//串口显示重量
	Serial.print(" kg\n");	//显示单位
	Serial.print("\n");		//显示单位
	delay(2000);				//延时2s
*/   
 //   Relay1_Run();
    Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC()); 
    Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
    Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF()); 
    Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");
	delay(2000);		

    }
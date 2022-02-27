 //把远程下载下来的代码合并到本地仓库，远程的和本地的合并
 // git merge origin / master

#include <blinker_user.h>
#include <Arduino.h>
 //#include "HX711.h"
#include "pump.h"
#include "oled.h"
#include <Wire.h>
#include <Adafruit_MLX90614.h>

 float Weight = 0; //称重值初始化
Adafruit_MLX90614 mlx = Adafruit_MLX90614();


    void setup()
    {
        // 初始化串口
     	Serial.begin(9600);	
	    Serial.print("Welcome to use!\n");
        blinker_init();
        //  Init_Hx711();				//初始化HX711模块连接的IO设置
        //    Init_Relay();
        mlx.begin();
        oled_init();
    
    
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
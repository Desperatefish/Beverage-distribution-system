#include "oled.h"
/***************************
   Begin Settings
 **************************/

const char *WIFI_SSID = "TP-LINK_2011"; //填写你的WIFI名称及密码
const char *WIFI_PWD = "20112011";

//const char* WIFI_SSID = "9Pro";  //填写你的WIFI名称及密码
//const char* WIFI_PWD = "13451812385";

const char* BILIBILIID = "34037524";  //填写你的B站账号

//由于太多人使用我的秘钥，导致获取次数超额，所以不提供秘钥了，大家可以到https://dev.heweather.com/获取免费的
const char* HEFENG_KEY = "591622de8b5a4b96ab2ab3ceba06ca1b";//填写你的和风天气秘钥
const char* HEFENG_LOCATION = "CN101190104";//填写你的城市ID,可到https://where.heweather.com/index.html查询
//const char* HEFENG_LOCATION = "auto_ip";//自动IP定位

#define TZ              8      // 中国时区为8
#define DST_MN          0      // 默认为0

const int UPDATE_INTERVAL_SECS = 5 * 60; // 5分钟更新一次天气
const int UPDATE_CURR_INTERVAL_SECS = 2 * 59; // 2分钟更新一次粉丝数

const int I2C_DISPLAY_ADDRESS = 0x3c;  //I2c地址默认
//#if defined(ESP8266)
//const int SDA_PIN = 8;  //引脚连接
//const int SDC_PIN = 9;  //
//#endif
#define SDA_PIN 0
#define SDC_PIN 2

const String WDAY_NAMES[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};  //星期
const String MONTH_NAMES[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};  //月份


/***************************
   End Settings
 **************************/
 
//SH1106Wire     display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN);   // 1.3寸用这个
SSD1306Wire     display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN);   // 0.96寸用这个
OLEDDisplayUi   ui( &display );

HeFengCurrentData currentWeather; //实例化对象
HeFengForeData foreWeather[3];
HeFeng HeFengClient;

#define TZ_MN           ((TZ)*60)   //时间换算
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)

time_t now; //实例化时间

bool readyForWeatherUpdate = false; // 天气更新标志
bool first = true;  //首次更新标志
long timeSinceLastWUpdate = 0;    //上次更新后的时间
long timeSinceLastCurrUpdate = 0;   //上次天气更新后的时间

String fans = "-1"; //粉丝数

void drawProgress(OLEDDisplay *display, int percentage, String label);   //提前声明函数
void updateData(OLEDDisplay *display);
void updateDatas(OLEDDisplay *display);
void drawDateTime(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawCurrentWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawForecast(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawForecastDetails(OLEDDisplay *display, int x, int y, int dayIndex);
void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state);
void setReadyForWeatherUpdate();

//添加框架
//此数组保留指向所有帧的函数指针
//框架是从右向左滑动的单个视图
FrameCallback frames[] = { drawDateTime, drawCurrentWeather, drawForecast };
//页面数量
int numberOfFrames = 3;

OverlayCallback overlays[] = { drawHeaderOverlay }; //覆盖回调函数
int numberOfOverlays = 1;  //覆盖数

void wificonnect() {  //WIFI密码连接，Web配网请注释
  WiFi.begin(WIFI_SSID, WIFI_PWD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(80);
    display.clear();
    display.drawXbm(34, 0, bili_Logo_width, bili_Logo_height, bili_Logo_5);
    display.display();
    delay(80);
    display.clear();
    display.drawXbm(34, 0, bili_Logo_width, bili_Logo_height, bili_Logo_6);
    display.display();
    delay(80);
    display.clear();
    display.drawXbm(34, 0, bili_Logo_width, bili_Logo_height, bili_Logo_7);
    display.display();
    delay(80);
    display.clear();
    display.drawXbm(34, 0, bili_Logo_width, bili_Logo_height, bili_Logo_8);
    display.display();
    delay(80);
    display.clear();
    display.drawXbm(34, 0, bili_Logo_width, bili_Logo_height, bili_Logo_1);
    display.display();
    delay(80);
    display.clear();
    display.drawXbm(34, 0, bili_Logo_width, bili_Logo_height, bili_Logo_2);
    display.display();
    delay(80);
    display.clear();
    display.drawXbm(34, 0, bili_Logo_width, bili_Logo_height, bili_Logo_3);
    display.display();
    delay(80);
    display.clear();
    display.drawXbm(34, 0, bili_Logo_width, bili_Logo_height, bili_Logo_4);
    display.display();
  }
  Serial.println("");
  delay(500);
}


void drawProgress(OLEDDisplay *display, int percentage, String label) {    //绘制进度
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(64, 10, label);
  display->drawProgressBar(2, 28, 124, 10, percentage);
  display->display();
}

void updateData(OLEDDisplay *display) {  //天气更新
  HeFengClient.doUpdateCurr(&currentWeather, HEFENG_KEY, HEFENG_LOCATION);
  HeFengClient.doUpdateFore(foreWeather, HEFENG_KEY, HEFENG_LOCATION);
  readyForWeatherUpdate = false;
}

void updateDatas(OLEDDisplay *display) {  //首次天气更新
  drawProgress(display, 0, "Updating fansnumb...");
  HeFengClient.fans(&currentWeather, BILIBILIID);
  fans = String(currentWeather.follower);
  
  drawProgress(display, 33, "Updating weather...");
  HeFengClient.doUpdateCurr(&currentWeather, HEFENG_KEY, HEFENG_LOCATION);
  
  drawProgress(display, 66, "Updating forecasts...");
  HeFengClient.doUpdateFore(foreWeather, HEFENG_KEY, HEFENG_LOCATION);
  
  readyForWeatherUpdate = false;
  drawProgress(display, 100, "Done...");
  delay(200);
  
}

void drawDateTime(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {  //显示时间
  now = time(nullptr);
  struct tm* timeInfo;
  timeInfo = localtime(&now);
  char buff[16];

  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);
  String date = WDAY_NAMES[timeInfo->tm_wday];

  sprintf_P(buff, PSTR("%04d-%02d-%02d  %s"), timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday, WDAY_NAMES[timeInfo->tm_wday].c_str());
  display->drawString(64 + x, 5 + y, String(buff));
  display->setFont(ArialMT_Plain_24);

  sprintf_P(buff, PSTR("%02d:%02d:%02d"), timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
  display->drawString(64 + x, 22 + y, String(buff));
  display->setTextAlignment(TEXT_ALIGN_LEFT);
}

void drawCurrentWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {  //显示天气
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 38 + y, currentWeather.cond_txt + "    |   Wind: " + currentWeather.wind_sc + "  ");

  display->setFont(ArialMT_Plain_24);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  String temp = currentWeather.tmp + "°C" ;
  display->drawString(60 + x, 3 + y, temp);
  display->setFont(ArialMT_Plain_10);
  display->drawString(62 + x, 26 + y, currentWeather.fl + "°C | " + currentWeather.hum + "%");
  display->setFont(Meteocons_Plain_36);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(32 + x, 0 + y, currentWeather.iconMeteoCon);
}

void drawForecast(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {  //天气预报
  drawForecastDetails(display, x, y, 0);
  drawForecastDetails(display, x + 44, y, 1);
  drawForecastDetails(display, x + 88, y, 2);
}

void drawForecastDetails(OLEDDisplay *display, int x, int y, int dayIndex) {  //天气预报

  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(x + 20, y, foreWeather[dayIndex].datestr);
  display->setFont(Meteocons_Plain_21);
  display->drawString(x + 20, y + 12, foreWeather[dayIndex].iconMeteoCon);

  String temp = foreWeather[dayIndex].tmp_min + " | " + foreWeather[dayIndex].tmp_max;
  display->setFont(ArialMT_Plain_10);
  display->drawString(x + 20, y + 34, temp);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
}

void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {   //绘图页眉覆盖
  now = time(nullptr);
  struct tm* timeInfo;
  timeInfo = localtime(&now);
  char buff[14];
  sprintf_P(buff, PSTR("%02d:%02d"), timeInfo->tm_hour, timeInfo->tm_min);

  display->setColor(WHITE);
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(6, 54, String(buff));
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  String temp = fans;
  display->drawString(122, 54, temp);
  display->drawHorizontalLine(0, 52, 128);
}

void setReadyForWeatherUpdate() {  //为天气更新做好准备
  Serial.println("Setting readyForUpdate to true");
  readyForWeatherUpdate = true;
}
void oled_init() {
  // 屏幕初始化
  display.init();
  display.clear();
  display.display();

  display.flipScreenVertically(); //屏幕翻转
  display.setContrast(255); //屏幕亮度
  
  // 用固定密码连接，Web配网请注释
  wificonnect();

  ui.setTargetFPS(30);  //刷新频率

  ui.setActiveSymbol(activeSymbole); //设置活动符号
  ui.setInactiveSymbol(inactiveSymbole); //设置非活动符号

  // 符号位置
  // 你可以把这个改成TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);

  // 定义第一帧在栏中的位置
  ui.setIndicatorDirection(LEFT_RIGHT);

  // 屏幕切换方向
  // 您可以更改使用的屏幕切换方向 SLIDE_LEFT, SLIDE_RIGHT, SLIDE_TOP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  ui.setFrames(frames, numberOfFrames); // 设置框架
  ui.setTimePerFrame(5000); //设置切换时间
  
  ui.setOverlays(overlays, numberOfOverlays); //设置覆盖

  // UI负责初始化显示
  ui.init();
  display.flipScreenVertically(); //屏幕反转

  configTime(TZ_SEC, DST_SEC, "ntp.ntsc.ac.cn", "ntp1.aliyun.com"); //ntp获取时间，你也可用其他"pool.ntp.org","0.cn.pool.ntp.org","1.cn.pool.ntp.org","ntp1.aliyun.com"
  delay(200);

}

void oled_loop() 
{
  if (first) {  //首次加载
    updateDatas(&display);
    first = false;
  }
  if (millis() - timeSinceLastWUpdate > (1000L * UPDATE_INTERVAL_SECS)) { //屏幕刷新
    setReadyForWeatherUpdate();
    timeSinceLastWUpdate = millis();
  }
  if (millis() - timeSinceLastCurrUpdate > (1000L * UPDATE_CURR_INTERVAL_SECS)) { //粉丝数更新
    HeFengClient.fans(&currentWeather, BILIBILIID);
    fans = String(currentWeather.follower);
    timeSinceLastCurrUpdate = millis();
  }

  if (readyForWeatherUpdate && ui.getUiState()->frameState == FIXED) { //天气更新
    updateData(&display);
  }

  int remainingTimeBudget = ui.update(); //剩余时间预算

  if (remainingTimeBudget > 0) {
    //你可以在这里工作如果你低于你的时间预算。
    delay(remainingTimeBudget);
  }

}

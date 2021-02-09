#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <ESP8266WiFi.h>
#include "NTP.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h> //  ESP8266WebServer库
#include <ESP8266mDNS.h>
#include "WiFiManager.h" //https://github.com/tzapu/WiFiManager
#include <Time.h>
#include <Timezone.h>

// ST7789 TFT module connections
#define TFT_DC 5   // TFT DC  pin is connected to NodeMCU pin D1 (GPIO5)
#define TFT_RST 16 // TFT RST pin is connected to NodeMCU pin D0 (GPIO16)
#define TFT_CS 15  // TFT CS  pin is connected to NodeMCU pin D8 (GPIO15)
// initialize ST7789 TFT library with hardware SPI module
// SCK (CLK) ---> NodeMCU pin D5 (GPIO14)
// MOSI(DIN) ---> NodeMCU pin D7 (GPIO13)
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
WiFiManager wifiManager;
float p = 3.1415926;

//函数声明
void tftPrintTest();
void configModeCallback(WiFiManager *myWiFiManager);
void webInit();
void myMDNSinit();

// 北京时间时区
#define STD_TIMEZONE_OFFSET +8
TimeChangeRule mySTD = {"", First, Sun, Jan, 0, STD_TIMEZONE_OFFSET * 60};
Timezone myTZ(mySTD, mySTD);

ESP8266WebServer esp8266_server(80);

void setup(void)
{
  Serial.begin(115200);

  // if the display has CS pin try with SPI_MODE0
  tft.init(240, 240, SPI_MODE2); // 改成你屏幕的大小

  // if the screen is flipped, remove this command
  tft.setRotation(2);
  tft.fillScreen(ST77XX_BLACK);
  delay(500);
  Serial.println(F("TFT init"));

  wifiManager.setAPCallback(configModeCallback);
  if (!wifiManager.autoConnect("EspDisplay"))
  {
    Serial.println("wifi connected fail...");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }
  Serial.println("wifi connected success...");
  myMDNSinit();
  Serial.println("dns init...");

  initNTP();
  Serial.println("ntp init...");

  // tft print function!
  // tftPrintTest();
  delay(4000);
}

void loop()
{
  MDNS.update();
  esp8266_server.handleClient(); // 处理http服务器访问

  TimeChangeRule *tcr; // Pointer to the time change rule

  // Read the current UTC time from the NTP provider
  time_t utc = now();

  // Convert to local time taking DST into consideration
  time_t localTime = myTZ.toLocal(utc, &tcr);

  // Map time to pixel positions
  int weekdays = weekday(localTime);
  int days = day(localTime);
  int months = month(localTime);
  int years = year(localTime);
  int seconds = second(localTime);
  int minutes = minute(localTime);
  int hours = hour(localTime); //12 hour format use : hourFormat12(localTime)  isPM()/isAM()

  /////process time///////
  String h = "";
  String m = "";
  String s = "";
  if (hours < 10)
  {
    h = "0" + String(hours);
  }
  else
  {
    h = String(hours);
  }
  if (minutes < 10)
  {
    m = "0" + String(minutes);
  }
  else
  {
    m = String(minutes);
  }
  if (seconds < 10)
  {
    s = "0" + String(seconds);
  }
  else
  {
    s = String(seconds);
  }
  /////process date///////
  String mm = "";
  String dd = "";
  String w = dayStr(weekdays);

  if (months < 10)
  {
    mm = "0" + String(months);
  }
  else
  {
    mm = String(months);
  }

  if (days < 10)
  {
    dd = "0" + String(days);
  }
  else
  {
    dd = String(days);
  }
  String myTime = h + ":" + m + " " + s;
  String myDate = mm + "月" + dd + "日";
  String myWeek = w.substring(0, 3);
  Serial.println("-----------------");
  Serial.println("myTime: " + myTime);
  Serial.println("myDate: " + myDate);
  Serial.println("myWeek: " + myWeek);

  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.print(myTime);
  tft.setTextWrap(true);
  delay(1000);
}

// void tftPrintTest()
// {
//   tft.setTextWrap(false);
//   tft.fillScreen(ST77XX_BLACK);
//   tft.setCursor(0, 30);
//   tft.setTextColor(ST77XX_RED);
//   tft.setTextSize(1);
//   tft.println("Hello World!");
//   tft.setTextColor(ST77XX_YELLOW);
//   tft.setTextSize(2);
//   tft.println("Hello World!");
//   tft.setTextColor(ST77XX_GREEN);
//   tft.setTextSize(3);
//   tft.println("Hello World!");
//   tft.setTextColor(ST77XX_BLUE);
//   tft.setTextSize(4);
//   tft.print(1234.567);
//   tft.fillScreen(ST77XX_BLACK);
// }

void configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println("check wifi connect and reset");
}

void myMDNSinit()
{
  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "EPaperThing.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin("EspDisplay"))
  {
    Serial.println("Error setting up MDNS responder!");
    while (1)
    {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
}
void handleRoot()
{ //处理网站根目录“/”的访问请求
  String content = "<!DOCTYPE html><html lang='zh-CN'><head><meta charset='UTF-8'></head><body><center><form action='TODO 'method='POST'><br>提醒事项：<input type='text'name='todo'value=''><br><br><br><input type='submit'value='更新提醒事项'></form><form action='CLEAR 'method='POST'><br><br><input type='submit'value='清屏'></form></center></body></html>";
  esp8266_server.send(200, "text/html", content); // NodeMCU将调用此函数。访问成功返回200状态码，返回信息类型text/html
}

// 设置处理404情况的函数'handleNotFound'
void handleNotFound()
{                                                           // 当浏览器请求的网络资源无法在服务器找到时，返回404状态码，和提升信息类型和内容
  esp8266_server.send(404, "text/plain", "404: Not found"); // NodeMCU将调用此函数。
}
void webInit()
{
  //--------"启动网络服务功能"程序部分开始--------
  esp8266_server.begin();
  esp8266_server.on("/", HTTP_GET, handleRoot); //访问网站的根目录 处理GET请求 执行handleRoot函数
  esp8266_server.onNotFound(handleNotFound);    //当请求服务器找不到时，执行handleNotFound函数
  //--------"启动网络服务功能"程序部分结束--------
  Serial.println("HTTP esp8266_server started"); //  告知用户ESP8266网络服务功能已经启动
}
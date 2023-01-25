#include <TFT_eSPI.h>
#include <SPI.h>
#include "WiFi.h"
#include <Wire.h>


#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include <HTTPClient.h>

// You can customize the SSID name and change the password
const char* ssid = "BZ";
const char* password = "Blutzucker";

String JsonBuffer1;
String url = "http://192.168.4.2:17580/sgv.json?count=1"; //Blood glucose values from Xdrip

int BZ_X=5;
int BZ_Y=30;
int VB_Y=10;
int a;

// TFT Pins has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
// #define TFT_MOSI            19
// #define TFT_SCLK            18
// #define TFT_CS              5
// #define TFT_DC              16
// #define TFT_RST             23
// #define TFT_BL              4   // Display backlight control pin


#define ADC_EN              14  //ADC_EN is the ADC detection enable port
#define ADC_PIN             34
#define BUTTON_1            35
#define BUTTON_2            0

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library


RTC_DATA_ATTR long long last_date;
RTC_DATA_ATTR int Delta;
RTC_DATA_ATTR int BZ;


String httpGETRequest(const char* serverName) { // this is used to write the information from the webservices into a string. needed for weather and Nightscout
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  http.addHeader("api-secret", "c01cb099637039f535a420998f445aee58bef05e"); // SHA-1 hash of the Passwort set in Xdrip
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  // Debug
  if (httpResponseCode>0) {

    payload = http.getString();
  }
  // Free resources
  http.end();
  return payload;
}

void BG()
{
JsonBuffer1 = httpGETRequest(url.c_str());
/// String input;

StaticJsonDocument<512> doc;

DeserializationError error = deserializeJson(doc, JsonBuffer1);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

JsonObject root_0 = doc[0];
const char* root_0__id = root_0["_id"]; // "ecc22646-caec-4061-a7ab-7fad8d6bc8a8"
const char* root_0_device = root_0["device"]; // "Other App"
const char* root_0_dateString = root_0["dateString"]; // "2021-03-01T15:49:10.353+0100"
const char* root_0_sysTime = root_0["sysTime"]; // "2021-03-01T15:49:10.353+0100"
long long root_0_date = root_0["date"]; // 1614610150353
int root_0_sgv = root_0["sgv"]; // 102
float root_0_delta = root_0["delta"]; // -4.015
const char* root_0_direction = root_0["direction"]; // "Flat"
int root_0_noise = root_0["noise"]; // 1
long root_0_filtered = root_0["filtered"]; // 63000
long root_0_unfiltered = root_0["unfiltered"]; // 63000
int root_0_rssi = root_0["rssi"]; // 100
const char* root_0_type = root_0["type"]; // "sgv"
const char* root_0_units_hint = root_0["units_hint"]; // "mgdl"

Delta = round(root_0_delta);
BZ = round(root_0_sgv);
 
  if (root_0_date > last_date || a==3 || a==6 || a==9 || a==12 || a==15 || a==18 || a== 21  || a==24 || a==27 || a==30 || a>=33)
  {

     if (root_0_date > last_date)
    {
    a=0;
    tft.fillScreen(TFT_BLACK);
    }
    last_date = root_0_date;
    Serial.println(BZ);
    if (BZ <=70 || BZ >= 180)
    {
    tft.fillScreen(TFT_RED);
    }
            tft.setTextSize(8);
            tft.setCursor(BZ_X,BZ_Y); 
            if (BZ+(5*Delta) <= 70 && BZ >70 || BZ+(5*Delta) >= 180 && BZ <180)
            {
             tft.setTextColor(TFT_RED);
            }
            else
            {
             tft.setTextColor(TFT_WHITE); 
            }
            
            tft.print(BZ); 
            tft.setTextSize(5);
            if(Delta >=0)
            {
            tft.setCursor(BZ_X+130,BZ_Y+5);
            tft.print("+");
            tft.setCursor(BZ_X+166,BZ_Y+5);
            tft.print(Delta);
            }
            else
            {
            tft.setCursor(BZ_X+130,BZ_Y+5);
            tft.print(Delta);
            }
            if (a>=21)
            {
             tft.setTextColor(TFT_YELLOW);
            }
            tft.fillRect(BZ_X+75,BZ_Y+65,26,15,TFT_BLACK);
            tft.setTextSize(3);
            tft.setCursor(BZ_X+75,BZ_Y+65);    
            tft.print(a/3);
            tft.setCursor(BZ_X+102,BZ_Y+65);    
            tft.print("min");

  }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Start");

    /*
    ADC_EN is the ADC detection enable port
    If the USB port is used for power supply, it is turned on by default.
    If it is powered by battery, it needs to be set to high level
    */
    pinMode(ADC_EN, OUTPUT);
    digitalWrite(ADC_EN, HIGH);

    tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(3);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(0, 0);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);

    

    /*
    if (TFT_BL > 0) {                           // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
        pinMode(TFT_BL, OUTPUT);                // Set backlight pin to output mode
        digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    }
    */
WiFi.softAP(ssid, password);
tft.setTextSize(2);
tft.setTextColor(TFT_WHITE);
tft.drawString("an",10,10);
int i=1;
int y=15;
int x;
 while (WiFi.softAPgetStationNum() == 0)
    {
    x=25+3*i;
    if (x>=200)
    {
      i=1;
      y=y+3;
    }
    x=25+3*i;  
    //tft.setCursor(x,y);
    tft.setTextSize(1);
    tft.drawString(".",x,y);
    i++;
    delay(1000);
    }
tft.fillScreen(TFT_BLACK);
    
}

void loop()
{
delay(1000);
BG();
delay(19000);
a++;
  
}

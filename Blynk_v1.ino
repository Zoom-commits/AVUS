
// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "TMPLfivw00cL"
#define BLYNK_DEVICE_NAME "TestAVUS"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_WROVER_BOARD
//#define USE_TTGO_T7

#include "BlynkEdgent.h"

// Includes 
#include <NTPClient.h>
#include <ESP32Time.h>
//#include <SPI.h>
#include "Arduino.h"

// --- Audio I2S -------------------
#include "Audio.h"
#include "SPI.h"
#include "SD.h"
#include "FS.h"

//---- Digital I/O used uSD -----
#define SD_CS          15
#define SPI_MOSI      13
#define SPI_MISO      32
#define SPI_SCK       14

//---- Digital I/O used I2S -----
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26
Audio audio;
WiFiClient client;
// -------- IFTTT ------------------
#define key "dcd50PRWdVFAnMTFbBRKRU"; 
#define Event "ESP32_data";

String MakerIFTTT_Key ;
String MakerIFTTT_Event;
String temp = "";
String humi = "";

int  i = 0;
int delayms = 7000;

char *append_str(char *here, String s) {  int i=0; while (*here++ = s[i]){i++;};return here-1;}
char *append_ul(char *here, unsigned long u) { char buf[20]; return append_str(here, ultoa(u, buf, 10));}
char post_rqst[256];char *p;char *content_length_here;char *json_start;int compi;

// -------- RTC WIFI ----------------
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -18000;
const int   daylightOffset_sec = 0;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec);
ESP32Time rtc;

// -------- Notifications -----------
bool approve = true;
bool approve1 = true;

// ------- Control ------------------
char myChar, estado = 1, input;

// ------- Start set up -------------
void setup()
{
  // ------- SD set up --------------
  pinMode(SD_CS, OUTPUT);      digitalWrite(SD_CS, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    //SPI.setFrequency(1000000);
    Serial.begin(115200);
    SD.begin(SD_CS);
  // ------ I2S audio set up --------
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(20); // 0...21
    audio.connecttoFS(SD, "/Start.mp3");
    delay(100);
  // ------ WiFi set up ------------
    BlynkEdgent.begin();
  
  // ------ Intern RTC with NTP ----
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)){
      rtc.setTimeStruct(timeinfo); 
    }
}

void loop() {
  BlynkEdgent.run();
  
  audio.loop();

  // --------- Start NTP server with ESP's RTC ------
              //timeClient.update();
              //if(timeClient.getHours() == 14 && timeClient.getMinutes()== 15 && approve==true){
              if(rtc.getHour(true) == 14 && rtc.getMinute()== 15 && approve==true){
              approve=false;
//              audio.connecttoFS(SD,"/Music/005pastillas.mp3");
              audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.mp3");
              Serial.println(timeClient.getDay());
              Serial.println("Buscando dispositivo 1");
              audio.loop();
              } else if (timeClient.getHours() == 14 && timeClient.getMinutes() > 15){
                approve=true;
              }
              if(timeClient.getHours() == 14 && timeClient.getMinutes()==16 && approve1==true){
              approve1=false;
              audio.connecttoFS(SD,"/quenospaso.mp3");
              Serial.println("Buscando dispositivo 2");
               audio.loop();
              } else if (timeClient.getHours() == 14 && timeClient.getMinutes()>16){
                approve1=true;
              }
  // ----------- END NTP server ------------
}
BLYNK_WRITE(V0){
  int state=param.asInt();
  if(state==1){
    Serial.println("Prueba de activación");
    
      i = (random(0, 100));
        temp = i; 
        i = (random(90, 200));
        humi = i;
    audio.connecttoFS(SD,"/Device1.mp3");
    WiFi_SendData();

        Serial.println("data Send to google Sheet");
        Serial.print(temp);
        Serial.print("  ");
        Serial.println(humi);
        Serial.println("Event has been Triggered");
        audio.loop();
        //delay(1000);
      
  }
}
BLYNK_WRITE(V1){
  int state=param.asInt();
  if(state==1){
    Serial.println("Prueba de activación botón 2");
    
//      i = (random(0, 100));
        temp = "Receptor 2";
        i = (random(90, 200));
        humi = i;
        
        audio.connecttoFS(SD,"/Device2.mp3");
        WiFi_SendData();

        Serial.println("data Send to google Sheet");
        Serial.print(temp);
        Serial.print("  ");
        Serial.println(humi);
        Serial.println("Event has been Triggered");
        audio.loop();
        //delay(1000);
  }
}
void WiFi_SendData() {
  if (client.connect("maker.ifttt.com", 80)) {
    MakerIFTTT_Key = key;
    MakerIFTTT_Event = Event;
    p = post_rqst;
    p = append_str(p, "POST /trigger/");
    p = append_str(p, MakerIFTTT_Event);
    p = append_str(p, "/with/key/");
    p = append_str(p, MakerIFTTT_Key);
    p = append_str(p, " HTTP/1.1\r\n");
    p = append_str(p, "Host: maker.ifttt.com\r\n");
    p = append_str(p, "Content-Type: application/json\r\n");
    p = append_str(p, "Content-Length: ");
    content_length_here = p;
    p = append_str(p, "NN\r\n");
    p = append_str(p, "\r\n");
    json_start = p;
    p = append_str(p, "{\"value1\":\"");
    p = append_str(p, temp);
    p = append_str(p, "\",\"value2\":\"");
    p = append_str(p, humi);
    p = append_str(p, "\"}");

    compi = strlen(json_start);
    content_length_here[0] = '0' + (compi / 10);
    content_length_here[1] = '0' + (compi % 10);
    client.print(post_rqst);
  }
}

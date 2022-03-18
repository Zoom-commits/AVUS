
#include <BluetoothSerial.h>
#include <WiFi.h>
#include <NTPClient.h>
#include "DFRobotDFPlayerMini.h"

// --- Audio I2S -------------------
#include "Arduino.h"
#include "Audio.h"
#include "SD.h"
#include "FS.h"

// Digital I/O used
#define SD_CS          5
#define SPI_MOSI      23    // SD Card
#define SPI_MISO      19
#define SPI_SCK       18

#define I2S_DOUT      25
#define I2S_BCLK      27    // I2S
#define I2S_LRC       26

Audio audio;

DFRobotDFPlayerMini myDFPlayer;
HardwareSerial mySoftwareSerial(1);

int  i=0;
int delayms=7000;

// -------- IFTTT ------------------
#define key "dcd50PRWdVFAnMTFbBRKRU"; 
#define Event "ESP32_data";

String MakerIFTTT_Key ;
String MakerIFTTT_Event;
String temp = "";
String humi = "";

char *append_str(char *here, String s) {  int i=0; while (*here++ = s[i]){i++;};return here-1;}
char *append_ul(char *here, unsigned long u) { char buf[20]; return append_str(here, ultoa(u, buf, 10));}
char post_rqst[256];char *p;char *content_length_here;char *json_start;int compi;

// -------- RTC WIFI ----------------
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -18000;
const int   daylightOffset_sec = 0;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer,gmtOffset_sec);

// ------- Connectivity -------------
WiFiClient client;
BluetoothSerial SerialBT;
String PASSWORD;
String wifiString;

// -------- Notifications -----------
bool approve= true;
bool approve1= true;

// ------- Control ------------------
char myChar, estado=1, input;

// ------- Start set up -------------
void setup() {

  Serial.begin(115200);
  
 //--------------- BT ---------------------
  SerialBT.begin("ESP32"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
 
 //------------ DFPLAYER --------------------
  Start_DFPLAYER();

}

// --------- Start loop ------------------
void loop() {

    switch(estado){
      case 1:
//           
          while(SerialBT.available()){
             myChar=SerialBT.read(); 
             PASSWORD += (myChar);
//           Serial.println(PASSWORD);
             estado=2;
          }
       
         break; 

      case 2:
          Wifi_Scan();
          Wifi_connection(); 

          estado=3;
        break;

      case 3:
          //-------- Rx y Tx ----------
          estado=4;
        break;

      case 4:
        // --------- Start NTP server with ESP's RTC ------
        timeClient.update();
        if(timeClient.getHours() == 10 && timeClient.getMinutes()== 11 && approve==true){
        approve=false;
        Serial.println(timeClient.getDay());
        Serial.println(F("myDFPlayer.playFolder(1, 5), pastillas"));
         myDFPlayer.playFolder(1, 5); 
        } else if (timeClient.getHours() == 10 && timeClient.getMinutes() > 11){
          approve=true;
        }
        if(timeClient.getHours() == 10 && timeClient.getMinutes()==13 && approve1==true){
        approve1=false;
        
        Serial.println(F("myDFPlayer.playFolder(1, 1), song1"));
         myDFPlayer.playFolder(1, 1); 
        } else if (timeClient.getHours() == 10 && timeClient.getMinutes()>13){
          approve1=true;
        }

      // ----------- END NTP server ------------
      input=SerialBT.read();
      
      if (input=='a'){
         Serial.println(F("myDFPlayer.playFolder(1, 5), pastillas"));
         myDFPlayer.playFolder(1, 5);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
         //myDFPlayer.play(1);
         //delay(delayms);
      }
      if (input=='b'){
         Serial.println(F("myDFPlayer.playFolder(1, 1), song1"));
         myDFPlayer.playFolder(1, 1);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
        // delay(delayms);
      }
      if (input=='c'){
         Serial.println(F("myDFPlayer.playFolder(2, 3)"));
         myDFPlayer.playFolder(2, 3);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
         //delay(delayms);
      }
      if (input=='d'){
         Serial.println(F("myDFPlayer.playFolder(2, 4)"));
         myDFPlayer.playFolder(2, 4);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
         //delay(delayms);
      }
  
  delay(20);

 // ------- Send data by IFTTT server --------------------
    if(input=='e'){
      
        i = (random(0,100));
        temp = i;
        i = (random(90,200));
        humi = i;
          
          WiFi_SendData ();  
          
          Serial.println("data Send to google Sheet");
          Serial.print(temp);
          Serial.print("  ");
          Serial.println(humi);
          Serial.println("Event has been Triggered");
        delay(6000);
      }
      break;
    }
}


void WiFi_SendData (){
  if(client.connect("maker.ifttt.com",80)) {
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
        
        compi= strlen(json_start);
        content_length_here[0] = '0' + (compi/10);
        content_length_here[1] = '0' + (compi%10);
        client.print(post_rqst);
  }
}
void Wifi_Scan(){
  
    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < 4; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }

    SerialBT.write('Hola');
}
void Wifi_connection(){
  
    wifiString= WiFi.SSID(0);

    //-------- Conversión de String a Char* -----------------
    int wifiString_len = wifiString.length() + 1;
    char char_array_wifi[wifiString_len];
    wifiString.toCharArray(char_array_wifi, wifiString_len);
    char* WIFI_SSID = char_array_wifi;

    int pass_len = PASSWORD.length() + 1;
    char char_password[pass_len];
    PASSWORD.toCharArray(char_password, pass_len);
    char* WIFI_PASSWORD = char_password;
    
    //-------- Conexión a network ---------------------------
    Serial.print("Connecting to ");
    WiFi.mode(WIFI_STA);   //   create wifi station
    Serial.println(WIFI_SSID);
    Serial.print("with password: ");
    Serial.println(PASSWORD);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
       // Wait a bit before scanning again
    delay(5000);
}
void Start_DFPLAYER(){
  mySoftwareSerial.begin(9600, SERIAL_8N1, 16, 17);  // speed, type, RX, TX
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(15);  //Set volume value. From 0 to 30
}

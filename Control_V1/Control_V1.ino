#include <BluetoothSerial.h>

#include <WiFi.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "DFRobotDFPlayerMini.h"

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
HardwareSerial mySoftwareSerial(1);

#define WIFI_SSID "MIRQUE"
#define  WIFI_PASSWORD "Juan80434818*"
#define key "dcd50PRWdVFAnMTFbBRKRU";
#define Event "ESP32_data";

///rf
const uint64_t my_radio_pipe = 0xE8E8F0F0E1LL; //Remember that this code should be the same for the receiver
const int t1=2,t2=15;
RF24 radio(4, 5);

// The sizeof this struct should not exceed 32 bytes
struct Data_to_be_sent {
  byte ch1; 
};

Data_to_be_sent sent_data;


int  i=0;
char input;
int delayms=7000;
String MakerIFTTT_Key ;
String MakerIFTTT_Event;
char *append_str(char *here, String s) {  int i=0; while (*here++ = s[i]){i++;};return here-1;}
char *append_ul(char *here, unsigned long u) { char buf[20]; return append_str(here, ultoa(u, buf, 10));}
char post_rqst[256];char *p;char *content_length_here;char *json_start;int compi;


WiFiClient client;
BluetoothSerial SerialBT;


void setup() {
//------------ RF--------------------
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(my_radio_pipe);  
  sent_data.ch1 = 127;
  pinMode(t1, INPUT); 
  pinMode(t2, INPUT); 

  
  Serial.begin(9600);
 //------------ DFPLAYER --------------------

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

  myDFPlayer.volume(22);  //Set volume value. From 0 to 30
  //myDFPlayer.play(1);  //Play the first mp3

  
 //------------ WiFi ------------------------
  Serial.print("Connecting to ");
  WiFi.mode(WIFI_STA);   //   create wifi station
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

  }

  Serial.println("");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(2000);
 
 //--------------- BT ---------------------

 SerialBT.begin("ESP32"); //Bluetooth device name
 Serial.println("The device started, now you can pair it with bluetooth!");
 

}

String temp = "";
String humi = "";

void loop() {
// RF

Transmision();
  static unsigned long timer = millis();
  
  if (SerialBT.available()){
      input = SerialBT.read();
      
      if (input=='a'){
         Serial.println(F("myDFPlayer.playFolder(1, 5), pastillas"));
         myDFPlayer.playFolder(1, 5);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
         //myDFPlayer.play(1);
         delay(delayms);
      }
      if (input=='b'){
         Serial.println(F("myDFPlayer.playFolder(1, 1), song1"));
         myDFPlayer.playFolder(1, 1);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
         delay(delayms);
      }
      if (input=='c'){
         Serial.println(F("myDFPlayer.playFolder(2, 3)"));
         myDFPlayer.playFolder(2, 3);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
         delay(delayms);
      }
      if (input=='d'){
         Serial.println(F("myDFPlayer.playFolder(2, 4)"));
         myDFPlayer.playFolder(2, 4);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
         delay(delayms);
      }
      Serial.write(SerialBT.read());
    }
  delay(20);
 

    if(input=='e'){
      
        i = (random(0,100));
        temp = i;
        i = (random(90,200));
        humi = i;
          
          WiFi_connection ();  
          
          Serial.println("data Send to google Sheet");
          Serial.print(temp);
          Serial.print("  ");
          Serial.println(humi);
          Serial.println("Event has been Triggered");
        delay(6000);
      }
    }


void WiFi_connection (){
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

void Transmision(){
  
   if(digitalRead(t1)){
    sent_data.ch1 =250 ;
    }
  if(digitalRead(t2)){
    sent_data.ch1 =180 ;
    }
    Serial.println(sent_data.ch1);
  radio.write(&sent_data, sizeof(Data_to_be_sent));
  
  }

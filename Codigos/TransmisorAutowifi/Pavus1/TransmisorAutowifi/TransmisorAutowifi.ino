/*
  Avus Turkey with AutoConnect library
  Developed by Juan Mirque And Cristian Acevedo
*/

//************ LIBRERIAS DE ESP32 **************************************//
#include "Arduino.h"
//************ LIBRERIAS DE AUDIO MAX *********************************//
#include "Audio.h"
//************ DEFINICION DE PINES I2C  ******************************//
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26
Audio             audio;
/*
 * SET UP START
 */
#define ServerVersion "1.0"
String Device = "";
String humi = "";
String  webpage = "";
bool    SPIFFS_present = false;
int ch1value = 0;

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  //************* Configuración de AutoConnect WiFi *************************//
  //************* Servidor web de configuración *************************//
  // Establish a connection with an autoReconnect option.
  configSetUp();
  //************* NRF24 CONFIGURATION *************************//  
  nrfPines();
  sdPinConfiguracion();
  nrfConfiguracion();
  //************* SD CARD CONFIGURATION ***********************// 
  sdConfiguracion();
  //************* MAX I2C CONFIGURATION ***********************// 
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(19); // 0...21
      //audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.mp3");
 audio.connecttoFS(SD, "/Start.mp3");
  delay(100);
  //************* INTEN RTC CONFIGURATION ***********************// 
  rtcWifi();
  setBotones();
  //************* TIRA RGB CONFIGURATION ***********************// 
  setUpRGB();  
 //************* Leer recordatorios ***********************// 
  readFile(SD, "/LeerESP.csv");

  //************* Set-up SERVER SD ***********************// 
  ServerSDsetup();
} // setup end

unsigned long last_Time = 0;

void loop() {
  
  request(); // request Server SD
  
  ch1value = receive_the_data();
  portalcito();
  audio.loop();
  delay(10);
 if(ch1value==10){
   
    mostrarRGB(0,255,0,0);
    Serial.println("LLego"); 
    
    ch1value=33;
  }
  if(ch1value==20){
    Serial.println("up"); 
    mostrarRGB(0,0,0,255);
    ch1value=33;
  }
//************* NOTIFICACTIONS AND SLEEP MODE ***********************// 


Notificaciones();
Notificaciones1();
Notificaciones2();
Notificaciones3();
Notificaciones4();


activarBotones();
// 
  int state=Serial.read();
  //Serial.println(state);
//  delay(1000);
//  if(state==49){
//     llavesitas(94,"/Device1.mp3","hoy","Dispositivo 1");
//     state=4;
//  }
//  if(state==50){
//     llavesitas(150,"/Device2.mp3","mañana","Dispositivo 2");
//     printLocalTime();
//     state=4;
//  }  
}

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
String Device = "";
String humi = "";
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
  audio.setVolume(20); // 0...21
      //audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.mp3");
 audio.connecttoFS(SD, "/Start.mp3");
  delay(100);
  //************* INTEN RTC CONFIGURATION ***********************// 
  rtcWifi();
  setBotones();
  //************* TIRA RGB CONFIGURATION ***********************// 
  setUpRGB();  
} // setup end

unsigned long last_Time = 0;

void loop() {
  
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
// 
  int state=Serial.read();
  //Serial.println(state);
//  delay(1000);
  if(state==49){
   
        llavesitas(94,"Dispositivo 1","hoy");
      state=4;
  }
  if(state==50){
     llavesitas(150,"Dispositivo 2","mañana");
     printLocalTime();
     state=4;
  }
 activarBotones();
  
}

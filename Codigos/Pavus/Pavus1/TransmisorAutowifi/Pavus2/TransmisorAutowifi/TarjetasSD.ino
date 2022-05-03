#include "SD.h"
#include "FS.h"
#include <SPI.h>
//************ DEFINICION DE PINES SPI SD ****************************//
#define HSPI_MISO   32
#define HSPI_MOSI   13
#define HSPI_SCLK   14
#define HSPI_SS     15
File archivo;     // objeto archivo del tipo File


//String formattedDate="hola";
//String dayStamp="prueba";
//String timeStamp="despues de hoy";
String dataMessage;

void sdPinConfiguracion(){
  SPIMonda.begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI);
}

void sdConfiguracion(){
   pinMode(HSPI_SS, OUTPUT);      digitalWrite(HSPI_SS, HIGH);
    SD.begin(HSPI_SS);
    if (!SD.begin(HSPI_SS)) {     // inicializacion de tarjeta SD
      Serial.println("fallo en inicializacion !");// si falla se muestra texto correspondiente y
      return;         // se sale del setup() para finalizar el programa
    }else{
       Serial.println("Sirvio esta chimbada");
       //audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.aac");
    }
}

void logSDCard(String dispositivo, String Horita) {
  dataMessage =  String(dispositivo) + "," + String(Horita) + "\r\n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/data.csv", dataMessage.c_str());
}


// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

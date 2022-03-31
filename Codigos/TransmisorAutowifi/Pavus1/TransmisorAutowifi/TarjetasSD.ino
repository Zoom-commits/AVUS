#include "SD.h"
#include "FS.h"
#include <SPI.h>
#include <Separador.h>
Separador sep;

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
//Variables alarmas//
int hora1 = 25;
int minuto1 = 25;
String  nombreCancion1;

int hora2 = 25;
int minuto2 = 0;
String  nombreCancion2;

int hora3 = 25;
int minuto3 = 0;
String nombreCancion3;

int hora4 = 25;
int minuto4 = 0;
String nombreCancion4;

int hora5 = 25;
int minuto5 = 0;
String nombreCancion5;

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


void readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  String control;
  // Serial.print("Read from file: ");
  while (file.available()) {
    // Serial.write(file.read());
    control = control + char(file.read());
  }
  Serial.print(control);

  String L1 = sep.separa(control, '\n',0);
  String L2 = sep.separa(control, '\n', 1);
  String L3 = sep.separa(control, '\n', 2);
  String L4 = sep.separa(control, '\n', 3);
  String L5 = sep.separa(control, '\n', 4);
  String L6 = sep.separa(control, '\n', 5);

  String h1 = sep.separa(L2, ',', 0);
  String m1 = sep.separa(L2, ',', 1);
  String n1 = sep.separa(L2, ',', 2);

  String h2 = sep.separa(L3, ',', 0);
  String m2 = sep.separa(L3, ',', 1);
  String n2 = sep.separa(L3, ',', 2);

  String h3 = sep.separa(L4, ',', 0);
  String m3 = sep.separa(L4, ',', 1);
  String n3 = sep.separa(L4, ',', 2);

  String h4 = sep.separa(L5, ',', 0);
  String m4 = sep.separa(L5, ',', 1);
  String n4 = sep.separa(L5, ',', 2);

  String h5 = sep.separa(L6, ',', 0);
  String m5 = sep.separa(L6, ',', 1);
  String n5 = sep.separa(L6, ',', 2);

//  int conv=0;

  hora1 = h1.toInt();
  minuto1 = m1.toInt();
  n1="/"+n1;
  nombreCancion1 =  n1;
  
  hora2 = h2.toInt();
  minuto2 = m2.toInt();
  n2="/"+n2;
  nombreCancion2 = n2;
  
  hora3 = h3.toInt();
  minuto3 = m3.toInt();
  n3="/"+n3;
  nombreCancion3 =  n3;
  
  hora4 = h4.toInt();
  minuto4 = m4.toInt();
  n4="/"+n4;
  nombreCancion4 =  n4;
  
  hora5 = h5.toInt();
  minuto5 = m5.toInt();
  n5="/"+n5;
  nombreCancion5 =  n5;

  
  
//  Serial.println("Hora: " + hora1);
//  Serial.println("minuto: " + minuto1);
//  Serial.println("cancion: " + nombreCancion1);
//
//  Serial.println("Hora2: " + hora2);
//  Serial.println("Segundo2: " + minuto2);
//  Serial.println("cancion2: " + nombreCancion2);

  //    Serial.println("Hora3: "+h3);
  //    Serial.println("Segundo3: "+s3);
  //    Serial.println("cancion3: "+n3);

  file.close();
}

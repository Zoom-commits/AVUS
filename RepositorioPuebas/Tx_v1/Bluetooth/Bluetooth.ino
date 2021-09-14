#include "SD32.h"
#include "BluetoothSerial.h"
#include <SPI.h>    // incluye libreria interfaz SPI

#define SSpin 5    // Slave Select en pin digital 10
File archivo;      // objeto archivo del tipo File
String input;
//
BluetoothSerial SerialBT;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
   SerialBT.begin("ESP32"); //Bluetooth device name
   Serial.println("The device started, now you can pair it with bluetooth!");

  Start_SD();
  WriteuSD("Hola");
  ReaduSD();
  
}

void Start_SD(){  
  Serial.println("Inicializando tarjeta ..."); // texto en ventana de monitor
    if (!SD.begin(SSpin)) {     // inicializacion de tarjeta SD
      Serial.println("fallo en inicializacion !");// si falla se muestra texto correspondiente y
      return;         // se sale del setup() para finalizar el programa
    }
  
  Serial.println("inicializacion correcta");  // texto de inicializacion correcta
}
void WriteuSD(String Input_BT){
  archivo = SD.open("prueba.txt", FILE_WRITE);  // apertura para lectura/escritura de archivo prueba.txt
  
  if (archivo) {
    
    archivo.println(Input_BT);  // escritura de una linea de texto en archivo
    
    Serial.println("Escribiendo en archivo prueba.txt..."); // texto en monitor serie
    archivo.close();        // cierre del archivo
    Serial.println("escritura correcta"); // texto de escritura correcta en monitor serie
  } else {
    Serial.println("error en apertura de prueba.txt");  // texto de falla en apertura de archivo
  }
}
void ReaduSD(){
 archivo = SD.open("prueba.txt");    // apertura de archivo prueba.txt
  if (archivo) {
    Serial.println("Contenido de prueba.txt:"); // texto en monitor serie
    while (archivo.available()) {   // mientras exista contenido en el archivo
      Serial.write(archivo.read());     // lectura de a un caracter por vez
    }
    archivo.close();        // cierre de archivo
  } else {
    Serial.println("error en la apertura de prueba.txt");// texto de falla en apertura de archivo
  } 
}



void loop() {
  // put your main code here, to run repeatedly:
//if ( Serial.available())
//        SerialBT.write(Serial.read());



  if (SerialBT.available()){
  
    if(SerialBT.read()=='h'){
      
      input = "Cris";
      
      WriteuSD(input);
      ReaduSD();
     Serial.write(SerialBT.read());
    }
  delay(20);
  }
}

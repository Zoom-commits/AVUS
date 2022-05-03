/* Receiver code for the Arduino Radio control with PWM output
 * Install the NRF24 library to your IDE
 * Upload this code to the Arduino UNO, NANO, Pro mini (5V,16MHz)
 * Connect a NRF24 module to it:
 
    Module // Arduino UNO,NANO
    
    GND    ->   GND
    Vcc    ->   3.3V
    CE     ->   D9
    CSN    ->   D10
    CLK    ->   D13
    MOSI   ->   D11
    MISO   ->   D12

This code receive 1 channels and prints the value on the serial monitor
Please, like share and subscribe : https://www.youtube.com/c/ELECTRONOOBS
*/

#include "pitches.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Notas de la cancion:
int melody[] = {
    NOTE_A4, NOTE_D4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_D4, NOTE_F4, NOTE_G4, NOTE_E4,
    NOTE_G4, NOTE_C4, NOTE_F4, NOTE_E4, NOTE_G4, NOTE_C4, NOTE_F4, NOTE_E4, NOTE_D4
   
};

// duracion notas: 1= negra 2 = blanca...
int DuracionNotas[] = {
  2, 2, 4, 4, 2, 2, 4, 4, 1, 2, 2, 4, 4, 2, 2, 4, 4, 1,
};


const uint64_t pipeIn = 0xE8E8F0F0E1LL;     //Remember that this code is the same as in the transmitter
const int led1=5;
RF24 radio(9, 10);  //CSN and CE pins

// The sizeof this struct should not exceed 32 bytes
struct Received_data {
  byte ch1;
};

int ch1_value = 0;
Received_data received_data;


/**************************************************/

void setup()
{
  //Serial.begin(9600);
  //We reset the received values
  received_data.ch1 = 127;
 
  //Once again, begin and radio configuration
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);  
  radio.openReadingPipe(1,pipeIn);
  
  //We start the radio comunication
  radio.startListening();
 pinMode(led1, OUTPUT); 
}

/**************************************************/

unsigned long last_Time = 0;

//We create the function that will read the data each certain time
void receive_the_data()
{
  while ( radio.available() ) {
  radio.read(&received_data, sizeof(Received_data));
  last_Time = millis(); //Here we receive the data
}
}

/**************************************************/

void loop()
{
  //Receive the radio data
  receive_the_data();

  
  ch1_value = received_data.ch1;
 // Serial.println(ch1_value);
  if(ch1_value==180){
    digitalWrite(led1, HIGH);
    sound();
    }else{
      digitalWrite(led1, LOW);
    }
  
}//Loop end


void sound(){
  // Notas de la melodia, :
  for (int Nota = 0; Nota < 18; Nota++) {

    // calculo de la duracin de la nota, dividimos un segundo por el tipo de nota
    
    int Duracion = 1000 / DuracionNotas[Nota];
// pin usado numero 6
    tone(6, melody[Nota], Duracion);

    // para oir bien la melodia entre notas aadimos un retardo de un 40%
    
    int pausa = Duracion * 1.40;
       delay(pausa);
    
    // Paramos la melodia
    noTone(6);
  }
}

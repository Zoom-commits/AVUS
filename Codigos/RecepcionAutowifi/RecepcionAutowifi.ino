#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "pitches.h"

#define RX_ADDRESS "AVUS"
#define TX_ADDRESS "AVUZ"
byte address[6] = RX_ADDRESS;
byte addressb[6] = TX_ADDRESS;
RF24 radio(9, 10);  //CSN and CE pins

// The sizeof this struct should not exceed 32 bytes
struct Received_data {
  byte ch1;
};
int ch1_value = 0;
Received_data received_data;
//************* TRANSMISIÓN **********************************************//
struct Data_to_be_sent {
  byte ch2;
};
Data_to_be_sent sent_data;
int ReadVoltage ;
int up = 0;
int down = 0;
/**************************************************/
int melodyPin = 7;
const int PinSalir = LOW;

void setup()
{
  pinMode(PinSalir, INPUT);
  Serial.begin(9600);
  //We reset the received values
  received_data.ch1 = 124;
  //sent_data.ch2 = 223;
  //Once again, begin and radio configuration
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, address);
  radio.openWritingPipe(addressb);
  //We start the radio comunication
  radio.startListening();
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

int estado = 1;
void loop()
{
//  if (digitalRead(PinSalir) == LOW) {
//    Serial.println("LOW");
//  }

  switch (estado) {
    case 1:
      //Receive the radio data
      receive_the_data();
      ch1_value = received_data.ch1;
     // Serial.println(ch1_value);
      if (ch1_value == 94) {
         Serial.println("AAA");
        sing();
        received_data.ch1 = 33;
      }
      if (ch1_value == 150) {
         Serial.println("bbb");
        sound();
        received_data.ch1 = 33;
      }
      //estado = 2;
      break;
    case 2:
                ReadVoltage = analogRead(A0);
                if(ReadVoltage<=100&&down==0){
                  radio.stopListening();
                  sent_data.ch2 = 10;
                   for(int y=0;y<100;y++){
                      radio.write(&sent_data, sizeof(Data_to_be_sent));
                       Serial.println(sent_data.ch2);
                      delay(1);
                   }
                 Serial.println("down");
                 down=1;
                 up=0;
                 radio.startListening();
                }
                if(ReadVoltage>=900&&up==0){
                  sent_data.ch2 = 20;
                  radio.stopListening();
                   for(int y=0;y<100;y++){
                      radio.write(&sent_data, sizeof(Data_to_be_sent));
                       Serial.println(sent_data.ch2);
                      delay(1);
                   }
                 Serial.println("up");
                 down=0;
                 up=1;
                 radio.startListening();
                }

      estado = 1;
      break;
  }
}//Loop end





void buzz(int targetPin, long frequency, long length) {
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }
}
int melodyA [] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDuration[] = {
  250, 125, 250, 125, 250, 125, 250, 125
};

int melody[] = {
  NOTE_A4, NOTE_D4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_D4, NOTE_F4, NOTE_G4, NOTE_E4,
  NOTE_G4, NOTE_C4, NOTE_F4, NOTE_E4, NOTE_G4, NOTE_C4, NOTE_F4, NOTE_E4, NOTE_D4

};

int DuracionNotas[] = {
  2, 2, 4, 4, 2, 2, 4, 4, 1, 2, 2, 4, 4, 2, 2, 4, 4, 1,
};
void sing() {
  int size = sizeof(melodyA) / sizeof(int);
  for (int thisNote = 0; thisNote < size; thisNote++) {
    if (digitalRead(PinSalir) == LOW) {
      Serial.println("LOW");
      break;
    }
    buzz(melodyPin, melodyA[thisNote], noteDuration[thisNote] * 2);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:

    int pauseBetweenNotes = noteDuration[thisNote] * 1.30;
    delay(pauseBetweenNotes);

    // stop the tone playing:
    buzz(melodyPin, 0, noteDuration[thisNote]);

  }
}


void sound() {
  // Notas de la melodia, :
  for (int Nota = 0; Nota < 18; Nota++) {
    //   if(digitalRead(PinSalir)==LOW){
    //  Nota=19;
    //  }

    if (digitalRead(PinSalir) == LOW) {
      Serial.println("LOW");
      break;
    }
    // calculo de la duracin de la nota, dividimos un segundo por el tipo de nota

    int Duracion = 2000 / DuracionNotas[Nota];
    // pin usado numero 3
    tone(melodyPin, melody[Nota], Duracion);

    // para oir bien la melodia entre notas aadimos un retardo de un 40%

    int pausa = Duracion * 1.40;
    delay(pausa);

    // Paramos la melodia
    noTone(melodyPin);
  }
}

//void salir(){
//  if(digitalRead(PinSalir)==LOW){
//
//  }
//}
void velocidadMenos()
{

}

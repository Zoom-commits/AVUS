#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "pitches.h"

#define RX_ADDRESS "AVUS"
#define TX_ADDRESS "AVUZ"
#define REST      0

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


int tempo = 120;
// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;

void setup()
{
  pinMode(PinSalir, INPUT);
  //Serial.begin(9600);
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
         //Serial.println("AAA");
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
                      // Serial.println(sent_data.ch2);
                      delay(1);
                   }
                 //Serial.println("down");
                 down=1;
                 up=0;
                 radio.startListening();
                }
                if(ReadVoltage>=900&&up==0){
                  sent_data.ch2 = 20;
                  radio.stopListening();
                   for(int y=0;y<100;y++){
                      radio.write(&sent_data, sizeof(Data_to_be_sent));
                       //Serial.println(sent_data.ch2);
                      delay(1);
                   }
                 //Serial.println("up");
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


int melody[] = {
  REST,2, REST,4, REST,8, NOTE_DS4,8, 
  NOTE_E4,-4, REST,8, NOTE_FS4,8, NOTE_G4,-4, REST,8, NOTE_DS4,8,
  NOTE_E4,-8, NOTE_FS4,8,  NOTE_G4,-8, NOTE_C5,8, NOTE_B4,-8, NOTE_E4,8, NOTE_G4,-8, NOTE_B4,8,   
  NOTE_AS4,2, NOTE_A4,-16, NOTE_G4,-16, NOTE_E4,-16, NOTE_D4,-16, 
  NOTE_E4,2, REST,4, REST,8, NOTE_DS4,4,

  NOTE_E4,-4, REST,8, NOTE_FS4,8, NOTE_G4,-4, REST,8, NOTE_DS4,8,
  NOTE_E4,-8, NOTE_FS4,8,  NOTE_G4,-8, NOTE_C5,8, NOTE_B4,-8, NOTE_G4,8, NOTE_B4,-8, NOTE_E5,8,
  NOTE_DS5,1,   
  NOTE_D5,2, REST,4, REST,8, NOTE_DS4,8, 
  NOTE_E4,-4, REST,8, NOTE_FS4,8, NOTE_G4,-4, REST,8, NOTE_DS4,8,
  NOTE_E4,-8, NOTE_FS4,8,  NOTE_G4,-8, NOTE_C5,8, NOTE_B4,-8, NOTE_E4,8, NOTE_G4,-8, NOTE_B4,8,   
  
  NOTE_AS4,2, NOTE_A4,-16, NOTE_G4,-16, NOTE_E4,-16, NOTE_D4,-16, 
  NOTE_E4,-4, REST,4,
  REST,4, NOTE_E5,-8, NOTE_D5,8, NOTE_B4,-8, NOTE_A4,8, NOTE_G4,-8, NOTE_E4,-8,
  NOTE_AS4,16, NOTE_A4,-8, NOTE_AS4,16, NOTE_A4,-8, NOTE_AS4,16, NOTE_A4,-8, NOTE_AS4,16, NOTE_A4,-8,   
  NOTE_G4,-16, NOTE_E4,-16, NOTE_D4,-16, NOTE_E4,16, NOTE_E4,16, NOTE_E4,2,
};
int notes = sizeof(melody) / sizeof(melody[0]) / 2;
void sound() {
  // Notas de la melodia, :
 for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(melodyPin, melody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
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

/* 
  Take on me
  Connect a piezo buzzer or speaker to pin 11 or select a new pin.
  More songs available at https://github.com/robsoncouto/arduino-songs                                            
                                              
                                              Robson Couto, 2019
*/

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
int melodyPin = 3;
const int PinSalir = LOW;

// change this to make the song slower or faster
int tempo = 140;

// change this to whichever pin you want to use
int buzzer = 3;

// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int melody[] = {

  // Take on me, by A-ha
  // Score available at https://musescore.com/user/27103612/scores/4834399
  // Arranged by Edward Truong

  NOTE_FS5,8, NOTE_FS5,8,NOTE_D5,8, NOTE_B4,8, REST,8, NOTE_B4,8, REST,8, NOTE_E5,8, 
  REST,8, NOTE_E5,8, REST,8, NOTE_E5,8, NOTE_GS5,8, NOTE_GS5,8, NOTE_A5,8, NOTE_B5,8,
  NOTE_A5,8, NOTE_A5,8, NOTE_A5,8, NOTE_E5,8, REST,8, NOTE_D5,8, REST,8, NOTE_FS5,8, 
  REST,8, NOTE_FS5,8, REST,8, NOTE_FS5,8, NOTE_E5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,8,
  NOTE_FS5,8, NOTE_FS5,8,NOTE_D5,8, NOTE_B4,8, REST,8, NOTE_B4,8, REST,8, NOTE_E5,8, 
  
  REST,8, NOTE_E5,8, REST,8, NOTE_E5,8, NOTE_GS5,8, NOTE_GS5,8, NOTE_A5,8, NOTE_B5,8,
  NOTE_A5,8, NOTE_A5,8, NOTE_A5,8, NOTE_E5,8, REST,8, NOTE_D5,8, REST,8, NOTE_FS5,8, 
  REST,8, NOTE_FS5,8, REST,8, NOTE_FS5,8, NOTE_E5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,8,
  NOTE_FS5,8, NOTE_FS5,8,NOTE_D5,8, NOTE_B4,8, REST,8, NOTE_B4,8, REST,8, NOTE_E5,8, 
  REST,8, NOTE_E5,8, REST,8, NOTE_E5,8, NOTE_GS5,8, NOTE_GS5,8, NOTE_A5,8, NOTE_B5,8,
  
  NOTE_A5,8, NOTE_A5,8, NOTE_A5,8, NOTE_E5,8, REST,8, NOTE_D5,8, REST,8, NOTE_FS5,8, 
  REST,8, NOTE_FS5,8, REST,8, NOTE_FS5,8, NOTE_E5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,8,
  
};

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

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
//if (Serial.available() > 0) {
//ch1_value = Serial.read();
// Serial.println(ch1_value);
//}

  switch (estado) {
    case 1:
      //Receive the radio data
      receive_the_data();
      ch1_value = received_data.ch1;
//      Serial.println(ch1_value);
     
      //if (ch1_value == 55) {
        //Serial.println("bbb");
        sing();
        //received_data.ch1 = 33;
      //}
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

void sing() {
  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
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
    tone(buzzer, melody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(buzzer);
  }
}

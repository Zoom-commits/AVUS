/* 
  Pink Panther theme
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
//#define NOTE_B0  31
//#define NOTE_C1  33
//#define NOTE_CS1 35
//#define NOTE_D1  37
//#define NOTE_DS1 39
//#define NOTE_E1  41
//#define NOTE_F1  44
//#define NOTE_FS1 46
//#define NOTE_G1  49
//#define NOTE_GS1 52
//#define NOTE_A1  55
//#define NOTE_AS1 58
//#define NOTE_B1  62
//#define NOTE_C2  65
//#define NOTE_CS2 69
//#define NOTE_D2  73
//#define NOTE_DS2 78
//#define NOTE_E2  82
//#define NOTE_F2  87
//#define NOTE_FS2 93
//#define NOTE_G2  98
//#define NOTE_GS2 104
//#define NOTE_A2  110
//#define NOTE_AS2 117
//#define NOTE_B2  123
//#define NOTE_C3  131
//#define NOTE_CS3 139
//#define NOTE_D3  147
//#define NOTE_DS3 156
//#define NOTE_E3  165
//#define NOTE_F3  175
//#define NOTE_FS3 185
//#define NOTE_G3  196
//#define NOTE_GS3 208
//#define NOTE_A3  220
//#define NOTE_AS3 233
//#define NOTE_B3  247
//#define NOTE_C4  262
//#define NOTE_CS4 277
//#define NOTE_D4  294
//#define NOTE_DS4 311
//#define NOTE_E4  330
//#define NOTE_F4  349
//#define NOTE_FS4 370
//#define NOTE_G4  392
//#define NOTE_GS4 415
//#define NOTE_A4  440
//#define NOTE_AS4 466
//#define NOTE_B4  494
//#define NOTE_C5  523
//#define NOTE_CS5 554
//#define NOTE_D5  587
//#define NOTE_DS5 622
//#define NOTE_E5  659
//#define NOTE_F5  698
//#define NOTE_FS5 740
//#define NOTE_G5  784
//#define NOTE_GS5 831
//#define NOTE_A5  880
//#define NOTE_AS5 932
//#define NOTE_B5  988
//#define NOTE_C6  1047
//#define NOTE_CS6 1109
//#define NOTE_D6  1175
//#define NOTE_DS6 1245
//#define NOTE_E6  1319
//#define NOTE_F6  1397
//#define NOTE_FS6 1480
//#define NOTE_G6  1568
//#define NOTE_GS6 1661
//#define NOTE_A6  1760
//#define NOTE_AS6 1865
//#define NOTE_B6  1976
//#define NOTE_C7  2093
//#define NOTE_CS7 2217
//#define NOTE_D7  2349
//#define NOTE_DS7 2489
//#define NOTE_E7  2637
//#define NOTE_F7  2794
//#define NOTE_FS7 2960
//#define NOTE_G7  3136
//#define NOTE_GS7 3322
//#define NOTE_A7  3520
//#define NOTE_AS7 3729
//#define NOTE_B7  3951
//#define NOTE_C8  4186
//#define NOTE_CS8 4435
//#define NOTE_D8  4699
//#define NOTE_DS8 4978
//#define REST      0


// change this to make the song slower or faster
int tempo = 120;

// change this to whichever pin you want to use
int buzzer = 3;

// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int melody[] = {

  // Pink Panther theme
  // Score available at https://musescore.com/benedictsong/the-pink-panther
  // Theme by Masato Nakamura, arranged by Teddy Mason

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

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;

void setup()
{
  pinMode(PinSalir, INPUT);
//  Serial.begin(9600);
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
     
      if (ch1_value == 22) {
//         Serial.println("bbb");
        sing();
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

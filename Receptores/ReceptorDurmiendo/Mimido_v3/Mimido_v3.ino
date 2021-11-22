#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "pitches.h"



#include <avr/sleep.h>
#include "LowPower.h"  //8 sec timer to use
#include <avr/power.h>

#define RX_ADDRESS "AVUS"



RF24 radio(9,10);

const byte rxAddr[6] = RX_ADDRESS;

int i=0;
int j=0;

struct Received_data {
  byte ch1;
  
};

int ch1_value = 0;
Received_data received_data;

int melodyPin = 7;

void setup(){
// slow clock down to 4 MHz so that it can work at 1.8 volt
//clock_prescale_set (clock_div_2);
Serial.begin(9600);

//  while (!Serial);
 received_data.ch1 = 127;
 
  //Once again, begin and radio configuration
  radio.begin();
  radio.setAutoAck(false);
  //radio.setRetries(0,15)
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1,rxAddr);
  
  //We start the radio comunication
  radio.startListening();

 Serial.println("Recibiendo");

 

}

char text[2] = {0};
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
void loop(){
    //sound();
    receive_the_data();

  
  ch1_value = received_data.ch1;
  Serial.println(ch1_value);
 //radio.powerUp();
//        for(int i=0;i<1000;i++){
//          delay(1);
//        
//    if (radio.available()) {
//                        receive_the_data();
//                
//                  
//             ch1_value = received_data.ch1;
//                    //  radio.printDetails();
//               Serial.println(ch1_value);
//                //        radio.read(&text, sizeof(text));      
//                
//                    i=i+1;
//                
//                    if (i>59){i=0;j=j+1;}
//                
//                   
//                    if(ch1_value==94){
//                      sound();
//                       radio.powerDown ();
//                    //for(int ti=0;ti<4;ti++) {  //upto 5*8 this works OK
////                     Serial.println("a mimir");
//                    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF); 
//                      Serial.println("recibido se mimio");
//                    //}
//                    }
//                    radio.powerUp();
//                    radio.startListening(); 
//                    ch1_value=0;                   
//      }}
//      if(!radio.available()){
//         radio.powerDown ();
//         Serial.println("en 4s mimir");
//         LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
//          radio.powerUp(); 
//          radio.startListening();  
//     }
        
}


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

  buzz(melodyPin, melodyA[thisNote], noteDuration[thisNote]);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:

    int pauseBetweenNotes = noteDuration[thisNote] * 1.30;
    delay(pauseBetweenNotes);

    // stop the tone playing:
    buzz(melodyPin, 0, noteDuration[thisNote]);

  }
}


void sound(){
  // Notas de la melodia, :
  for (int Nota = 0; Nota < 18; Nota++) {

    // calculo de la duracin de la nota, dividimos un segundo por el tipo de nota
    
    int Duracion = 1000 / DuracionNotas[Nota];
// pin usado numero 3
    tone(3, melody[Nota], Duracion);

    // para oir bien la melodia entre notas aadimos un retardo de un 40%
    
    int pausa = Duracion * 1.40;
       delay(pausa);
    
    // Paramos la melodia
    noTone(6);
  }
}

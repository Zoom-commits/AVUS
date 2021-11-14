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


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
//#include "RF24Network.h"
//#include "RF24Mesh.h"
#define RX_ADDRESS "AVUS"
const byte rxAddr[6] = RX_ADDRESS;
const uint64_t pipeIn = 0xE8E8F0F0E1LL;     //Remember that this code is the same as in the transmitter
const int led1=7;
 //RF24 radio(3, 3);  //CSN and CE pins//TINNY
RF24 radio(9, 10);  //CSN and CE pins/NANO
//RF24Network network(radio);
//RF24Mesh mesh(radio, network);
// The sizeof this struct should not exceed 32 bytes
struct Received_data {
  byte ch1;
  
};

int ch1_value = 0;
Received_data received_data;

#define RX_ADDRESS "AVUS"
byte address[6] = RX_ADDRESS;
/**************************************************/

void setup()
{
  //We reset the received values
  received_data.ch1 = 127;
 Serial.begin(115200);
  //Once again, begin and radio configuration
  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(0,15); 
  radio.openReadingPipe(1,address);
  
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
  Serial.println(ch1_value);
  if(ch1_value==250){
    digitalWrite(led1, HIGH);
    }else{
      digitalWrite(led1, LOW);
    }
  
}//Loop end

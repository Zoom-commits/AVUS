//************ LIBRERIAS DE NRF24L01  *********************************//
#include <nRF24L01.h>
#include <RF24.h>

//************ DEFINICION DE PINES SPI NRF ****************************//
#define RX_ADDRESS "AVUS"
#define TX_ADDRESS "AVUZ"
#define VSPI_MISO   19
#define VSPI_MOSI   23
#define VSPI_SCLK   18
#define VSPI_SS     5

//************* TRANSMISIÓN **********************************************//
byte address[6] = RX_ADDRESS;
byte addressb[6] = TX_ADDRESS;

RF24 radio(4,5);
// The sizeof this struct should not exceed 32 bytes
struct Data_to_be_sent {
  byte ch1; 
};
Data_to_be_sent sent_data;
//************* Recepcion**********************************************//
struct Received_data {
  byte ch2;
};

int ch1_value = 0;

Received_data received_data;


void nrfPines(){
SPI.begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI);
}
int receive_the_data(){
  while ( radio.available() ) {
  radio.read(&received_data, sizeof(Received_data));
  last_Time = millis(); //Here we receive the data
  ch1_value = received_data.ch2;
  return ch1_value;
  }
}
void nrfConfiguracion(){
//************* NRF24 CONFIGURATION *************************// 
    
    radio.begin();
    radio.setAutoAck(false);
    radio.setDataRate(RF24_250KBPS);
    radio.openWritingPipe(address);  
    radio.openReadingPipe(1,addressb);
    radio.startListening();
    sent_data.ch1 = 123;
    received_data.ch2 = 127;
}

void llavesitas(int val,const char * dispo, String horita, String Devices){
  //const char * sonidotx= dispo;
  
   radio.stopListening();
   Serial.println("funcion 2 OK");
   sent_data.ch1 = val;

   for(int y=0;y<100;y++){
      radio.write(&sent_data, sizeof(Data_to_be_sent));
      delay(1);
      Serial.println(sent_data.ch1);
   }
   
   Device = Devices; 
   int i = (random(90, 200));
   humi = i;
   audio.connecttoFS(SD,dispo);         
   WiFi_SendData(); // envío de dato

   Serial.println("data Send to google Sheet");
   Serial.print("...");
   Serial.println("The data was storaged in Sheets");

//************* SD CARD STORAGE ***********************// 
 logSDCard(dispo,horita);
 radio.startListening();
}

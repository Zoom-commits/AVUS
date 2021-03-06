/*
  Avus v1 with AutoConnect library
  Developed by Juan Mirque And Cristian Acevedo
*/
//************ LIBRERIAS DE WIFI ***************************************//
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
#include <AutoConnect.h>
//************ LIBRERIAS DE ESP32 **************************************//
#include <NTPClient.h>
#include <ESP32Time.h>
#include <SPI.h>
#include "Arduino.h"
//************ LIBRERIAS DE AUDIO MAX *********************************//
#include "Audio.h"
#include "SD.h"
#include "FS.h"

//************ LIBRERIAS DE NRF24L01  *********************************//
#include <nRF24L01.h>
#include <RF24.h>

//************ DEFINICION DE PINES SPI NRF ****************************//
#define RX_ADDRESS "AVUS"
#define VSPI_MISO   19
#define VSPI_MOSI   23
#define VSPI_SCLK   18
#define VSPI_SS     5

//************ DEFINICION DE PINES SPI SD ****************************//
#define HSPI_MISO   32
#define HSPI_MOSI   13
#define HSPI_SCLK   14
#define HSPI_SS     15
File archivo;     // objeto archivo del tipo File

//************ DEFINICION DE PINES I2C  ******************************//
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

//************ DEFINICION DE TIEMPO SLEEP MODE **********************//
#define uS_TO_S_FACTOR 1000000 //Conversion factor for micro to seconds
#define TIME_TO_SLEEP 60       //Time EPS32 will go to sleep (in seconds)

//************ DEFINICION DE SERVIDOR IFTTT ************************//
#define key "dcd50PRWdVFAnMTFbBRKRU"; 
#define Event "ESP32_data";

WebServer         Server;
AutoConnect       Portal(Server);
AutoConnectConfig Config;       // Enable autoReconnect supported on v0.9.4
Audio             audio;
WiFiClient        client;

//************* Pagina HTML para configurar el WiFi **********************//
void rootPage() {
  String  content =
    "<html>"
    "<head>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "<script type=\"text/javascript\">"
    "setTimeout(\"location.reload()\", 1000);"
    "</script>"
    "</head>"
    "<body>"
    "<h2 align=\"center\" style=\"color:blue;margin:20px;\">Bienvenido a AVUS!</h2>"
    "<p style=\"text-align:center;\">Por favor presione el boton de abajo para configurar el WiFi del dispositivo.</p>"
    "<p></p><p style=\"padding-top:20px;text-align:center\">" AUTOCONNECT_LINK(COG_24) "</p>"
    "</body>"
    "</html>";

  Server.send(200, "text/html", content);
}
void startPage() {
  // The /start page just constitutes timezone,
  // it redirects to the root page without the content response.
  Server.sendHeader("Location", String("http://") + Server.client().localIP().toString() + String("/"));
  Server.send(302, "text/plain", "");
  Server.client().flush();
  Server.client().stop();
}

//************* IFTTT ***************************************************//
String MakerIFTTT_Key ;
String MakerIFTTT_Event;
String Device = "";
String humi = "";

int  i = 0;
int delayms = 7000;

char *append_str(char *here, String s) {  int i=0; while (*here++ = s[i]){i++;};return here-1;}
char *append_ul(char *here, unsigned long u) { char buf[20]; return append_str(here, ultoa(u, buf, 10));}
char post_rqst[256];char *p;char *content_length_here;char *json_start;int compi;

//************* RTC WIFI *************************************************//
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -18000;
const int   daylightOffset_sec = 0;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec);
ESP32Time rtc;

//************* TRANSMISI??N **********************************************//
byte address[6] = RX_ADDRESS;
RF24 radio(4,5);
// The sizeof this struct should not exceed 32 bytes
struct Data_to_be_sent {
  byte ch1; 
};
Data_to_be_sent sent_data;

//************* NOTIFICACIONES ******************************************//
bool approve = true;
bool approve1 = true;
bool approve2 = true;

//************* Libreria y definici??n LEDs RGB *************************//
#include  <Adafruit_NeoPixel.h> 
#define   PIN 17          //Pin GPIO
#define   NUMPIXELS  8    //# de pixeles en la cinta led RGB
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const int llaves = 22;
/*
 * SET UP START
 */

 float Temperature=3.14;
float Humidity=4.85;
String formattedDate="hola";
String dayStamp="prueba";
String timeStamp="despues de hoy";
String dataMessage;
void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  
//************* Configuraci??n de AutoConnect WiFi *************************//
    Config.homeUri="/_ac";
    Config.apid = "AVUS";
    Config.psk  = "tesisAvus";
    Config.apip=IPAddress(10,1,1,1);
    //Config.immediateStart = true;
    Config.hostName = "Avus_Wifi";
    Config.title = "AVUS";
    Config.autoSave = AC_SAVECREDENTIAL_AUTO;
    Config.autoReset = false;
    Config.autoReconnect = true;  
    Config.reconnectInterval = 6; 
    Config.autoRise = true;
    Config.retainPortal = true; 
  Portal.config(Config);
  
//************* Servidor web de configuraci??n *************************//
  Server.on("/", rootPage);
  Server.on("/start", startPage);   // Set NTP server trigger handler

// Establish a connection with an autoReconnect option.
  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
    Serial.println(WiFi.getHostname());
  }
//************* NRF24 CONFIGURATION *************************// 
    SPI.begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI);
    
    SPIMonda.begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI);
    radio.begin();
    radio.setAutoAck(false);
    radio.setDataRate(RF24_250KBPS);
    radio.openWritingPipe(address);  
    sent_data.ch1 = 123;

//************* SD CARD CONFIGURATION ***********************// 
    pinMode(HSPI_SS, OUTPUT);      digitalWrite(HSPI_SS, HIGH);
    SD.begin(HSPI_SS);
    if (!SD.begin(HSPI_SS)) {     // inicializacion de tarjeta SD
      Serial.println("fallo en inicializacion !");// si falla se muestra texto correspondiente y
      return;         // se sale del setup() para finalizar el programa
    }else{
       Serial.println("Sirvio esta chimbada");
       //audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.aac");
    }

//************* MAX I2C CONFIGURATION ***********************// 
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(15); // 0...21
    audio.connecttoFS(SD, "/Start.mp3");
    delay(100);
    
//************* INTEN RTC CONFIGURATION ***********************// 
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)){
      rtc.setTimeStruct(timeinfo); 
    }
    
    pinMode(llaves, INPUT);  

//************* TIRA RGB CONFIGURATION ***********************// 
    pixels.begin(); 

} // setup end
/*
 * void Loop start
 */
void loop() {
  Portal.handleClient();
  audio.loop();
  delay(10);
  
//************* NOTIFICACTIONS AND SLEEP MODE ***********************// 
 if(rtc.getHour(true) == 19 && rtc.getMinute()== 9 && approve==true){
    approve=false;
// audio.connecttoFS(SD,"/Music/005pastillas.mp3");
    audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.mp3");
    Serial.println(timeClient.getDay());
    Serial.println("Recordatorio uno");
    audio.loop();
 } else if (rtc.getHour(true) == 19 && rtc.getMinute() > 9){
    approve=true;
 }
 
 if(rtc.getHour(true) == 19 && rtc.getMinute()==12 && approve1==true){
    approve1=false;
    audio.connecttoFS(SD,"/quenospaso.mp3");
    Serial.println("Recordatorio 2");
    audio.loop();
 } else if (rtc.getHour(true) == 19 && rtc.getMinute()>12){
    approve1=true;
 }
 
 if(rtc.getHour(true) == 19 && rtc.getMinute()==10 && approve2==true){
    approve2=false;
    Serial.println("A mimir");
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
 } else if (rtc.getHour(true) == 19 && rtc.getMinute()>10){
    approve2=true;
 }
 
  int state=Serial.read();
  //Serial.println(state);
//  delay(1000);
  if(state==49){
    Serial.println("Prueba de activaci??n");
    
      i = (random(0, 100));
        Device = i; 
        i = (random(90, 200));
        humi = i;
    //audio.connecttoFS(SD,"/Device1.mp3");
    WiFi_SendData();

        Serial.println("data Send to google Sheet");
        Serial.print(Device);
        Serial.print("  ");
        Serial.println(humi);
        Serial.println("Event has been Triggered");
        //audio.loop();
        //delay(1000);
        llavesitas();
      state=4;
  }
  if(state==50){
     llavesitas2();
  }
}
void llavesitas(){
  //audio.connecttoFS(SD,"/quenospaso.mp3");
   Serial.println("funcion 1 OK");
   sent_data.ch1 = 94;
   
   for(int y=0;y<100;y++){
      radio.write(&sent_data, sizeof(Data_to_be_sent));
      delay(1);
      Serial.println(sent_data.ch1);
   }
   Device = "Device 1"; 
   i = (random(90, 200));
   humi = i;
   audio.connecttoFS(SD,"/Device1.mp3");         
   WiFi_SendData(); // env??o de dato

   Serial.println("data Send to google Sheet");
   Serial.print("...");
   Serial.println("The data was storaged in Sheets");

//************* SD CARD STORAGE ***********************// 
   archivo = SD.open("datalog.csv", FILE_WRITE);  

   if (archivo) {
    archivo.println("");
      archivo.print("\n Juanito tqm");  // escritura de una linea de texto en archivo
      archivo.print(" ,"); 
      archivo.println("x5 tqm \n "); 
      archivo.close();        // cierre del archivo
      Serial.println("escritura correcta"); // texto de escritura correcta en monitor serie
  } else {
      Serial.println("error en apertura de prueba.txt");  // texto de falla en apertura de archivo
  }

  archivo = SD.open("/datalog.csv");    // apertura de archivo prueba.txt
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
void llavesitas2(){
  //audio.connecttoFS(SD,"/quenospaso.mp3");
   Serial.println("funcion 2 OK");
   sent_data.ch1 = 150;
   
   for(int y=0;y<100;y++){
      radio.write(&sent_data, sizeof(Data_to_be_sent));
      delay(1);
      Serial.println(sent_data.ch1);
   }
   Device = "Device 2"; 
   i = (random(90, 200));
   humi = i;
   audio.connecttoFS(SD,"/Device2.mp3");         
   WiFi_SendData(); // env??o de dato

   Serial.println("data Send to google Sheet");
   Serial.print("...");
   Serial.println("The data was storaged in Sheets");

//************* SD CARD STORAGE ***********************// 
logSDCard();

}

//************* IFTTT data sending function ***********************// 
void WiFi_SendData() {
  if (client.connect("maker.ifttt.com", 80)) {
    MakerIFTTT_Key = key;
    MakerIFTTT_Event = Event;
    p = post_rqst;
    p = append_str(p, "POST /trigger/");
    p = append_str(p, MakerIFTTT_Event);
    p = append_str(p, "/with/key/");
    p = append_str(p, MakerIFTTT_Key);
    p = append_str(p, " HTTP/1.1\r\n");
    p = append_str(p, "Host: maker.ifttt.com\r\n");
    p = append_str(p, "Content-Type: application/json\r\n");
    p = append_str(p, "Content-Length: ");
    content_length_here = p;
    p = append_str(p, "NN\r\n");
    p = append_str(p, "\r\n");
    json_start = p;
    p = append_str(p, "{\"value1\":\"");
    p = append_str(p, Device);
    p = append_str(p, "\",\"value2\":\"");
    p = append_str(p, humi);
    p = append_str(p, "\"}");

    compi = strlen(json_start);
    content_length_here[0] = '0' + (compi / 10);
    content_length_here[1] = '0' + (compi % 10);
    client.print(post_rqst);
  }
}

void logSDCard() {
  dataMessage =  String(dayStamp) + "," + String(timeStamp) + "," + 
                String(Temperature) + "," + String(Humidity)+ "\r\n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/data.csv", dataMessage.c_str());
}
// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
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

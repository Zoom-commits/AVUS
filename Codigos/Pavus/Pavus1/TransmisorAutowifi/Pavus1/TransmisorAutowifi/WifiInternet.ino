//************ LIBRERIAS DE WIFI ***************************************//
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
#include <AutoConnect.h>
#include <NTPClient.h>
#include <ESP32Time.h>


//************ DEFINICION DE TIEMPO SLEEP MODE **********************//
#define uS_TO_S_FACTOR 1000000 //Conversion factor for micro to seconds
#define TIME_TO_SLEEP 60       //Time EPS32 will go to sleep (in seconds)

//************ DEFINICION DE SERVIDOR IFTTT ************************//
#define key "dcd50PRWdVFAnMTFbBRKRU"; 
#define Event "PAVUS 1";

WebServer         Server;
AutoConnect       Portal(Server);
AutoConnectConfig Config;       // Enable autoReconnect supported on v0.9.4
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

//************* NOTIFICACIONES ******************************************//
bool approve = true;
bool approve1 = true;
bool approve2 = true;


void portalcito(){
Portal.handleClient();
}

void rtcWifi(){
//************* INTEN RTC CONFIGURATION ***********************// 
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)){
      rtc.setTimeStruct(timeinfo); 
    }
}
void configSetUp(){ 
//************* Configuración de AutoConnect WiFi *************************//
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
  
//************* Servidor web de configuración *************************//
  Server.on("/", rootPage);
  Server.on("/start", startPage);   // Set NTP server trigger handler

// Establish a connection with an autoReconnect option.
  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
    Serial.println(WiFi.getHostname());
  }
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

//************* NOTIFICACTIONS AND SLEEP MODE ***********************// 
void Notificaciones(){
 if(rtc.getHour(true) == hora1 && rtc.getMinute()== minuto1 && approve==true){
    approve=false;
    const char * notif1 = nombreCancion1.c_str();
    audio.connecttoFS(SD,notif1);
    Serial.println(timeClient.getDay());
    Serial.println("Recordatorio 1");
 
    audio.loop();
 } else if (rtc.getHour(true) == hora1 && rtc.getMinute() > minuto1){
    approve=true;
 }
}

void Notificaciones1(){
 if(rtc.getHour(true) == hora2 && rtc.getMinute()==minuto2 && approve1==true){
    approve1=false;
     const char * notif2 = nombreCancion2.c_str();
    audio.connecttoFS(SD,notif2);
    Serial.println("Recordatorio 2");
    audio.loop();
 } else if (rtc.getHour(true) == hora2 && rtc.getMinute()>minuto2){
    approve1=true;
 }
}
void Notificaciones2(){
 if(rtc.getHour(true) == hora3 && rtc.getMinute()==minuto3 && approve2==true){
    approve2=false;
    const char * notif3 = nombreCancion3.c_str();
    audio.connecttoFS(SD,notif3);
    Serial.println("Recordatorio 3");
    audio.loop();
 } else if (rtc.getHour(true) == hora3 && rtc.getMinute()>minuto3){
    approve2=true;
 }
}
// ------- NOTIFICACIONES DE PRUEBAA -----

void Notificaciones3(){
 if(rtc.getHour(true) == hora4 && rtc.getMinute()== minuto4 && approve==true){
    approve=false;
     const char * notif4 = nombreCancion4.c_str();
    audio.connecttoFS(SD,notif4);
    Serial.println(timeClient.getDay());
    Serial.println("Recordatorio 4");
    audio.loop();
 } else if (rtc.getHour(true) == hora4 && rtc.getMinute() > minuto4){
    approve=true;
 }
}

void Notificaciones4(){
 if(rtc.getHour(true) == hora5 && rtc.getMinute()==minuto5 && approve1==true){
    approve1=false; 
    const char * notif5 = nombreCancion5.c_str();
    audio.connecttoFS(SD,notif5);
    Serial.println("Recordatorio 5");
    audio.loop();
 } else if (rtc.getHour(true) == hora5 && rtc.getMinute()>minuto5){
    approve1=true;
 }
}

// ------- FIN DE PRUEBAS -----------------


void Apagado(){
 if(rtc.getHour(true) == 15 && rtc.getMinute()==00 && approve2==true){
    approve2=false;
    Serial.println("A mimir");
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
 } else if (rtc.getHour(true) == 15 && rtc.getMinute()>00){
    approve2=true;
 }
}

String printLocalTime()
{
  rtc.getTimeDate(true);
  
  return rtc.getTimeDate(true);
}

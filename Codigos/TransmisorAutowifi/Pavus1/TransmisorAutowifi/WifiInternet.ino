//************ LIBRERIAS DE WIFI ***************************************//
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
#include <AutoConnect.h>
#include <NTPClient.h>
#include <ESP32Time.h>

// -- librerías de servidor SD -----
#include "SPIFFS.h"
#include "CSS.h"



//String  webpage = "";
//bool    SPIFFS_present = false;



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


WebServer server(80); // Web Server SD

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
//************* Server SD ***************************************************//
void ServerSDsetup(){
  
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS initialisation failed...");
    SPIFFS_present = false; 
  }
  else
  {
    Serial.println(F("SPIFFS initialised... file access enabled..."));
    SPIFFS_present = true; 
  }
  ///////////////////////////// Server Commands 
  server.on("/",         HomePage);
  server.on("/download", File_Download);
  server.on("/upload",   File_Upload);
  server.on("/fupload",  HTTP_POST,[](){ server.send(200);}, handleFileUpload);
  server.on("/stream",   File_Stream);
  server.on("/delete",   File_Delete);
  server.on("/dir",      SPIFFS_dir);
  
  ///////////////////////////// End of Request commands
  server.begin();
  Serial.println("HTTP server started");
}

void request(){
  server.handleClient(); // Listen for client connections
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

//*********************SERVER SD****************************************

// All supporting functions from here...
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void HomePage(){
  SendHTML_Header();
  webpage += F("<a href='/download'><button>Download</button></a>");
  webpage += F("<a href='/upload'><button>Upload</button></a>");
  webpage += F("<a href='/stream'><button>Stream</button></a>");
  webpage += F("<a href='/delete'><button>Delete</button></a>");
  webpage += F("<a href='/dir'><button>Directory</button></a>");
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop(); // Stop is needed because no content length was sent
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void File_Download(){ // This gets called twice, the first pass selects the input, the second pass then processes the command line arguments
  if (server.args() > 0 ) { // Arguments were received
    if (server.hasArg("download")) DownloadFile(server.arg(0));
  }
  else SelectInput("Enter filename to download","download","download");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DownloadFile(String filename){
  if (SPIFFS_present) { 
    File download = SPIFFS.open("/"+filename,  "r");
    if (download) {
      server.sendHeader("Content-Type", "text/text");
      server.sendHeader("Content-Disposition", "attachment; filename="+filename);
      server.sendHeader("Connection", "close");
      server.streamFile(download, "application/octet-stream");
      download.close();
    } else ReportFileNotPresent("download"); 
  } else ReportSPIFFSNotPresent();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void File_Upload(){
  append_page_header();
  webpage += F("<h3>Select File to Upload</h3>"); 
  webpage += F("<FORM action='/fupload' method='post' enctype='multipart/form-data'>");
  webpage += F("<input class='buttons' style='width:40%' type='file' name='fupload' id = 'fupload' value=''><br>");
  webpage += F("<br><button class='buttons' style='width:10%' type='submit'>Upload File</button><br>");
  webpage += F("<a href='/'>[Back]</a><br><br>");
  append_page_footer();
  server.send(200, "text/html",webpage);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
File UploadFile; 
void handleFileUpload(){ // upload a new file to the Filing system
  HTTPUpload& uploadfile = server.upload(); // See https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/srcv
                                            // For further information on 'status' structure, there are other reasons such as a failed transfer that could be used
  if(uploadfile.status == UPLOAD_FILE_START)
  {
    String filename = uploadfile.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    Serial.print("Upload File Name: "); Serial.println(filename);
    SPIFFS.remove(filename);                  // Remove a previous version, otherwise data is appended the file again
    UploadFile = SPIFFS.open(filename, "w");  // Open the file for writing in SPIFFS (create it, if doesn't exist)
  }
  else if (uploadfile.status == UPLOAD_FILE_WRITE)
  {
    if(UploadFile) UploadFile.write(uploadfile.buf, uploadfile.currentSize); // Write the received bytes to the file
  } 
  else if (uploadfile.status == UPLOAD_FILE_END)
  {
    if(UploadFile)          // If the file was successfully created
    {                                    
      UploadFile.close();   // Close the file again
      Serial.print("Upload Size: "); Serial.println(uploadfile.totalSize);
      webpage = "";
      append_page_header();
      webpage += F("<h3>File was successfully uploaded</h3>"); 
      webpage += F("<h2>Uploaded File Name: "); webpage += uploadfile.filename+"</h2>";
      webpage += F("<h2>File Size: "); webpage += file_size(uploadfile.totalSize) + "</h2><br>"; 
      append_page_footer();
      server.send(200,"text/html",webpage);
    } 
    else
    {
      ReportCouldNotCreateFile("upload");
    }
  }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//#ifdef ESP32
void SPIFFS_dir(){ 
  if (SPIFFS_present) { 
    File root = SPIFFS.open("/");
    if (root) {
      root.rewindDirectory();
      SendHTML_Header();
      webpage += F("<h3 class='rcorners_m'>SD Card Contents</h3><br>");
      webpage += F("<table align='center'>");
      webpage += F("<tr><th>Name/Type</th><th style='width:20%'>Type File/Dir</th><th>File Size</th></tr>");
      printDirectory("/",0);
      webpage += F("</table>");
      SendHTML_Content();
      root.close();
    }
    else 
    {
      SendHTML_Header();
      webpage += F("<h3>No Files Found</h3>");
    }
    append_page_footer();
    SendHTML_Content();
    SendHTML_Stop();   // Stop is needed because no content length was sent
  } else ReportSPIFFSNotPresent();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void printDirectory(const char * dirname, uint8_t levels){
  File root = SPIFFS.open(dirname);
  if(!root){
    return;
  }
  if(!root.isDirectory()){
    return;
  }
  File file = root.openNextFile();
  while(file){
    if (webpage.length() > 1000) {
      SendHTML_Content();
    }
    if(file.isDirectory()){
      webpage += "<tr><td>"+String(file.isDirectory()?"Dir":"File")+"</td><td>"+String(file.name())+"</td><td></td></tr>";
      printDirectory(file.name(), levels-1);
    }
    else
    {
      webpage += "<tr><td>"+String(file.name())+"</td>";
      webpage += "<td>"+String(file.isDirectory()?"Dir":"File")+"</td>";
      int bytes = file.size();
      String fsize = "";
      if (bytes < 1024)                     fsize = String(bytes)+" B";
      else if(bytes < (1024 * 1024))        fsize = String(bytes/1024.0,3)+" KB";
      else if(bytes < (1024 * 1024 * 1024)) fsize = String(bytes/1024.0/1024.0,3)+" MB";
      else                                  fsize = String(bytes/1024.0/1024.0/1024.0,3)+" GB";
      webpage += "<td>"+fsize+"</td></tr>";
    }
    file = root.openNextFile();
  }
  file.close();
}
//#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//#ifdef ESP8266
//void SPIFFS_dir(){
//  String str;
//  if (SPIFFS_present) { 
//    Dir dir = SPIFFS.openDir("/");
//    SendHTML_Header();
//    webpage += F("<h3 class='rcorners_m'>SPIFFS Card Contents</h3><br>");
//    webpage += F("<table align='center'>");
//    webpage += F("<tr><th>Name/Type</th><th style='width:40%'>File Size</th></tr>");
//    while (dir.next()) {
//      Serial.print(dir.fileName());
//      webpage += "<tr><td>"+String(dir.fileName())+"</td>";
//      str  = dir.fileName();
//      str += " / ";
//      if(dir.fileSize()) {
//        File f = dir.openFile("r");
//        Serial.println(f.size());
//        int bytes = f.size();
//        String fsize = "";
//        if (bytes < 1024)                     fsize = String(bytes)+" B";
//        else if(bytes < (1024 * 1024))        fsize = String(bytes/1024.0,3)+" KB";
//        else if(bytes < (1024 * 1024 * 1024)) fsize = String(bytes/1024.0/1024.0,3)+" MB";
//        else                                  fsize = String(bytes/1024.0/1024.0/1024.0,3)+" GB";
//        webpage += "<td>"+fsize+"</td></tr>";
//        f.close();
//      }
//      str += String(dir.fileSize());
//      str += "\r\n";
//      Serial.println(str);
//    }
//    webpage += F("</table>");
//    SendHTML_Content();
//    append_page_footer();
//    SendHTML_Content();
//    SendHTML_Stop();   // Stop is needed because no content length was sent
//  } else ReportSPIFFSNotPresent();
//}
//#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void File_Stream(){
  if (server.args() > 0 ) { // Arguments were received
    if (server.hasArg("stream")) SPIFFS_file_stream(server.arg(0));
  }
  else SelectInput("Enter a File to Stream","stream","stream");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SPIFFS_file_stream(String filename) { 
  if (SPIFFS_present) { 
    File dataFile = SPIFFS.open("/"+filename,  "r"); // Now read data from SPIFFS Card 
    if (dataFile) { 
      if (dataFile.available()) { // If data is available and present 
        String dataType = "application/octet-stream"; 
        if (server.streamFile(dataFile, dataType) != dataFile.size()) {Serial.print(F("Sent less data than expected!")); } 
      }
      dataFile.close(); // close the file: 
    } else ReportFileNotPresent("Cstream");
  } else ReportSPIFFSNotPresent(); 
}   
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void File_Delete(){
  if (server.args() > 0 ) { // Arguments were received
    if (server.hasArg("delete")) SPIFFS_file_delete(server.arg(0));
  }
  else SelectInput("Select a File to Delete","delete","delete");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SPIFFS_file_delete(String filename) { // Delete the file 
  if (SPIFFS_present) { 
    SendHTML_Header();
    File dataFile = SPIFFS.open("/"+filename, "r"); // Now read data from SPIFFS Card 
    if (dataFile)
    {
      if (SPIFFS.remove("/"+filename)) {
        Serial.println(F("File deleted successfully"));
        webpage += "<h3>File '"+filename+"' has been erased</h3>"; 
        webpage += F("<a href='/delete'>[Back]</a><br><br>");
      }
      else
      { 
        webpage += F("<h3>File was not deleted - error</h3>");
        webpage += F("<a href='delete'>[Back]</a><br><br>");
      }
    } else ReportFileNotPresent("delete");
    append_page_footer(); 
    SendHTML_Content();
    SendHTML_Stop();
  } else ReportSPIFFSNotPresent();
} 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SendHTML_Header(){
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate"); 
  server.sendHeader("Pragma", "no-cache"); 
  server.sendHeader("Expires", "-1"); 
  server.setContentLength(CONTENT_LENGTH_UNKNOWN); 
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves. 
  append_page_header();
  server.sendContent(webpage);
  webpage = "";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SendHTML_Content(){
  server.sendContent(webpage);
  webpage = "";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SendHTML_Stop(){
  server.sendContent("");
  server.client().stop(); // Stop is needed because no content length was sent
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SelectInput(String heading1, String command, String arg_calling_name){
  SendHTML_Header();
  webpage += F("<h3>"); webpage += heading1 + "</h3>"; 
  webpage += F("<FORM action='/"); webpage += command + "' method='post'>"; // Must match the calling argument e.g. '/chart' calls '/chart' after selection but with arguments!
  webpage += F("<input type='text' name='"); webpage += arg_calling_name; webpage += F("' value=''><br>");
  webpage += F("<type='submit' name='"); webpage += arg_calling_name; webpage += F("' value=''><br><br>");
  webpage += F("<a href='/'>[Back]</a><br><br>");
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ReportSPIFFSNotPresent(){
  SendHTML_Header();
  webpage += F("<h3>No SPIFFS Card present</h3>"); 
  webpage += F("<a href='/'>[Back]</a><br><br>");
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ReportFileNotPresent(String target){
  SendHTML_Header();
  webpage += F("<h3>File does not exist</h3>"); 
  webpage += F("<a href='/"); webpage += target + "'>[Back]</a><br><br>";
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ReportCouldNotCreateFile(String target){
  SendHTML_Header();
  webpage += F("<h3>Could Not Create Uploaded File (write-protected?)</h3>"); 
  webpage += F("<a href='/"); webpage += target + "'>[Back]</a><br><br>";
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
String file_size(int bytes){
  String fsize = "";
  if (bytes < 1024)                 fsize = String(bytes)+" B";
  else if(bytes < (1024*1024))      fsize = String(bytes/1024.0,3)+" KB";
  else if(bytes < (1024*1024*1024)) fsize = String(bytes/1024.0/1024.0,3)+" MB";
  else                              fsize = String(bytes/1024.0/1024.0/1024.0,3)+" GB";
  return fsize;
}

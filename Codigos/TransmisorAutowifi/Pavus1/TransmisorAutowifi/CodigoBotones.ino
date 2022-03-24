const int b2=22, b1=21, b3=2, b4=33, b5=34, b6=35;


void setBotones(){
pinMode(b1, INPUT); 
pinMode(b2, INPUT); 
pinMode(b3, INPUT); 
pinMode(b4, INPUT); 
pinMode(b5, INPUT);
pinMode(b6, INPUT);   
}

void activarBotones(){
  
   if(digitalRead(b1)){
      llavesitas(11,"/Device1.mp3",printLocalTime(),"Dispositivo 1");
      Serial.println(printLocalTime());
   }
   if(digitalRead(b2)){
      llavesitas(22,"/Device2.mp3",printLocalTime(),"Dispositivo 2");
        Serial.println(printLocalTime());
   }
   if(digitalRead(b3)){
      llavesitas(33,"/Device3.mp3",printLocalTime(),"Dispositivo 3");
        Serial.println(printLocalTime());
   }
   if(digitalRead(b4)){
      llavesitas(44,"/Device4.mp3",printLocalTime(),"Dispositivo 4");
        Serial.println(printLocalTime());
   }
   if(digitalRead(b5)){
      llavesitas(55,"/Device5.mp3",printLocalTime(),"Dispositivo 5");
        Serial.println(printLocalTime());
   }
   if(digitalRead(b6)){
      llavesitas(66,"/Device6.mp3",printLocalTime(),"Dispositivo 6");
      Serial.println(printLocalTime());
   }

}

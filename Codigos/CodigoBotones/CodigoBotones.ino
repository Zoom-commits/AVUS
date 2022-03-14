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
      llavesitas(111,"/Device1.mp3","Hoy");
   }
   if(digitalRead(b2)){
      llavesitas(222,"/Device2.mp3","Hoy");
   }
   if(digitalRead(b3)){
      llavesitas(333,"/Device3.mp3","Hoy");
   }
   if(digitalRead(b4)){
      llavesitas(444,"/Device4.mp3","Hoy");
   }
   if(digitalRead(b5)){
      llavesitas(555,"/Device5.mp3","Hoy");
   }
   if(digitalRead(b6)){
      llavesitas(666,"/Device6.mp3","Hoy");
   }

}

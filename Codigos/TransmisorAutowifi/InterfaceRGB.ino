//************* Libreria y definición LEDs RGB *************************//
#include  <Adafruit_NeoPixel.h> 
#define   PIN 22          //Pin GPIO
#define   NUMPIXELS  8    //# de pixeles en la cinta led RGB
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setUpRGB(){
   pixels.begin(); 
   pixels.setBrightness(34);
   mostrarRGB(0,255,0,0);
   mostrarRGB(1,255,0,0);
   mostrarRGB(2,255,0,0);
   mostrarRGB(3,255,0,0);
   mostrarRGB(4,255,0,0);
   mostrarRGB(5,255,0,0);
   mostrarRGB(6,255,0,0);
   mostrarRGB(7,255,0,0);
}
void mostrarRGB(int pix, int r, int g, int b){
   pixels.setPixelColor(pix, r, g, b);
   pixels.show();
}
 

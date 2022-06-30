#include <Arduino.h>

#include <Adafruit_NeoPixel.h>

#define PIN 26
#define NUMPIXELS 7

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 20 

void setup() {
  delay(100);
  pixels.begin();
}

void displayAll(int r, int g){
  for (int pixel = 0; pixel < NUMPIXELS; pixel++) {
    pixels.setPixelColor(pixel, pixels.Color(r, g, 0));
  }
  pixels.show();
  delay(DELAYVAL);
}

void loop() {
  int r = 0;
  int g = 255;
  while (r < 255) {
    displayAll(r,g);
    r++;
  }
  //r 255 g 255
  while(g>0){
    displayAll(r, g);
    g--;
  }
  //r 255 g 0
  while (g < 255) {
    displayAll(r, g);
    g++;
  }
  //r 255 g 255
  while (r > 0) {
    displayAll(r, g);
    r--;
  }
  //r 0 g 255
}
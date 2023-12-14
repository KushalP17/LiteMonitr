#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>
#define PIN_WS2812B 16
#define NUM_PANELS 8
#define NUM_PIXELS NUM_PANELS * 8 * 32

double brightness = .5;

Adafruit_NeoPixel ws2812b(NUM_PIXELS, PIN_WS2812B, NEO_GRB + NEO_KHZ800);

void drawSinglePixel(uint8_t row, uint8_t col, uint8_t R, uint8_t G, uint8_t B){
  int pixel;

  int rowMod16 = row % 16;
  int colMod2 = col % 2;
  int rowby8 = row / 8;


  if(rowMod16 < 8){
    if(colMod2){
      int pixOnPanel = 256*rowby8; //pix 0 on panel
      pixOnPanel += (8*col); //top pix on column
      pixel = pixOnPanel + (7-rowMod16); //correct pix w/ row and col
    }else{
      int pixOnPanel = 256*rowby8; //pix 0 on panel
      pixOnPanel += (8*col); //top pix on column
      pixel = pixOnPanel + rowMod16; //correct pix w/ row and col
    }
  }else{
    if(colMod2){
      int pixOnPanel = 256*rowby8; //pix 0 on panel
      pixOnPanel += (8*(31-col)); //top pix on column
      pixel = pixOnPanel + (15-rowMod16); //correct pix w/ row and col
    }else{
      int pixOnPanel = 256*rowby8; //pix 0 on panel
      pixOnPanel += (8*(31-col)); //top pix on column
      pixel = pixOnPanel + (rowMod16-8); //correct pix w/ row and col
    }
  }

  ws2812b.setPixelColor(pixel, ws2812b.Color(round(R*brightness), round(G*brightness), round(B*brightness)));
  ws2812b.show();
}

void setup() {
  // put your setup code here, to run once:
  ws2812b.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  drawSinglePixel(0, 0, 255, 255, 255);
  drawSinglePixel(63, 31, 255, 0, 0);
  drawSinglePixel(43, 19, 0, 255, 255);
  drawSinglePixel(43, 18, 0, 255, 255);
  drawSinglePixel(43, 17, 0, 255, 255);
  drawSinglePixel(43, 16, 0, 255, 255);
  drawSinglePixel(43, 15, 0, 255, 255);
  drawSinglePixel(43, 14, 0, 255, 255);
  drawSinglePixel(42, 19, 0, 255, 255);
  drawSinglePixel(41, 19, 0, 255, 255);
  drawSinglePixel(40, 19, 0, 255, 255);
  drawSinglePixel(39, 19, 0, 255, 255);
  drawSinglePixel(38, 19, 0, 255, 255);

}

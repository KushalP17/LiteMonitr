#include <Adafruit_NeoPixel.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <math.h>
#include "imagepresets.h"

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define PIN_WS2812B 16
#define NUM_PANELS 8
#define NUM_PIXELS NUM_PANELS * 8 * 32

double brightness = .5;
bool newDataAvailable = false;
bool deviceConnected = false;
std::string value;

Adafruit_NeoPixel ws2812b(NUM_PIXELS, PIN_WS2812B, NEO_GRB + NEO_KHZ800);

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    }

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        pServer->startAdvertising();
    }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    value = pCharacteristic->getValue();

    if (value.length() > 0) {
      // Serial.println("*********");
      // Serial.print("New value: ");
      // for (int i = 0; i < value.length(); i++) {
      //   Serial.print(value[i]);
      // }

      // Serial.println();
      // Serial.println("*********");
      newDataAvailable = true;
    }
  }
};

void mapIDX1TopLeft(const uint8_t sample[][32][3], uint8_t rowOffset, uint8_t colOffset, int pixelOffset) {
  int pixel = 0 + pixelOffset;

  for (int col = 0; col < 32; col++) {
    for (int row = 0; row < 8; row++) {
      if (col % 2 == 1) {
        ws2812b.setPixelColor(pixel, ws2812b.Color(round(sample[7 - row + rowOffset][col + colOffset][0]*brightness), round(sample[7 - row + rowOffset][col][1]*brightness), round(sample[7 - row + rowOffset][col][2]*brightness)));
      } else {
        ws2812b.setPixelColor(pixel, ws2812b.Color(round(brightness*sample[row + rowOffset][col + colOffset][0]), round(brightness*sample[row + rowOffset][col + colOffset][1]), round(brightness*sample[row + rowOffset][col + colOffset][2])));
      }
      pixel++;
    }
  }
}

void mapIDX1BottomRight(const uint8_t sample[][32][3], uint8_t rowOffset, uint8_t colOffset, int pixelOffset) {
  int pixel = 0 + pixelOffset;

  for (int col = 0; col < 32; col++) {
    for (int row = 0; row < 8; row++) {
      if (col % 2 == 1) {
        ws2812b.setPixelColor(pixel, ws2812b.Color(round(brightness*sample[row + rowOffset][31 - col + colOffset][0]), round(brightness*sample[row + rowOffset][31 - col + colOffset][1]), round(brightness*sample[row + rowOffset][31 - col + colOffset][2])));
      } else {
        ws2812b.setPixelColor(pixel, ws2812b.Color(round(brightness*sample[7 - row + rowOffset][31 - col + colOffset][0]), round(brightness*sample[7 - row + rowOffset][31 - col + colOffset][1]), round(brightness*sample[7 - row + rowOffset][31 - col + colOffset][2])));
      }
      pixel++;
    }
  }
}

void draw64x32(const uint8_t sample[][32][3]) {
  mapIDX1TopLeft(sample, 0, 0, 0);
  mapIDX1BottomRight(sample, 8, 0, 256);
  mapIDX1TopLeft(sample, 16, 0, 512);
  mapIDX1BottomRight(sample, 24, 0, 768);
  mapIDX1TopLeft(sample, 32, 0, 1024);
  mapIDX1BottomRight(sample, 40, 0, 1280);
  mapIDX1TopLeft(sample, 48, 0, 1536);
  mapIDX1BottomRight(sample, 56, 0, 1792);

  ws2812b.show();
}

void draw32x32(const uint8_t sample[][32][3]) {
  mapIDX1TopLeft(sample, 0, 0, 0);
  mapIDX1BottomRight(sample, 8, 0, 256);
  mapIDX1TopLeft(sample, 16, 0, 512);
  mapIDX1BottomRight(sample, 24, 0, 768);
  ws2812b.show();
}

void drawSinglePixel(uint8_t row, uint8_t col, uint8_t R, uint8_t G, uint8_t B){
  int pixel;

  int rowMod16 = row % 16;
  int colMod2 = col % 2;
  int rowby8 = row / 8;
  int pixOnPanel = 256*rowby8; //pix 0 on panel


  if(rowMod16 < 8){
    if(colMod2){
      pixOnPanel += (8*col); //top pix on column
      pixel = pixOnPanel + (7-rowMod16); //correct pix w/ row and col
    }else{
      pixOnPanel += (8*col); //top pix on column
      pixel = pixOnPanel + rowMod16; //correct pix w/ row and col
    }
  }else{
    if(colMod2){
      pixOnPanel += (8*(31-col)); //top pix on column
      pixel = pixOnPanel + (15-rowMod16); //correct pix w/ row and col
    }else{
      pixOnPanel += (8*(31-col)); //top pix on column
      pixel = pixOnPanel + (rowMod16-8); //correct pix w/ row and col
    }
  }

  ws2812b.setPixelColor(pixel, ws2812b.Color(round(R*brightness), round(G*brightness), round(B*brightness)));
  ws2812b.show();
}

void setSinglePixel(uint8_t row, uint8_t col, uint8_t R, uint8_t G, uint8_t B){
  int pixel;

  int rowMod16 = row % 16;
  int colMod2 = col % 2;
  int rowby8 = row / 8;
  int pixOnPanel = 256*rowby8; //pix 0 on panel


  if(rowMod16 < 8){
    if(colMod2){
      pixOnPanel += (8*col); //top pix on column
      pixel = pixOnPanel + (7-rowMod16); //correct pix w/ row and col
    }else{
      pixOnPanel += (8*col); //top pix on column
      pixel = pixOnPanel + rowMod16; //correct pix w/ row and col
    }
  }else{
    if(colMod2){
      pixOnPanel += (8*(31-col)); //top pix on column
      pixel = pixOnPanel + (15-rowMod16); //correct pix w/ row and col
    }else{
      pixOnPanel += (8*(31-col)); //top pix on column
      pixel = pixOnPanel + (rowMod16-8); //correct pix w/ row and col
    }
  }

  ws2812b.setPixelColor(pixel, ws2812b.Color(round(R*brightness), round(G*brightness), round(B*brightness)));
}


void setup() {

  ws2812b.begin();

  Serial.begin(115200);

  BLEDevice::init("LiteMonitr");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic =
    pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("Connect to BLE now - Program Start");
}

int option = -1;

void loop() {
  
  if (deviceConnected && newDataAvailable) {
    if(value.length() == 1){
      Serial.print("Received value: ");
      Serial.println((int)value[0]);

      int preOption = (int)value[0];
      Serial.print("PreOption is ");
      Serial.println(preOption);

      if(preOption == 6){
        brightness -= .05;
        if(brightness < 0) brightness = 0;
        Serial.println("Brightness decrease");
      }else if(preOption == 7){
        brightness += .05;
        if(brightness > 1) brightness = 1;
        Serial.println("Brightness increase");
      }else{
        // built-in prev option to auto adjust brightness
        option = (int)value[0];
        Serial.print("Option is ");
        Serial.println(option);
      }

      switch(option){
        case 0:
          ws2812b.clear();
          ws2812b.show();
          Serial.println("Cleared");
          break;
        case 1:
          // draw64x32(testimage);
          // Serial.println("testimage");
          draw64x32(snake);
          draw64x32(snake2);
          draw64x32(snake3);
          draw64x32(snake4);
          draw64x32(snake5);
          draw64x32(snake6);
          draw64x32(snake7);
          draw64x32(snake8);
          draw64x32(snake9);
          draw64x32(snake10);
          draw64x32(snake11);

          break;
        case 2:
          draw64x32(mariospritetest);
          Serial.println("mariospritetest");
          break;
        case 3:
          draw64x32(pacman);
          draw64x32(pacman2);
          draw64x32(pacman3);
          draw64x32(pacman2);
          draw64x32(pacman);
          draw64x32(pacman2);
          draw64x32(pacman3);
          draw64x32(pacman2);
          draw64x32(pacman);
          draw64x32(pacman2);
          draw64x32(pacman3);
          draw64x32(pacman2);
          draw64x32(pacman);
          Serial.println("pacman");
          break;
        case 4:
          draw64x32(polkadots);
          Serial.println("polkadots");
          break;
        case 5:
          draw64x32(stickman);
          Serial.println("stickman");
          break;
      }

    }else if(value.length() == 20 && value[0] == 37){
      uint8_t R = value[1];
      uint8_t G = value[2];
      uint8_t B = value[3];

      uint8_t rowIdx = 4;
      uint8_t colIdx = 5;

      // Serial.println("drawing (" + String(R) + ", " + String(G) + ", " + String(B) + " ) @ " + String(row) + ", " + String(col));

      while(value[rowIdx] < 64 && value[colIdx] < 32 && rowIdx < 20 && colIdx < 20) {

        setSinglePixel(value[rowIdx], value[colIdx], R, G, B);
        Serial.println("drawing (" + String(R) + ", " + String(G) + ", " + String(B) + " ) @ " + String((uint8_t)value[rowIdx]) + ", " + String((uint8_t)value[colIdx]));

        rowIdx += 2;
        colIdx += 2;

        
      }

      ws2812b.show();

    }
    // Reset the flag
      newDataAvailable = false;
  }
}

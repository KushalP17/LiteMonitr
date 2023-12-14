#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <PNGdec.h>
#include "FS.h"
#include <LittleFS.h>

#define FileSys LittleFS
#define PIN_WS2812B 16
#define NUM_PANELS 8
#define NUM_PIXELS NUM_PANELS * 8 * 32
PNG png;


File pngfile;

void *pngOpen(const char *filename, int32_t *size) {
  Serial.printf("Attempting to open %s\n", filename);
  pngfile = FileSys.open(filename, "r");
  *size = pngfile.size();
  return &pngfile;
}

void pngClose(void *handle) {
  File pngfile = *((File *)handle);
  if (pngfile) pngfile.close();
}

int32_t pngRead(PNGFILE *page, uint8_t *buffer, int32_t length) {
  if (!pngfile) return 0;
  page = page;  // Avoid warning
  return pngfile.read(buffer, length);
}

int32_t pngSeek(PNGFILE *page, int32_t position) {
  if (!pngfile) return 0;
  page = page;  // Avoid warning
  return pngfile.seek(position);
}


typedef struct myprivate {
  bool bConvert;
} PRIVATE;

void PNGDraw(PNGDRAW *pDraw) {
  PRIVATE *pPriv = (PRIVATE *)pDraw->pUser;
  uint16_t usPixels[240];

  if (pPriv->bConvert)
    png.getLineAsRGB565(pDraw, usPixels, PNG_RGB565_LITTLE_ENDIAN, 0xffffffff);  // don't do alpha color blending
} /* PNGDraw() */

String file = "/test.png";

Adafruit_NeoPixel ws2812b(NUM_PIXELS, PIN_WS2812B, NEO_GRB + NEO_KHZ800);




void setup() {
  // put your setup code here, to run once:
  Serial.begin(921600);
}

void loop() {
  long lTime;
  char szTemp[256];
  PRIVATE priv;

  Serial.println("Start Check");

  //open(const char *szFilename, PNG_OPEN_CALLBACK *pfnOpen, PNG_CLOSE_CALLBACK *pfnClose, PNG_READ_CALLBACK *pfnRead, PNG_SEEK_CALLBACK *pfnSeek, PNG_DRAW_CALLBACK *pfnDraw);
  int rc = png.open(file.c_str(), pngOpen, pngClose, pngRead, pngSeek, PNGDraw);
  Serial.println(rc);
  if (rc == PNG_SUCCESS) {
    Serial.println("Successfully opened test.png");
    priv.bConvert = false;
    lTime = micros();
    rc = png.decode((void *)&priv, PNG_FAST_PALETTE);
    lTime = micros() - lTime;
    sprintf(szTemp, "Decode time for native pixels = %d us\n", (int)lTime);
    Serial.print(szTemp);
    priv.bConvert = true;
    lTime = micros();
    rc = png.decode((void *)&priv, PNG_FAST_PALETTE);
    lTime = micros() - lTime;
    sprintf(szTemp, "Decode time for RGB565 pixels = %d us\n", (int)lTime);
    Serial.print(szTemp);
    png.close();  // not needed for memory->memory decode
  }
}

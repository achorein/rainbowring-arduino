#include <Adafruit_NeoPixel.h>

#define PIXEL_NUMBER 16
#define COLOR_NUMBER 32

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_NUMBER, 10, NEO_GRB + NEO_KHZ400);

uint8_t  mode   = 2, // Current animation effect
         offset = 0, // Position of animation (rotate)
         color_offset = 0, // current rainbow color
         animation_duration = 10000; // animation duration
uint32_t prevTime;
uint32_t color[COLOR_NUMBER]; // = {0xFF0000,0xE2571E,0xFF7F00,0xFFFF00,0x00FF00,0x96bf33,0x0000FF,0x4B0082,0x8B00FF,0xffffff};
//uint32_t color[7] = {0xFF0000, 0xFF7F00, 0xFFFF00, 0x00FF00, 0x0000FF, 0x4B0082, 0x9400D3};

void setup() {
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
  if(F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  generateColor();
  pixels.begin();
  pixels.setBrightness(10); // 1/3 brightness
  prevTime = millis();
}

void generateColor() { 
  float frequency = .3;
  for (uint8_t i=0;i<COLOR_NUMBER;++i)
  {
     uint32_t R = sin(frequency*i + 0) * 127 + 128;
     uint32_t G = sin(frequency*i + 2) * 127 + 128;
     uint32_t B = sin(frequency*i + 4) * 127 + 128;
     color[i] = (R << 16) | (G << 8) | B;
  }
}

void loop() {
  uint8_t  i, pos;
  uint32_t t;
 
  switch(mode) {
 
   case 0: // smiley
    animation_duration = 10000; // 10sec
    for (i=0;i<PIXEL_NUMBER;i++) pixels.setPixelColor(i, 0);
    color_offset = 0;
    pos = (1+offset)%PIXEL_NUMBER;
    /*for (i=pos;i<pos+6;i++) {
      pixels.setPixelColor(i%PIXEL_NUMBER, color[color_offset]);
      color_offset += 5;
    }*/
    for (i=pos;i<pos+2;i++) {
      pixels.setPixelColor(i%PIXEL_NUMBER, 0x0000FF);
    }
    for (;i<pos+4;i++) {
      pixels.setPixelColor(i%PIXEL_NUMBER, 0xFFFFFF);
    }
    for (;i<pos+6;i++) {
      pixels.setPixelColor(i%PIXEL_NUMBER, 0xFF0000);
    }
    pixels.setPixelColor((10+offset)%PIXEL_NUMBER, 0xFFFFFF);
    pixels.setPixelColor((13+offset)%PIXEL_NUMBER, 0xFFFFFF);
    pixels.show();
    offset++;
    delay(200);
    break;
 
   case 1: // Spinny wheels (8 LEDs on at a time)
    animation_duration = 5000; // 5sec
    for(i=0; i<16; i++) {
      uint32_t c = 0;
      if(((offset + i) & 7) < 2) c = color[color_offset]; // 4 pixels on...
      pixels.setPixelColor(   i, c); // First eye
      pixels.setPixelColor(31-i, c); // Second eye (flipped)
    }
    pixels.show();
    offset++;
    delay(50);
    break;

  case 2: // French Flag
    animation_duration = 10000; // 10sec
    pos = (offset)%PIXEL_NUMBER;
    for(i=pos; i<5+pos; i++) {
      pixels.setPixelColor(i%PIXEL_NUMBER, 0xFF0000);
    }
    for(; i<8+pos; i++) {
      pixels.setPixelColor(i%PIXEL_NUMBER, 0xFFFFFF);
    }
    for(; i<13+pos; i++) {
      pixels.setPixelColor(i%PIXEL_NUMBER, 0x0000FF);
    }
    for(; i<16+pos; i++) {
      pixels.setPixelColor(i%PIXEL_NUMBER, 0xFFFFFF);
    }
    pixels.show();
    offset++;
    delay(500);
    break;
 
  case 3: // Snake
    animation_duration = 5000; // 5sec
    pos = offset%16;
    if (pos-1 < 0 )
      pixels.setPixelColor(PIXEL_NUMBER-1, 0);
    else
      pixels.setPixelColor(pos-1, 0);
    for(i=pos; i<pos+6; i++) {
      pixels.setPixelColor(i%PIXEL_NUMBER, color[color_offset]); // Body
    }
    pixels.setPixelColor((i+1)%PIXEL_NUMBER, 0);
    pixels.show();
    offset++;
    delay(50);
    break;

  case 4: // Full ring color (rainbow)
    animation_duration = 5000; // 5sec
    for(i=0; i<16; i++) {
      pixels.setPixelColor(i, color[color_offset]); // Body
    }
    pixels.show();
    delay(50);
    break;
 }
 
  
  
  color_offset = (color_offset + 1)%COLOR_NUMBER;
 
  t = millis();
  if((t - prevTime) > 8000) {      // Every X seconds...
    mode++;                        // Next mode
    if(mode > 4) {                 // End of modes?
      mode = 0;                    // Start modes over
    }
    color_offset=0;
    for(i=0; i<PIXEL_NUMBER; i++) pixels.setPixelColor(i, 0);
    prevTime = t;
  }
}

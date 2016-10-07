#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define NUMPIXELS 60

int gravity[3] = { 0 };  // 0 == x, 1 == y, 2 == z
const int analog_pin[3] = {A0, A1, A2};

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
}

void loop() {
  readSensor();
  for(int i = 0; i < NUMPIXELS; i++){
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  for(int i = 0; i < 5; i++){
    if(gravity[2] > 30){
      strip.setPixelColor(gravity[0] + i - 2, Wheel((i*10) & 255));
    }else{
      strip.setPixelColor(gravity[0] + i - 2, Wheel(((i*10)+128) & 255));
    }
  }
  strip.show();
}

void readSensor(){
  for(int i = 0; i < 3; i++){
    gravity[i] = analogRead(analog_pin[i]);
    gravity[i] = map(gravity[i], 150, 526, 0, 59);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color((255 - WheelPos * 3) / 3, 0, WheelPos);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos, (255 - WheelPos * 3) / 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos, (255 - WheelPos * 3) / 3, 0);
}

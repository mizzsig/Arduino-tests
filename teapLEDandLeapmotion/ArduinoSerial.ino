#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define NUMPIXELS 60

int gravity[3] = { 0 };  // 0 == x, 1 == y, 2 == z
const int analog_pin[3] = {A0, A1, A2};
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

String str;
int handX = 0;
int flag = 0;

void setup(){
  Serial.begin(115200);
  
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

void loop(){
  readSensor();
  flag = 0;
  while (Serial.available()){
    char c = Serial.read();
    if(c == 'e'){
      handX = str.toInt();
      str = "";
      convertHandX();
      lightLED();
      delay(10);
      break;
    }else{
      str.concat(c);
    }
  }
}

void convertHandX(){
  if(handX > 255){
    handX = 255;
  }else if(handX < -255){
    handX = -255;
  }
  handX = map(handX, -255, 255, 0, 59);
}

void lightLED(){
  for(int i = 0; i < NUMPIXELS; i++){
    if(abs(handX - i) <= 2 && abs(gravity[0] - i) <= 2){
      strip.setPixelColor(i, Wheel(80));
    }else if(abs(handX - i) <= 2){
      strip.setPixelColor(i, Wheel(170));
    }else if(abs(gravity[0] - i) <= 2){
      strip.setPixelColor(i, Wheel(0));
    }else{
      strip.setPixelColor(i, strip.Color(0, 0, 0));
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

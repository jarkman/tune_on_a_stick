#include <Adafruit_NeoPixel.h>

#define LED_STRIP_PIN 7
#define STRIP_LEN 144

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIP_LEN, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

void setupLedstrip() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

long lastT = 0;
int twinkle = 0;
int rgy = 0;

void loopLedstrip(uint32_t c) {
  
#ifdef DEBUG_RANGER
  return;
#endif

  long now = millis();
  if( now - lastT > 10 )
  {
    lastT = now;
  
    if( is_tune_dub_reggae )
    {
      twinkle++;
      if( twinkle > 16 )
      {
        rgy  = (rgy + 1 ) % 3;
        if( rgy == 0 )
          c = colorForRGB( 128, 0, 0 );
        else if( rgy == 1 )
          c = colorForRGB( 0, 128, 0 );
        else if( rgy == 2 )
          c = colorForRGB( 128, 128, 0 );
        
        twinkle = 0;
      }
    }
    
      wipeOut( c );
      strip.show();   

  }
}

int showRangeOnStrip( int range )
{
  uint32_t targetC = 0x0404;
  
  if( range < 0 ) // obstacle
   {
     range = 50;
     targetC = 0x000800;
   }
   
     for(int i=0; i<strip.numPixels(); i++) 
     {
       uint32_t c = 0;
       int n = abs( i - STRIP_LEN/2 );
       int limit = (range * STRIP_LEN) / (2 * max_range);
       
       if( abs( n - limit)  < 3 )
         c = targetC;
         
      strip.setPixelColor(i, c);
    }
    strip.show();
}

void wrainbow(uint8_t wait) {
  uint16_t i, j;

  uint32_t c;

  for(j=0; j<256; j++) {
    
    c = Wheel(j);
    if( j < 16 )
      c = Wheel(127);
      
      wipeOut( c );
      strip.show();   
   //delay(wait);   
     
    }
    
  
}

void wipeOut(uint32_t c) {
  
  int half = strip.numPixels()/2; // halfway along
  
 
   uint32_t prev = strip.getPixelColor(half);

  uint32_t e = prev;
  
  strip.setPixelColor(half,c);
    
  for(uint16_t i=1; i<half; i++) {
    
      prev = strip.getPixelColor(half+i);
      strip.setPixelColor(half-i, e);
      strip.setPixelColor(half+i, e);
      
      e = prev;
      
      
    
      
  }
}
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

uint32_t colorForRGB( int r, int g, int b )
{
  return strip.Color(r,g,b);
}

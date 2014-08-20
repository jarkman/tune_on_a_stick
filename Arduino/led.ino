
// Make sure these pins are all PWM ones! See 
// http://arduino.cc/en/Reference/analogWrite
int redPin = 3;
int greenPin = 5;
int bluePin = 6;

// Current value
int red = 0;
int green = 0;
int blue = 0;

//Step per millisec * 1000 for current fade
long dr = 0;
long dg = 0;
long db = 0;

//Initial value for current fade
long r0 = 0;
long g0 = 0;
long b0 = 0;

// Time we started fade
long fadeStart = 0;

//Time we should end fade
long fadeEnd = 0;

void setupLed() 
{ 

 pinMode(redPin, OUTPUT); 
 pinMode(greenPin, OUTPUT); 
 pinMode(bluePin, OUTPUT); 
 

}

uint32_t loopLed()
{
  long now = millis();
  
  if( fadeEnd == 0 || now > fadeEnd )
  {
    //Serial.println("no fade");
    return colorForRGB( red, green, blue );
  } 
  // Process fade  
  red  = r0 + (dr * (now-fadeStart))/1000l;
  green  = g0 + (dg * (now-fadeStart))/1000l;
  blue  = b0 + (db * (now-fadeStart))/1000l;
  

  writeLed();
  
  return colorForRGB( red, green, blue );
  
 
}

// Start a fade
void rgb( int brightness, int r, int g, int b, long duration )
{
  
   r = (r * brightness) / 256;
   g = (g * brightness) / 256;
   b = (b * brightness) / 256;
  
  r0 = red;
  g0 = green;
  b0 = blue;
  
  dr = ((r - r0) * 1000l) / duration;
  dg = ((g - g0) * 1000l) / duration;
  db = ((b - b0) * 1000l) / duration;
  fadeStart = millis();
  fadeEnd = duration + fadeStart;
 
}

// Set a value immediately
void rgb( int r, int g, int b )
{
  fadeEnd = 0;
  red = r;
  green = g;
  blue = b;
  writeLed();
}

void writeLed()
{
  
  if( red < 0 ) red = 0;
  if( red > 255 ) red = 255;
  if( green < 0 ) green = 0;
  if( green > 255 ) green = 255;
  if( blue < 0 ) blue = 0;
  if( blue > 255 ) blue = 255;
     
  analogWrite( redPin, red );
  analogWrite( greenPin, green );
  analogWrite( bluePin, blue );
}

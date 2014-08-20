
// For the axbotix MB1210 ranger
// See https://www.sparkfun.com/datasheets/Sensors/Proximity/XL-EZ1-Datasheet.pdf
// And http://playground.arduino.cc/Main/MaxSonar

#define ANALOG_RANGE
//#define SERIAL_RANGE  // SERIAL_RANGE works but only when MIDI is turned off - a restriction in SoftwareSerial, I think

#define RANGE_PIN  A0

#ifdef SERIAL_RANGE
SoftwareSerial ranger_serial(11,10,true); // use receive on 11 only, third parameter inverts data
#endif

int range_in_cm = 0;
int val = 0;

void setup_ranger()
{
#ifdef SERIAL_RANGE
    ranger_serial.begin(9600);
#endif
    
}


int read_ranger()
{
#ifdef ANALOG_RANGE
  float volts = (float) analogRead( RANGE_PIN ) * 5.0 / 1023.0;
  
  float current_cm = volts / 0.0049;  // 4.9mv/cm
  
  if( current_cm > 1.0 )
    range_in_cm = current_cm;
  
  #ifdef DEBUG
    //Serial.print(range_in_cm); 
    //Serial.println("cm");
#endif
#endif

#ifdef SERIAL_RANGE

  int av = ranger_serial.available();
  //Serial.println(av); 
  for( int i = 0; i < av; i ++ )
  {
    char c = ranger_serial.read();
    //Serial.println(c); 
    if( c == 'R' )
      val = 0;
    else if( isdigit( c ))
      val = val * 10 + (c - '0');
    else if( c == 13 )
    {
      range_in_cm = val;
      #ifdef DEBUG
        Serial.print(range_in_cm); 
        Serial.println("cm");
      #endif
    }
  }    
#endif
  

    return range_in_cm; // take the closest valid range during the period of this beat
}


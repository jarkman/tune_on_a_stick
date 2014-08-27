
// This connects to an A3144 Hall effect sensor, which reads an index pulse once per revolution
// Data sheet: http://www.allegromicro.com/~/media/Files/Datasheets/A3141-2-3-4-Datasheet.ashx

#define INDEX_PIN 4

boolean sensorOn = false;

long indexStartTime = 0;

int setupSpeed()
{
#ifdef SIMULATE_ROTATION
  millis_per_sweep = 3000;
#endif 

   pinMode( INDEX_PIN, INPUT_PULLUP );
   digitalWrite( INDEX_PIN, HIGH); // pullup
     
   sweep_start_time = millis();

}


int loopSpeed()
{
  
#ifdef SIMULATE_ROTATION
 long now = millis();
 long interval = now - sweep_start_time;
 if( interval > millis_per_sweep )
       sweep_start_time = now;
  
#else  

  boolean b = digitalRead( INDEX_PIN );
/*
if( b )
Serial.println("on");
else
Serial.println("off");
*/
  if( sensorOn != b )
  {
    sensorOn = b;
    if( sensorOn )
    {
       long now = millis();
       long interval = now - indexStartTime;
       
       if( interval < 1000 )
         interval = 1000;
         
#ifdef DEBUG

    Serial.print("millis_per_sweep now: ");
    Serial.print(interval); 
    Serial.println(" mS");
#endif

      millis_per_sweep = interval;
      sweep_start_time = now;

       indexStartTime = now;
    }
  }  
  
  #endif
}


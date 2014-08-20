
#include <Servo.h> 

//#define DEBUG

#define STATE_IDLE 0
#define STATE_PLAYING 1

#define MIDI_BACKING_VOLUME 74


int state = STATE_IDLE;

Servo myservo; 
int pos = 0;
long start_time = 0;
long millis_per_sweep = 3000;
int beats_per_bar = 4;
int bars_per_sweep = 8;
int beats_per_sweep;
int last_beat = 0;
long beat_duration;
long fade_duration;
long beat_start_time = 0;

int num_no_people = 0;
int num_tune_bars = 0;


#define LINES 2
#define LEAD_LINE 0
#define BACKING_LINE 1

#define MAX_DISTANCE 200 // cm



int this_beat_range = 0;


void setup() {
  
  #ifdef DEBUG
  while( ! Serial)
    ;
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  #endif
   
  setup_ranger();
  
  myservo.attach(8); 
  
  setup_scale();
  setup_midi();
  setupLed();
  setupTune();
  setupLedstrip();
  
  start_time = millis();
 
}

void loop() 
{

  uint32_t color = loopLed();

  loopLedstrip( color );
  
  pos = (((millis() - start_time) * 180L) / millis_per_sweep) % 180;
   myservo.write(pos); 
 
   this_beat_range = read_ranger();

  int beat = (pos * beats_per_sweep ) / 180;
  if( beat != last_beat ) // Time to start a note!
  {

   doBeat( beat );
   
    this_beat_range = 0;
    
  }

}

void doBeat( int beat )
{
  
#ifdef DEBUG
/*
    Serial.print("Beat range: ");
    Serial.print(this_beat_range); // Convert ping time to distance in cm and print result (0 = outside set distance range)
    Serial.println("cm");
    */
#endif

    last_beat = beat;

    beat_start_time = millis();

   
       
    if( beat % beats_per_bar == 0 ) // Start of bar
    {
      num_tune_bars ++;
      midi_boom();
      doBackingVolume();
    }
    else
       midi_tish(); // Start of note, not at start of bar
    
 
    boolean got_range = false;
    
    if( goodRange( this_beat_range )) // min range to screen out junk on desk
    {
      if( state != STATE_PLAYING )
        startPlaying();
        
      int note = noteForRange( this_beat_range, 0 );
      //Serial.println(note);
      
      setAndPlayNote( LEAD_LINE, beat, note );
      
      got_range = true;
      
      //rgb( note, note, note , fade_duration ); 
    }
    else
    {
      //setNote( LEAD_LINE, beat, 0 );
    }
    
    generateBacking( beat );
    

   int brightness = 256;

   if(beat % 2) // alternate notes are dimmer
      brightness = brightness / 8; // alternate notes are dimmer
 
   if( state == STATE_IDLE )
   {
       brightness -= num_no_people;
      if( brightness < 32)
        brightness = 32;
        
     if( beat % beats_per_bar != 0 ) // make the first beat stand out
       brightness = brightness / 4;
     
    
   }
   
    if( got_range )
    {
  
       // scale range to 0->256
       int wheelPos = (this_beat_range * 256) / MAX_DISTANCE;
       
       uint32_t rgbc = Wheel(wheelPos);
       int r = R( rgbc );
       int g = G( rgbc );
       int b = B( rgbc );
       
  
      if( beat % 2 == 0 ) // alternate coloyrs
         rgb( brightness, r,g,b , fade_duration ); 
      else
         rgb( brightness, r,g,b , fade_duration ); 
   
      num_no_people = 0;
 
      
    }
    else
    {
      if( state == STATE_IDLE )
        if( beat % 2 == 0 ) // alternate coloyrs
          rgb( brightness, 0,0,256 , fade_duration ); 
        else
          rgb( brightness, 0,0,128 , fade_duration ); 
      else
        if( beat % 2 == 0 ) // alternate coloyrs
          rgb( brightness, 64,256,64 , fade_duration ); 
        else
           rgb( brightness, 64,64,256 , fade_duration ); 
         
      num_no_people ++;
      
      if( num_no_people >= 2 * beats_per_sweep  )
        startIdle();
    }
}

int R( uint32_t rgb )
{
  return rgb >> 16;
}

int G( uint32_t rgb )
{
  return (rgb >> 8 ) & 0x0ff;
}

int B( uint32_t rgb )
{
  return (rgb  ) & 0x0ff;
}


void startPlaying()
{
  if( state == STATE_PLAYING )
    return;
    
  newTune();
  state = STATE_PLAYING;  
  num_tune_bars = 0;
}

void startIdle()
{
  if( state == STATE_IDLE )
    return;
    
  state = STATE_IDLE;
  num_tune_bars = 0;
}

boolean goodRange( int range )
{
  return range > 50 && range < MAX_DISTANCE;
}

void newTune()
{

  midiAllNotesOff(0);
  midiAllNotesOff(1);
  clearTune();
  pickRandomInstruments();
  pickRandomScale();
  
}

void doBackingVolume()
{
  
    int volume;
    int fade_bars = 8;
    
    if( state == STATE_IDLE )
    {
      volume = MIDI_BACKING_VOLUME >> num_tune_bars; // fade out
    }
    else
    {
      int shift = 8 - num_tune_bars;
      if( shift < 0 )
        shift = 0;
     volume =  MIDI_BACKING_VOLUME >> shift; // fade in
    }
    
#ifdef DEBUG

    Serial.print("backing volume: ");
    Serial.print(volume); // Convert ping time to distance in cm and print result (0 = outside set distance range)
    Serial.println("");
    
#endif
    midiSetChannelVolume(1,volume);
}


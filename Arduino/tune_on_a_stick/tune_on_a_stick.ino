
#include <Servo.h> 

#define DEBUG

//#define SIMULATOR  // simulate range data
//#define SIMULATE_ROTATION
#define DEBUG_RANGER // use led strip as range bargraph

//#define USE_SERVO // Servo used to simulate rotation before the real rotating rig was available
//#define SPARKFUN_SHIELD

#define DO_BACKING




#ifdef DEBUG_RANGER
#define SIMULATE_ROTATION
#undef DO_BACKING
#endif

#define CHANNELS 1
#define LEAD_CHANNEL 0


// General MIDI assigns drums to channel 10 by default (note: zero-based)
#define PERCUSSION_CHANNEL 9


#define MAX_BEATS_PER_BAR 6
#define MAX_BARS_PER_SWEEP 3


#define TUNE_LIST_SIZE MAX_BEATS_PER_BAR * MAX_BARS_PER_SWEEP
#define LINES 3
#define LEAD_LINE 0
#define BACKING_LINE 1
#define HARMONY_LINE 2


#define MIN_DISTANCE 50 // cm

#define IDLE_SWEEPS 2 // if we see no people for this long, go idle

#define STATE_IDLE 0
#define STATE_PLAYING 1


#define MIDI_BACKING_VOLUME 75
#define MIDI_HARMONY_VOLUME 100
#define MIDI_PERCUSSION_VOLUME 127

#define BACKING_FADE_BARS 4 // fade backing in or out over this many bars
#define PERCUSSION_FADE_BARS 100
#define LED_FADE_BARS 100

int state = STATE_IDLE;

#ifdef USE_SERVO
#define SERVO_PIN 8
Servo myservo; 
#endif



long angle = 0;
long sweep_start_time = 0;
long millis_per_sweep = 3000; // overwritten by loopSpeed
int beats_per_bar = 4;
int bars_per_sweep = 8;
int beats_per_sweep;
int last_beat = 0;
long beat_duration;
long fade_duration;
long beat_start_time = 0;

int num_no_people = 0;
int num_tune_bars = 0;

long num_total_beats = 0;

int this_beat_range = 0;
int max_range = 200;  // overwritten by max range from pot in read_ranger

void setup() {
  
  #ifdef DEBUG
  while( ! Serial)
    ;
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  #endif
   
  setup_ranger();
  
#ifdef USE_SERVO
  myservo.attach(SERVO_PIN); 
#endif
  
  setupMotor();  
  setupSpeed();

  setup_scale();
  setup_midi();
  setupLed();
  setupTune();
  setupLedstrip();
  setupSimulator();
  setupObstacles();

 setMotorPercent( 255 );
}

void loop() 
{
 loopSpeed();
 
  uint32_t color = loopLed();

  loopLedstrip( color );
  
 #ifdef USE_SERVO
   myservo.write((int) angle); 
 #endif
 
  this_beat_range = read_ranger();

  int beat = getBeat();
  
  if( isObstacle( this_beat_range, angle ))
    this_beat_range = -1; // ignore the obstacle
    
  
  
  if( beat != last_beat ) // Time to start a note!
  {

 #ifdef SIMULATOR
   this_beat_range = simulatorRange( beat );
 #endif
 
    doBeat( beat );
   
    this_beat_range = 0;
    
  }

}

int getBeat()
{
  angle = (((millis() - sweep_start_time) * 360L) / millis_per_sweep) % 360L;
  int beat =  (angle * beats_per_sweep ) / 360;
  return beat;
}

void doBeat( int beat )
{
  
#ifdef DEBUG

    Serial.print("Beat range: ");
    Serial.print(this_beat_range); // Convert ping time to distance in cm and print result (0 = outside set distance range)
    Serial.println("cm");
    
#endif

    num_total_beats++;

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
    
 #ifdef DEBUG_RANGER
      showRangeOnStrip( this_beat_range );
#endif


    boolean got_range = false;
    
    if( goodRange( this_beat_range )) // min range to screen out junk on desk
    {
      if( state != STATE_PLAYING ) {
        startPlaying();
        // Refresh the beat calculation, because it may have been changed by the new tune parameters
        beat = getBeat();
        last_beat = beat;
      }
        

      int note = noteForRange( this_beat_range, LEAD_CHANNEL );
      
      
      setAndPlayNote( LEAD_LINE, beat, note );
      
      got_range = true;
      
      //rgb( note, note, note , fade_duration ); 
    }
    else
    {
      setAndPlayNote( LEAD_LINE, beat, 0 ); // terminate the last note
    }
    
#ifdef DO_BACKING    
    generateBacking( beat );
#endif

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
       int wheelPos = (this_beat_range * 256) / max_range;
       
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
      {
        // fade LED action out over many bars
        int bars = num_tune_bars;

        // put a floor on fade        
        if( bars > LED_FADE_BARS - 16 )
          bars = LED_FADE_BARS - 16;
        
         brightness = (brightness * (LED_FADE_BARS - bars)) / LED_FADE_BARS; 

        if( beat % 2 == 0 ) // alternate coloyrs
          rgb( brightness, 0,0,256 , fade_duration ); 
        else
          rgb( brightness, 0,0,128 , fade_duration ); 
      }
      else
         if( this_beat_range == -1 )
            rgb( brightness, 128,0,0 , fade_duration ); // obstacles are green
         else 
            if( beat % 2 == 0 ) // alternate coloyrs
                rgb( brightness, 64,256,64 , fade_duration ); 
            else
               rgb( brightness, 64,64,256 , fade_duration ); 
         
      num_no_people ++;
      
      if( num_no_people >= IDLE_SWEEPS * beats_per_sweep  )
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
  return range > MIN_DISTANCE && range < max_range;
}

void newTune()
{
  midiAllNotesOff(0);
  midiAllNotesOff(1);
  midiAllNotesOff(2);
  clearTune();
  pickRandomInstruments();
  pickRandomScale();
  
  #ifdef DEBUG

    Serial.println("");
    Serial.print("newTune - ");
    Serial.print(beats_per_bar); 
    Serial.println(" beats per bar");
    
    Serial.print( 60000 / beat_duration );
    Serial.println(" bpm");
    Serial.println("");
    
      Serial.print( num_total_beats );
    Serial.println(" total beats so far");
    Serial.println("");
    
 
    
#endif
}

void doBackingVolume()
{
  
    int volume;
    int percussionVolume;
    
    int fade_bars = 8;
    
    if( state == STATE_IDLE )
    {
      if( num_tune_bars < BACKING_FADE_BARS )
        volume = (MIDI_BACKING_VOLUME * (BACKING_FADE_BARS - num_tune_bars)) / BACKING_FADE_BARS; // fade out
      else
        volume = 0;
        
      if( num_tune_bars < PERCUSSION_FADE_BARS )
        percussionVolume = (MIDI_PERCUSSION_VOLUME * (PERCUSSION_FADE_BARS - num_tune_bars)) / PERCUSSION_FADE_BARS; // fade out
      else
        percussionVolume = 0;
    }
    else
    {
      if( num_tune_bars < BACKING_FADE_BARS )
        volume = (MIDI_BACKING_VOLUME * num_tune_bars) / BACKING_FADE_BARS; // fade in
      else
        volume = MIDI_BACKING_VOLUME;
        
        
      percussionVolume = MIDI_PERCUSSION_VOLUME;
    }
    
#ifdef DEBUG
/*
    Serial.print("backing volume: ");
    Serial.print(volume); // Convert ping time to distance in cm and print result (0 = outside set distance range)
    Serial.println("");
*/    
#endif
    midiSetChannelVolume(1,volume);
    midiSetChannelVolume(PERCUSSION_CHANNEL,percussionVolume);
}



#define NUM_ACTIVES 12
#define NUM_SCALES 1


unsigned short int scaleMajor[] = { 0, 2, 4, 5, 7, 9, 11,  255 };
unsigned short int scaleMinor[] = { 0, 2, 3, 5, 7, 8, 10,  255 };
unsigned short int scaleFudgePentatonic[] = { 0, 0, 2, 4, 7, 7, 9,  255 }; // fudge the pentatonic scale to have 8 notes (repeat the root and fifth degree)
unsigned short int *scales[] = { scaleMajor, scaleMinor, scaleFudgePentatonic, NULL };

/* phill temporary - omit these more exotic scales for now
unsigned short int *scales[] = { scaleMajor, scaleMinor, scaleAlteredMinor, scalePentatonic, scalePentatonicBlues, NULL };
unsigned short int scaleAlteredMinor[] = { 0, 2, 3, 6, 7, 8, 11,  255 };
unsigned short int scalePentatonicBlues[] = { 0, 3, 5, 6, 7, 10,  255 };
unsigned short int scalePentatonic[] = { 0, 2, 4, 7, 9,  255 };
unsigned short int scaleFudgePentatonic[] = { 0, 0, 2, 4, 7, 7, 9,  255 }; // fudge the pentatonic scale to have 8 notes (repeat the root and fifth degree)
unsigned short int scaleFudgePentatonicBlues[] = { 0, 3, 5, 6, 6, 7, 10, 10,  255 }; // fudge the pentatonic scale to have 8 notes (repeat the "blue" notes)
*/

unsigned short int activeNotes[CHANNELS][ NUM_ACTIVES ];
int numActives[CHANNELS];

void setup_scale()
{
    pickRandomScale();
}

void pickRandomScale()
{
  setupBars();
  
    int scale = pickScaleFromPosition();
  
  // rootNote (lowest note in the list of allowed active notes) varies, but only for large movements 
  int rootNote = 50 + random(30); 
  
#ifdef DEBUG

    Serial.println("");
    Serial.print("Using scale number ");
    Serial.print(scale); 
    
    Serial.print( " with root note number " );
    Serial.println(rootNote);
    
#endif
  // phill temporary - this choses a list of allowed notes (actives)
  // favouring the root, fourth, and fifth degree of the scale
  // it DOES also pick other notes but the gps latWhiskers in the simulateGPS() tends
  // to result in mostly roots and fifths (which is no bad thing if we're aiming to imitatate western melodies)
  for( int channel = 0; channel < CHANNELS; channel ++ )
  {
    int a  = 0;
    int s = 0;
    int root_fourth_fifth = 0; // 0 = root, 3 = fourth, 4 = fifth
    
    for( int i = 0; a < NUM_ACTIVES; i ++ )
    {
    
      // favour root and fifth notes
      if( random( 2 ) )
      { // if the latitude bits are set - alternate between placing a root or fifth note
        activeNotes[channel][a] = scales[scale][root_fourth_fifth] + rootNote;
        
        // alternate between picking a root or fifth
        if (root_fourth_fifth == 0) 
          root_fourth_fifth = 4;
        else 
          root_fourth_fifth = 0;

        a++;
      }
      else
      { // otherwise ascend through the scale as normal
        activeNotes[channel][a] = scales[scale][s] + rootNote;
        a++;
      }
       
       s++;
       if(  scales[scale][s] > 250 )
       {
         s = 0;
         rootNote += 12;
       }
    
    }
    
    numActives[channel] = a;
    
    for( ; a < NUM_ACTIVES; a ++ )
      activeNotes[channel][a] = 0;


  #ifdef DEBUG
    Serial.print ("Channel ");
    Serial.print (channel, DEC);
    Serial.print (" active notes: ");
    for( int i = 0; i < NUM_ACTIVES; i ++ )
    {
      Serial.print (activeNotes[channel][i], DEC);
      Serial.print (" ");
    }
    Serial.println (" ");
  #endif
  }// channel loop
  
}

void setupBars()
{
  //millis_per_sweep = 1500 + random(1500);
  
  beats_per_bar = 4 + random( 3 );
  bars_per_sweep = 10 / beats_per_bar; // 2 or 1
  beats_per_sweep = beats_per_bar * bars_per_sweep;
  beat_duration = millis_per_sweep / beats_per_sweep;
  fade_duration = beat_duration/2;
}

int pickScaleFromPosition()
{
   int s = random( NUM_SCALES );
   return s;
}

int noteForRange( int cm, int channel )
{
  int index = ((MAX_DISTANCE - cm) * numActives[channel]) / MAX_DISTANCE; 
  return activeNotes[channel][index];
}


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

  for( int channel = 0; channel < CHANNELS; channel ++ )
  {
    
    // Generate a scale based on the chosen root node and mode
    for (int i = 0; i < NUM_ACTIVES; ++i) 
    {
      activeNotes[channel][i] = rootNote + scales[scale][i % 7] + 12 * (i / 7);
    }
    
    numActives[channel] = NUM_ACTIVES;

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
  
  beats_per_bar = 4 + random( 2 );
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
  int index = ((max_range - cm) * numActives[channel]) / max_range; 
  return activeNotes[channel][index];
}

// Find an active note offset from the given note, useful for generating harmonies
int noteAtOffset( int note, int channel, int offset ) {
  
  for (int i = 0; i < numActives[channel]; ++i)
  {
    if (activeNotes[channel][i] == note)
      return activeNotes[channel][(i + offset) % numActives[channel]];     
  }
  
  return 0; // not expected if the input note is valid
}

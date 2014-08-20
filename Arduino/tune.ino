#define MAX_BEATS_PER_BAR 6
#define MAX_BARS_PER_SWEEP 3


#define TUNE_LIST_SIZE MAX_BEATS_PER_BAR * MAX_BARS_PER_SWEEP

unsigned short int midiNotes[TUNE_LIST_SIZE][LINES];


void setupTune()
{
  clearTune();
}

void clearTune()
{
  for( int line = 0; line < LINES; line ++ )
    for( int beat = 0; beat < TUNE_LIST_SIZE; beat ++ )
    {
      midi_note_off( line, midiNotes[line][beat]);
      midiNotes[line][beat] = 0;
    }
}

void setAndPlayNote( int line, int beat, int note )
{
  setNote( line, beat, note );
  midi_note( line, note );
  int offBeat = (beat + beats_per_sweep - 2) % beats_per_sweep;
  
  midi_note_off( line, getNote(line, offBeat));
}

void setNote( int line, int beat, int note )
{
  midiNotes[line][beat] = note;  
  
}

int getNote( int line, int beat )
{
  return midiNotes[line][beat];  
}
    
    
int generateBacking( int beat )
{
  int i = beat;
  do
  {
    int bb = (beat + 2) % beats_per_sweep; // pick a lead line note from 2 notes into the future
    int note = getNote( LEAD_LINE, bb );
    note -= 24;
    
    if( i == beat )
      setAndPlayNote( BACKING_LINE, beat, note );
    else
      setNote( BACKING_LINE, beat, note );
      
    i++;
    i = i % beats_per_sweep;
  }
  while( i != beat );
}


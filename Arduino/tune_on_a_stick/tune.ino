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
    
    
// This is called on every beat to generate backing    
// It should do whatever note generation and playing it deems appropriate
// It can use the midiNotes array for storage via setNote( BACKING_LINE, beat, note ), if it is
// convenient to do generation (say) bar-by-bar instead of note-by-note

int generateBacking( int beat )
{
  int i = beat;
  do
  {
    int bb = (beat + 2) % beats_per_sweep; // pick a lead line note from 2 notes into the future
    int note = getNote( LEAD_LINE, bb );    // notes are midi note numbers
    note -= 24;      // shift it down a couple of octaves
    
    if( i == beat )
      setAndPlayNote( BACKING_LINE, beat, note );  // play the note that's due now
    else
      setNote( BACKING_LINE, beat, note );        // store up notes for the future (these are currently not used, as we make and play a new note every time we are called
      
    i++;
    i = i % beats_per_sweep;
  }
  while( i != beat );
}


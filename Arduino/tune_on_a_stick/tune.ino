

unsigned short int midiNotes[LINES][TUNE_LIST_SIZE];


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

  // Never set a note without playing it, otherwise we can't send the note-off message properly
  //setNote( line, beat, note );
  midiNotes[line][beat] = note;
  midi_note( line, note );
  

  // turn off the last note for this channel
  int offBeat = (beat + beats_per_sweep - 1) % beats_per_sweep;
  
  midi_note_off( line, getNote(line, offBeat));

}

/*
void setNote( int line, int beat, int note )
{
  midiNotes[line][beat] = note;  
  
}
*/
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
  
  int bb = (beat + 2) % beats_per_sweep; // pick a lead line note from 2 notes into the future
  int note = getNote( LEAD_LINE, bb );    // notes are midi note numbers
  
   // If we were picking notes in some other way, we might want to get them from 
   // activeNotes[LEAD_CHANNEL] which is the array of note numbers that the kead line is going to use  
   // It is numActives[LEAD_CHANNEL] long.


  note -= 24;      // shift it down a couple of octaves
    
   setAndPlayNote( BACKING_LINE, beat, note );  
      
  /*    
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
  */
}


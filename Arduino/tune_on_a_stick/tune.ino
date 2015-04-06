

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

    // turn off the last note for this channel
  int offBeat = (beat + beats_per_sweep - 1) % beats_per_sweep;
  
  midi_note_off( line, getNote(line, offBeat));
  
  
  // Never set a note without playing it, otherwise we can't send the note-off message properly
  //setNote( line, beat, note );
  midiNotes[line][beat] = note;
  midi_note( line, note );
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
    // do the "one drop" dub reggae thing for tunes with beats_per_bar == 4 or 8 
    if ( is_tune_dub_reggae && (beats_per_bar == 4 || beats_per_bar == 8) )
    { 
      // guitar "chip" sounds on the "off" beats -- skank!
      int skank_beat = ( beats_per_bar / 2);
      if ( beat % skank_beat == (skank_beat/2) )
      {
        // the method for handling notes is a little kludgy here
        // send a note off before sending a note on
        // cunning - like a FOX I tell you! Well, it, er... works I guess.
        
        int i=3;
        int chord[3] = { // root, plus some other notes (skip note nearest root)
            activeNotes[0][0], 
            activeNotes[0][2],
            activeNotes[0][3]
        };        
        while(i--) {
          midiNoteOff(11, chord[i], 0);
          midiNoteOn(11, chord[i], 100);
        }
        
      }
      
      // do the "one drop" beat
      // omit the boom from the the start of the bar ("drop" the "one") and
      // accentuate the middle of the bar with a snare rimshot
      if ( (beats_per_bar / (beat % beats_per_bar) ) == 2 ) // boom and snare rimshot at the middle of the bar
      {
         midi_boom();  // kick drum
         midi_crack(); // rimshot
      }
      
      // hihats on every beat to keep the pulse
      midi_tish();
    }
    else
    {
      // phill moved this if-else from doBeat() in tune_on_a_stick.cpp
      if ( beat % beats_per_bar == 0 ) // start of bar
          midi_boom();  // kick drum
      else
          midi_tish(); 
      
      ////////
      
      int bb = (beat + 2) % beats_per_sweep; // pick a lead line note from 2 notes into the future
      int note = getNote( LEAD_LINE, bb );    // notes are midi note numbers
      
       // If we were picking notes in some other way, we might want to get them from 
       // activeNotes[LEAD_CHANNEL] which is the array of note numbers that the kead line is going to use  
       // It is numActives[LEAD_CHANNEL] long.
    
      if (note > 0) {
        //note -= 24;      // shift it down a couple of octaves
        note -= 12;      // shift it down a couple of octaves
      } 
      else {
        // if no lead note, 50% chance to leave backing as it was last time around
        note = random(2) * getNote( BACKING_LINE, beat);
      } 
      
      setAndPlayNote( BACKING_LINE, beat, note); 
      
      // Now the harmony...
      note = getNote( LEAD_LINE, beat);
      
      if (note > 0) {
         note = (random(3) > 0) ? noteAtOffset(note, LEAD_CHANNEL, random(3) + 2) : 0; // third, fourth or fifth?
         if (note > 0) note -= 12;  // drop an octave
      }
      
      if (note == 0) // chance to repeat previous note
         note = (random(3) > 0) ? getNote( HARMONY_LINE, (beat + beats_per_sweep - 1) % beats_per_sweep) : 0;
         
      setAndPlayNote( HARMONY_LINE, beat, note);
    }
}


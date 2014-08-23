
int rangeForBeat[TUNE_LIST_SIZE];
int totalSweeps = 0;


int setupSimulator()
{
  for( int i = 0; i < TUNE_LIST_SIZE; i++)
    rangeForBeat[i] = 0;
  
}


int simulatorRange( int beat ) // return a simulated range for this beat
{
  
  if( beat == 0 )
    totalSweeps ++;
  
  if( totalSweeps >= 20 ) // Every 20 sweeps, go idle for 2 sweeps to put us intp STATE_IDLE and prompt a scale change
    totalSweeps = 0;
    
  if( totalSweeps <= IDLE_SWEEPS )
   {
#ifdef DEBUG
    Serial.print("Simulating idle, sweep ");
    Serial.println( totalSweeps );
#endif
    return 0;
  }    
  
  
    

  // now update our array
  
  if( random( 5 ) == 0 ) // a 1 in 5 chance of changing any note whenever we think about it, so the pattern tends to persist, simulating a slow-moving set of people
    if( random( 2 ) == 0 ) // notes will often become silent
      rangeForBeat[beat] = 0; // no person detected
    else
      rangeForBeat[beat] = MIN_DISTANCE + random( MAX_DISTANCE - MIN_DISTANCE ); // pick a random valid range for this person
      
#ifdef DEBUG

    Serial.print("Simulated range: ");
    Serial.println(rangeForBeat[beat]);    
#endif
    
  return rangeForBeat[beat];
}


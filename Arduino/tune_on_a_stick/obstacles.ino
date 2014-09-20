
// Keep track of the fixed objects in the room so we can ignore them

#define NUM_OBSTACLES (MAX_BEATS_PER_BAR * MAX_BARS_PER_SWEEP * 2)   // at least twice the usual number of beats
#define MAX_OBSTACLE_RANGE 1024 // more than any legal range
#define RANGE_MARGIN 20 // cm

int obstacleRange[NUM_OBSTACLES];
int obstacleCount[NUM_OBSTACLES];

int lastO = -1;

#define ROLLING_FRACTION 100L

int setupObstacles()
{
  for( int o = 0; o < NUM_OBSTACLES; o ++ )
  {
    obstacleRange[o] = 300;
    obstacleCount[o] = 0;
  }
}

int isObstacle( int range, long angle )
{
  
  
  //return false; // currently not reliable enough to use - too much sensor jitter

  
  int result = false;
  
  if( range < MIN_DISTANCE) // no object or very close object
    return false;
  
  int o = (angle * NUM_OBSTACLES)/360;
  
  long current = obstacleRange[o];
 
  if( abs( range - current ) < RANGE_MARGIN ) 
      result = true;
      
  if( o != lastO )
  {
    // new slot - only want to average one per slot
    
      
    
    // rolling average
    current = (current * (1000L - ROLLING_FRACTION) + (long) range * ROLLING_FRACTION) / 1000L;
    
    obstacleRange[o] = current;
    
    lastO = o;
  }
  
  

  return result;
}


/*

  int current = obstacleRange[o];
  
  if( abs( range - current ) < RANGE_MARGIN ) // matches existing max
  {
    if( o != lastO )
    {
      obstacleCount[o]++;
    #ifdef DEBUG
     Serial.print("Increased count ");
    Serial.print(obstacleCount[o]);
    Serial.print(" range ");
    Serial.print(range); 
    Serial.println(" cm");
    #endif
    }
    
    if( obstacleCount[o] > 12 ) // ignore it if we've seen it lots of times in a row
    {
#ifdef DEBUG
    if( o != lastO )
    {
    Serial.print("Rejecting obstacle index ");
    Serial.print(o);
    Serial.print(" count ");
    Serial.print(obstacleCount[o]);
    Serial.print(" range ");
    Serial.print(range); 
    Serial.println(" cm");
    }
#endif
      result = true;
    }
  }
  else if( range > current ) // new max, record it
  {
    if( o != lastO )
    {
     obstacleRange[o] = range;
     obstacleCount[o] = 0;
    }
  }
  
  lastO = o;

  return result;
}
*/


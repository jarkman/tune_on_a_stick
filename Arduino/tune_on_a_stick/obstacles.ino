
// Keep track of the fixed objects in the room so we can ignore them

#define NUM_OBSTACLES (MAX_BEATS_PER_BAR * MAX_BARS_PER_SWEEP * 2)   // at least twice the usual number of beats
#define MAX_OBSTACLE_RANGE 1024 // more than any legal range
#define RANGE_MARGIN 20 // cm

int obstacleRange[NUM_OBSTACLES];
int obstacleCount[NUM_OBSTACLES];

int lastO = -1;

int setupObstacles()
{
  for( int o = 0; o < NUM_OBSTACLES; o ++ )
  {
    obstacleRange[o] = 0;
    obstacleCount[o] = 0;
  }
}

int isObstacle( int range, long angle )
{
  int result = false;
  
#ifdef DEBUG_RANGER
  return false;
#endif
  
  if( ! goodRange( range ))
    return false;
  
  
  int o = (angle * NUM_OBSTACLES)/360;
  

  int current = obstacleRange[o];
  
  if( abs( range - current ) < RANGE_MARGIN ) // matches existing max
  {
    if( o != lastO )
      obstacleCount[o]++;
    
    
    if( obstacleCount[o] > 4 ) // ignore it if we've seen it lots of times in a row
    {
#ifdef DEBUG

    Serial.print("Rejecting obstacle index ");
    Serial.print(o);
    Serial.print(" count ");
    Serial.print(obstacleCount[o]);
    Serial.print(" range ");
    Serial.print(range); 
    Serial.println(" cm");
    
#endif
      result = true;
    }
  }
  else if( range > current ) // new max, record it
  {
     obstacleRange[o] = range;
     obstacleCount[o] = 0;
  }
  
  lastO = o;

  return result;
}


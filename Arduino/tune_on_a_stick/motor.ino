
#define MOTOR_PIN 11

int setupMotor()
{
   pinMode(MOTOR_PIN, OUTPUT); 

}


int setMotorPercent( int power )
{
    analogWrite( MOTOR_PIN, power );

}


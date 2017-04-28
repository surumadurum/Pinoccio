#include <Servo.h>

Servo servo_mouth_cat;
Servo servo_mouth_fox;
Servo servo_head_turn_cat;
Servo servo_head_turn_fox;

#define RECV_STEERING 22
#define RECV_SPEED 24

//LEFT MOTOR SPEED CONTROL
#define SPEED_CONTROL_1 2

//RIGHT MOTOR SPEED CONTROL
#define SPEED_CONTROL_2 3

//LEFT MOTOR DIRECTION CONTROL
#define DIRECTION_RELAY_11  49 
#define DIRECTION_RELAY_12  51

//RIGHT MOTOR DIRECTION CONTROL
#define DIRECTION_RELAY_21  47
#define DIRECTION_RELAY_22  45

/////MOUTH MOVEMENTS
//Both mouth-movements are controlled by one slider
#define MOUTH_IN 28
#define MOUTH_SYNC 26  //Switch to assign both mouth-movements to left-signal

#define RECV_HEAD_TURN_CAT 30
#define RECV_HEAD_TURN_FOX 32

#define SERVO_HEAD_TURN_CAT 8
#define SERVO_HEAD_TURN_FOX 11

#define SERVO_MOUTH_CAT 9
#define SERVO_MOUTH_FOX 12

#define ZERO_LIMIT_POS 20


//clone of map function for floats
float map_float(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//function to zero out middle range
void ConstrainZeroPosition(int* value, int range)
{
  if((*value>0) && (*value<range))
    *value=0;
  
  if((*value<0) && (*value>-range))
    *value=0;
}


void runForward(int motor)
{  
  switch(motor)
  {
    case 1:
      digitalWrite(DIRECTION_RELAY_11,HIGH);
      digitalWrite(DIRECTION_RELAY_12,LOW);
      break;
    
    case 2:
      digitalWrite(DIRECTION_RELAY_21,HIGH);
      digitalWrite(DIRECTION_RELAY_22,LOW);  
      break;
  }
}

void runBackward(int motor)
{
  switch (motor)
  {
    case 1:
      digitalWrite(DIRECTION_RELAY_11,LOW);
      digitalWrite(DIRECTION_RELAY_12,HIGH);
      break;
    
    case 2:
      digitalWrite(DIRECTION_RELAY_21,LOW);
      digitalWrite(DIRECTION_RELAY_22,HIGH);  
      break;
  }
}


void setup()
{
 // Serial.begin(115200);
  
  pinMode(RECV_STEERING,INPUT);  //Servo Signal v. Empfaenger
  pinMode(RECV_SPEED,INPUT);  //Servo Signal v. Empfaenger
  pinMode(MOUTH_IN,INPUT);
  pinMode(RECV_HEAD_TURN_CAT,INPUT);
  pinMode(RECV_HEAD_TURN_FOX,INPUT);
  pinMode(MOUTH_SYNC,INPUT);
 
  pinMode(SERVO_MOUTH_CAT,OUTPUT);
  pinMode(SERVO_MOUTH_FOX,OUTPUT);

  pinMode(SERVO_HEAD_TURN_FOX,OUTPUT);
  pinMode(SERVO_HEAD_TURN_CAT,OUTPUT);

  pinMode(SPEED_CONTROL_1,OUTPUT);
  pinMode(SPEED_CONTROL_2,OUTPUT);
  pinMode(DIRECTION_RELAY_11,OUTPUT);
  pinMode(DIRECTION_RELAY_12,OUTPUT);
  pinMode(DIRECTION_RELAY_22,OUTPUT);
  pinMode(DIRECTION_RELAY_21,OUTPUT);
  
  servo_mouth_cat.attach(SERVO_MOUTH_CAT);
  servo_mouth_fox.attach(SERVO_MOUTH_FOX);
  
  servo_head_turn_cat.attach(SERVO_HEAD_TURN_CAT);
  servo_head_turn_fox.attach(SERVO_HEAD_TURN_FOX);
  
  runForward(1);
  runForward(2);
}

void loop()
{
    
  
  
     int mouth_in =  map(pulseIn(MOUTH_IN,HIGH),1000,2000,-180,180);

      //Kill range around zero pos.
  //   ConstrainZeroPosition(&mouth_in,7);     
      
    //should both mouthes do the same movement
     int mouth_sync_sw = map(pulseIn(MOUTH_SYNC,HIGH),800,1600,0,1);


     //left mouth is negative part of signal
     int mouth_left = constrain(mouth_in,-180,-ZERO_LIMIT_POS); //strip off "valid part of signal"
    //use map to a)map the zero range in the middle of joystick to full servo range and b)invert the negative signal
    mouth_left = map(mouth_left,-180,-ZERO_LIMIT_POS,180,0);

    //mouth right is positive part of signal
    int mouth_right;
     if(mouth_sync_sw==0)
        mouth_right = map(constrain(mouth_in,ZERO_LIMIT_POS,180),ZERO_LIMIT_POS,180,0,180);   //as above, just with already positive part
     else // or equal to mouth_left if both should have the same movement
        mouth_right = mouth_left;


     servo_mouth_cat.write(180-mouth_left);
     servo_mouth_fox.write(180-mouth_right);  //this servo is 180 in idle position


    int mouth_left_turn_in_ =  map(pulseIn(RECV_HEAD_TURN_CAT,HIGH),1000,2000,0,180);

    servo_head_turn_cat.write(mouth_left_turn_in_);
  

    int mouth_right_turn_in_ =  map(pulseIn(RECV_HEAD_TURN_FOX,HIGH),1000,2000,0,180);

    servo_head_turn_fox.write(mouth_right_turn_in_);
 
/*      Serial.print(mouth_left,DEC);
      Serial.write("/");
      Serial.println(mouth_right,DEC);      //delay(10);
*/
 
    /* Serial.print(mouth_sync_sw,DEC);
     Serial.write("/");
     Serial.println(mouth_right_turn_in_,DEC);      //delay(10);
      */
      //myservo.write(mouth_in);
      
      //map speed
      int speed = map(pulseIn(RECV_SPEED,HIGH),1000,2000,-255,255);
      int steering = pulseIn(RECV_STEERING,HIGH);
      
      double mapped = map_float(steering,1000,2000,0,1);

      //calculate speeds of both motors from throttle and steering
      int left=mapped*speed*2;
      int right=(1-mapped)*speed*2;
      left = constrain(left,-255,255);
      right = constrain(right,-255,255);
      
      ConstrainZeroPosition(&left,ZERO_LIMIT_POS);
      ConstrainZeroPosition(&right,ZERO_LIMIT_POS);


      //turn at position?
      
      int steering_mapped_255 = map(steering,1000,2000,-255,255);
      ConstrainZeroPosition(&speed,ZERO_LIMIT_POS*2);
      ConstrainZeroPosition(&steering_mapped_255,ZERO_LIMIT_POS);
      if(speed==0)
      {
        speed= steering_mapped_255;
        left=speed;
        right=-speed;
        left = constrain(left,-255,255);
        right = constrain(right,-255,255);
 
      }
      
      
      
   /*   Serial.print(left,DEC);
      Serial.write("/");
      Serial.println(right,DEC);*/
     
   /*   Serial.print(steering_mapped_255,DEC);
      Serial.write("/");
      Serial.println(speed,DEC);*/

     
      //delay(10);

      //set directions
      //for/backwards?
      if(left>0)
        runForward(1);
      else
        runBackward(1);
      
      if(right>0)
        runForward(2);
      else
        runBackward(2);
      
        
      analogWrite(SPEED_CONTROL_1,abs(left));
      analogWrite(SPEED_CONTROL_2,abs(right));
}





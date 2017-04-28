#include <Servo.h>

#define PIN_SERVO_LEFTRIGHT 8
#define PIN_SERVO_UPDOWN   9

#define PIN_SWITCH  7
#define PIN_LIGHT   6

Servo servo_leftright;
Servo servo_updown;

int smooth_x = 0;
int smooth_y = 0;

int buttonPushCounter=0;
int lastButtonState = 0;

int led_intensity = 0;
long led_start_timepoint = 0;
long led_last_timepoint = 0;

int wait_time = 0;

void setup()
{
//Serial.begin(115200);


pinMode(PIN_SERVO_LEFTRIGHT,OUTPUT);
pinMode(PIN_SERVO_UPDOWN,OUTPUT);
servo_leftright.attach(PIN_SERVO_LEFTRIGHT);
servo_updown.attach(PIN_SERVO_UPDOWN);


pinMode(PIN_SWITCH,INPUT);
pinMode(PIN_LIGHT,OUTPUT);

}



void loop()
{ 

 int buttonState = digitalRead(PIN_SWITCH);

 
 if(buttonState == 1)
 {  

    //check if button is pressed
    //a) longer than 600ms (to prevent accidentally triggering nose flickering
    //b) and if time that passed since last state change of LED is longer than wait_time (will be randomly assigned in function itself)
 
    if((millis()-led_start_timepoint>600) && (millis()-led_last_timepoint>wait_time))
    {
      static int light_state=0;
      led_last_timepoint = millis();

      light_state = light_state^1;    //invert LED state (if it was on, set it off and vice vera)
      digitalWrite(PIN_LIGHT,light_state);

      wait_time = random(10,100); //assign random wait-time until LED state will change the next time (random flicker)
     
      }

    //check if button is pressed in the first second of startup, then trigger auto-mode
    if(millis()<1000)
    {
      AutoMode();
    }

 }
  else 
  {
    led_intensity = 0;
    led_start_timepoint = millis();
    digitalWrite(PIN_LIGHT,0);
  }
  
/*  for(int count=0; count < 10; count++)
  {
    int sensor_x = map(analogRead(0),0,1023,0,180);
    int sensor_y = map(analogRead(1),0,1023,0,180);
                          
    smooth_x = (smooth_x + sensor_x) /2;
    smooth_y = (smooth_y + sensor_y) /2;
    delay(1);
  }
   */

    int smooth_x = map(analogRead(0),0,1023,20,160);
    int smooth_y = map(analogRead(1),0,1023,20,160);

   
  if(smooth_x>90)
    smooth_x = 90 + 90* (1-(sin(3.14*smooth_x/180)));
  else if(smooth_x<90)
    smooth_x = 90 - 90* (1-(sin(3.14*smooth_x/180)));

  //inverse joystick left-right
  smooth_x = 180-smooth_x;

  
  //NOTE THAT HERE WE RESTRICT THE EXTENT OF MOVEMENT BY REDUCING THE SECOND FACTOR
  if(smooth_y>90)
    smooth_y = 90 + 50* (1-(sin(3.14*smooth_y/180)));
  else if(smooth_y<90)
    smooth_y = 90 - 50* (1-(sin(3.14*smooth_y/180)));

  servo_leftright.write(smooth_x);
  servo_updown.write(smooth_y);
  
}





//Here we have a hard-coded sequence of movements that will be triggered if the switch is pressed upon startup
void AutoMode()
{

    servo_leftright.write(90);
    servo_updown.write(90);
  
    int _delay = 10;


    //move head to one direction, start hard, end soft
    for(int x=0;x<90;x++)
    {
      servo_leftright.write(90+90*(sin(3.14*x/180)));
      delay(_delay);
      }

    //move it completely to other side, start hard, end soft
    for(int x=0;x<90;x++)
    {
      servo_leftright.write(180-180*(sin(3.14*x/180)));
      delay(_delay);
      }

    //start soft, end soft, 40 degrees (scan audience)
    for(int x=0;x<180;x++)
    {
      servo_leftright.write(0+40*(sin(3.14*x/180)));
      delay(_delay/2);
      }      

    //start soft, end soft, 40 degrees (scan audience)
    for(int x=0;x<180;x++)
    {
      servo_leftright.write(40+40*(sin(3.14*x/180)));
      delay(_delay/2);
      }      

    //start soft, end soft, look upwards
    for(int x=0;x<180;x++)
    {
      servo_updown.write(90+40*(sin(3.14*x/180)));
      delay(_delay/2);
      }      

  }

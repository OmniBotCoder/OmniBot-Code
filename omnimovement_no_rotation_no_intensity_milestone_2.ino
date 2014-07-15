#include <PS2X_lib.h>                                                       //import the code from the PS2X library alowing the use of many specialized functions
#include <Servo.h>                                                          //import the code from the Servo library alowing the use of many specialized functions
#include <Math.h>                                                           //import the code from the math library alowing the use of many specialized functions

PS2X ps2x;                                                                  // create PS2 Controller Class

Servo myservoA;                                                             //declare "myservoA" as a Servo
Servo myservoB;                                                             //declare "myservoB" as a Servo
Servo myservoC;                                                             //declare "myservoC" as a Servo

                                                                            //right now, the library does NOT support hot pluggable controllers, meaning 
                                                                            //you must always either restart your Arduino after you conect the controller, 
                                                                            //or call config_gamepad(pins) again after connecting the controller.

int error = 0;                                                              //needed when we configure the gamepad 
byte type = 0;                                                              //this library allows for the use of a guitar hero controller too and so we need to figure out the type of controller

byte vibrate = 0;                                                           //for vibration

double rightXValue = 0;                                                     //declare rightXValue which will be a modified value from the x value of the right analog control
double rightYValue = 0;                                                     //declare rightyValue which will be a modified value from the y value of the right analog control

double angle = 0;                                                           //declare angle which will be calculated using the right X and Y values
double intensity = 0;                                                       //declare intensity which was not used effectively in this stage of the code but will be used in later files

void setup(){
 Serial.begin(57600);                                                       //start serial monitor
 
 myservoA.attach(5);                                                        //attach myservoA to pin 5
 myservoB.attach(6);                                                        //attach myservoB to pin 6
 myservoC.attach(11);                                                       //attach myservoC to pin 11
 
 pinMode(5,OUTPUT);                                                         //set pin 5 to output
 pinMode(6,OUTPUT);                                                         //set pin 6 to output
 pinMode(11,OUTPUT);                                                        //set pin 11 to output
  
 error = ps2x.config_gamepad(9,8,7,10, true, true);                         //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  
}

void loop()
  { 
    ps2x.read_gamepad(false, vibrate);                                      //read controller and set large motor to spin at 'vibrate' speed
        
        rightXValue = map(ps2x.Analog(PSS_RX),0,255,-100,100);              //take the x-value of the right analog stick, but make it range from -100 to 100 instead of 0 to 255
        rightYValue = map(ps2x.Analog(PSS_RY)+1,0,255,100,-100);            //take the y-value of the right analog stick, but make it range from -100 to 100 instead of 0 to 255 and invert it
        intensity = sqrt(sq(rightXValue)+sq(rightYValue));                  //not used now, but calculates intensity as the distance of the analog stick from the center using the pythagorean theorem
        if (rightXValue>=0)                                                 //if x is positive
        {
          angle = atan(rightYValue/rightXValue);                            //calculates the angle using the arctan of y over x 
        }
        else                                                                //if x is negative
        {
          angle = atan(rightYValue/rightXValue)+3.14;                       //arctan will only return values from 0 to 3.14 which is inaccurate if x is negative and this makes it accurate by adding 3.14
        }
        if (angle<0)                                                        //if angle is negative
        {
          angle = angle+6.28;                                               //negative angles are annoying to work with and this makes all angles positive yet still accurate
        }
        
        angle = angle-6.28/4;                                               //rotate all angles 1.57 clockwise so zero degress is now forward/up
        
        if (angle<0)                                                        //fix negative angle issue again
        {
          angle = angle+6.28;
        }
                                                                            //use serial monitor to print values to help debug if needed
        Serial.print("Stick Values:");
        Serial.print(rightXValue); 
        Serial.print(",");
        Serial.println(rightYValue);  
        Serial.println(angle);
        
                                                                            //this code uses the angle to create 6 indvidual slices and each slice is equally sized and all have unique code inside of them
                                                                            //while each slice consistently tells one servo to move in a certain way, most send values that flucate based on angle for truly omnidirectional movement as opposed to just 6 directions
                                                                            //once again Serial Monitor is used for dubugging help
        if (0<angle && angle<=1.05)
        {
          myservoA.writeMicroseconds(map2(angle,0,1.05,2000,1500));         //go clockwise, but slow down as angle increases
          myservoB.writeMicroseconds(1000);                                 //go counterclockwise
          myservoC.writeMicroseconds(map2(angle,0,1.05,1500,2000));         //go clockwise, but speed up as angle increases
          Serial.println("SLICE ONE");
        }
        else if (1.05<angle && angle<=2.09)
        {
          myservoA.writeMicroseconds(map2(angle,1.05,2.09,1500,1000));      //go counterclockwise, but speed up as angle increases
          myservoB.writeMicroseconds(map2(angle,1.05,2.09,1000,1500));      //go counterclockwise, but slow down as angle increases
          myservoC.writeMicroseconds(2000);                                 //go clockwise
          Serial.println("SLICE TWO");
        }
        else if (2.09<angle && angle<=3.14)
        {
          myservoA.writeMicroseconds(1000);                                 //go counterclockwise
          myservoB.writeMicroseconds(map2(angle,2.09,3.14,1500,2000));      //go clockwise, but speed up as angle increases
          myservoC.writeMicroseconds(map2(angle,2.09,3.14,2000,1500));      //go clockwise, but slow down as angle increases
          Serial.println("SLICE THREE");
        }
        else if (3.14<angle && angle<=4.19)
        {
          myservoA.writeMicroseconds(map2(angle,3.14,4.19,1000,1500));      //go counterclockwise, but slow down as angle increases
          myservoB.writeMicroseconds(2000);                                 //go clockwise
          myservoC.writeMicroseconds(map2(angle,3.14,4.19,1500,1000));      //go counterclockwise, but speed up as angle increases
          Serial.println("SLICE FOUR");
        }
        else if (4.19<angle && angle<=5.23)
        {
          myservoA.writeMicroseconds(map2(angle,4.19,5.23,1500,2000));      //go clockwise, but speed up as angle increases
          myservoB.writeMicroseconds(map2(angle,4.19,5.23,2000,1500));      //go clockwise, but slow down as angle increases
          myservoC.writeMicroseconds(1000);                                 //go counterclockwise
          Serial.println("SLICE FIVE");
        }
        else if (5.23<angle && angle<=6.28)
        {
          myservoA.writeMicroseconds(2000);                                 //go clockwise
          myservoB.writeMicroseconds(map2(angle,5.23,6.28,1500,1000));      //go counterclockwise, but speed up as angle increases
          myservoC.writeMicroseconds(map2(angle,5.23,6.28,1000,1500));      //go counterclockwise, but slow down as angle increases
          Serial.println("SLICE SIX");
        }
        else                                                                //if the analog stick is not being moved or is being buggy
        {
          myservoA.writeMicroseconds(1500);                                 //stop moving
          myservoB.writeMicroseconds(1500);                                 //stop moving
          myservoC.writeMicroseconds(1500);                                 //stop moving
          Serial.println("NO SLICE");
        }
    delay(50);   
}
                                                                            //the typical map value only works with long integers and so I had to make my own map2 function to work with decimals
double map2(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


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
double rightYValue = 0;                                                     //declare rightYValue which will be a modified value from the y value of the right analog control
double leftXValue = 0;                                                      //declare leftXValue which will be a modified value from the x value of the left analog control
double leftYValue = 0;                                                      //declare leftYValue which will be a modified value from the y value of the left analog control

double angle = 0;                                                           //declare angle which will be calculated using the right X and Y values
double lAngle = 0;                                                          //declare lAngle which will be calculated using the left X and Y values
double intensity = 0;                                                       //declare intensity which will represent how far from the origin the right analog stick is and will be used (along with other things) to determine servo speed 
double lIntensity;                                                          //declare lIntensity which will represent how far from the origin the left analog stick is and will be used (along with other things) to determine servo speed 

                                                                            //The arduino has great difficulty doing math inside of functions, so these variables break up the math into smaller steps
double intensityApplied;                                                    //declare intensityApplied which will be a value in microseconds based on intensity
double lIntensityApplied;                                                   //declare lIntensityApplied which will be a value in microseconds based on lIntensity

double clockwiseIntensityApplied;                                           //declare clockwiseIntensityApplied which adds intensityApplied to 1500 (stationary) to give a variable clockwise speed
double counterIntensityApplied;                                             //declare counterIntensityApplied which subtracts intensityApplied from 1500 (stationary) to give a variable counter-clockwise speed
double clockwiseLIntensityApplied;                                          //declare clockwiseLIntensityApplied which adds intensityApplied to 1500 (stationary) to give a variable clockwise speed
double counterLIntensityApplied;                                            //declare counterLIntensityApplied which subtracts intensityApplied from 1500 (stationary) to give a variable counter-clockwise speed


void setup(){
 Serial.begin(57600);                                                       //start serial monitor
 
 myservoB.attach(5);                                                        //attach myservoB to pin 5
 myservoC.attach(6);                                                        //attach myservoC to pin 6
 myservoA.attach(11);                                                       //attach myservoA to pin 11
                                                                            //It should be noted that it is not in the initial abc order as I wanted to alter it to change which side of the robot
                                                                            //was facing forward- an issue easier to change in code than in mechanics
 
 pinMode(5,OUTPUT);                                                         //set pin 5 to output
 pinMode(6,OUTPUT);                                                         //set pin 6 to output
 pinMode(11,OUTPUT);                                                        //set pin 11 to output
  
 error = ps2x.config_gamepad(9,8,7,10, true, true);                         //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  
}

void loop()
  { 
    ps2x.read_gamepad(false, vibrate);                                      //read controller and set large motor to spin at 'vibrate' speed
        
                                                                            //This block of code seeks to make the values from the analog sticks fit on a coordinate grid to make the trig easier
        rightXValue = map(ps2x.Analog(PSS_RX),0,255,-100,100);              //take the x-value of the right analog stick, but make it range from -100 to 100 instead of 0 to 255
        rightYValue = map(ps2x.Analog(PSS_RY)+1,0,255,100,-100);            //take the y-value of the right analog stick, but make it range from -100 to 100 instead of 0 to 255 and invert it
        leftXValue = map(ps2x.Analog(PSS_LX),0,255,-100,100);               //take the x-value of the left analog stick, but make it range from -100 to 100 instead of 0 to 255
        leftYValue = map(ps2x.Analog(PSS_LX)+1,0,255,100,-100);             //take the y-value of the left analog stick, but make it range from -100 to 100 instead of 0 to 255 and invert it

                                                                            //This block of code deals with utilizing the x and y values to produce a useable and accurate measure of angle needed for direction
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
        
                                                                            //This block of code deals with warping the square coordinate grid to restrict it to a circular area
                                                                            //This is to make sure intensity will never exceed 100 and that the omnibot can go full speed (and not faster) in all directions
                                                                            //Given the angle and the premise that the hypotenuse should never exceed 100, it calculates the maximum x and y values
                                                                            //it then maps the actual x and y values so that they fall in between the maximum x and y values
                                                                            //sin and cosine act differently in each quadrant and so if statements are used to adress the quirks of each quadrant
        if(angle>4.71)                                                      //if the angle puts it in quadrant IV
        {
          rightXValue = map2(rightXValue,-100,100,-100*cos(angle),100*cos(angle));
          rightYValue = -map2(rightYValue,-100,100,-100*sin(angle),100*sin(angle));
        }
        else if(angle>3.14)                                                 //if the angle puts it in quadrant III
        {
          rightXValue = -map2(rightXValue,-100,100,-100*cos(angle),100*cos(angle));
          rightYValue = -map2(rightYValue,-100,100,-100*sin(angle),100*sin(angle));
        }
        else if(angle>1.57)                                                 //if the angle puts it in quadrant II
        {
          rightXValue = -map2(rightXValue,-100,100,-100*cos(angle),100*cos(angle));
          rightYValue = map2(rightYValue,-100,100,-100*sin(angle),100*sin(angle));
        }            
        else                                                                //if the angle puts it ibn quadrant I
        {
          rightXValue = map2(rightXValue,-100,100,-100*cos(angle),100*cos(angle));
          rightYValue = map2(rightYValue,-100,100,-100*sin(angle),100*sin(angle));
        }
        
        intensity = sqrt(sq(rightXValue)+sq(rightYValue));                  //now that the x and y values have been warped use pythagoras' theorem to calculate the distance from the origin using the new x and y
        
                                                                            //it is much less essntial to warp the left stick howeverfor the sake of consistency both are warped
                                                                            //this way both sticks react in the same way so as not to confuse the user
        
        if (leftXValue>=0)
        {
          lAngle = atan(leftYValue/leftXValue);
        }
        else
        {
          lAngle = atan(leftYValue/leftXValue)+3.14;
        }
        
        if (lAngle<0)
        {
          lAngle = lAngle+6.28;
        }
        if(lAngle>4.71)
        {
          leftXValue = map2(leftXValue,-100,100,-100*cos(lAngle),100*cos(lAngle));
          leftYValue = -map2(leftYValue,-100,100,-100*sin(lAngle),100*sin(lAngle));
        }
        else if(lAngle>3.14)
        {
          leftXValue = -map2(leftXValue,-100,100,-100*cos(lAngle),100*cos(lAngle));
          leftYValue = -map2(leftYValue,-100,100,-100*sin(lAngle),100*sin(lAngle));
        }
        else if(lAngle>1.57)
        {
          leftXValue = -map2(leftXValue,-100,100,-100*cos(lAngle),100*cos(lAngle));
          leftYValue = map2(leftYValue,-100,100,-100*sin(lAngle),100*sin(lAngle));
        }            
        else
        {
          leftXValue = map2(leftXValue,-100,100,-100*cos(lAngle),100*cos(lAngle));
          leftYValue = map2(leftYValue,-100,100,-100*sin(lAngle),100*sin(lAngle));
        }
        
        lIntensity = abs(leftXValue);                                       //since the omnibot cannot rotate up or down, we only care about the x value for left and right
        
         angle = angle-6.28/4;                                              //rotate all angles 1.57 clockwise so zero degress is now forward/up
        
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
        Serial.println(intensity);
        
                                                                            //this block of code makes use of a whole bunch of variables so that the arduino has the math broken up for it
                                                                            //additionally for dwbugging purposes, multiple variables allow one to pinpoint the point or error with greater ease
        intensityApplied=2.5*intensity;                                     //convert intensity into a microsecond value called intensityApplied
        lIntensityApplied=2.5*lIntensity;                                   //convert lIntensity into a microsecond value called LIntensityApplied
        clockwiseIntensityApplied=1500+intensityApplied;                    //add intensityApplied to the stationary speed to get a varying clockwise speed called clockwiseIntensityApplied
        counterIntensityApplied=1500-intensityApplied;                      //subtract intensityApplied from the stationary speed to get a varying counter-clockwise speed called counterIntensityApplied
        clockwiseLIntensityApplied=1500+lIntensityApplied;                  //add intensityApplied to the stationary speed to get a varying clockwise speed called clockwiseLIntensityApplied
        counterLIntensityApplied=1500-lIntensityApplied;                    //subtract intensityApplied from the stationary speed to get a varying counter-clockwise speed called counterLIntensityApplied
        
        Serial.println(clockwiseIntensityApplied);
        Serial.println(counterIntensityApplied);
        
                                                                            //this code uses the angle to create 6 indvidual slices and each slice is equally sized and all have unique code inside of them
                                                                            //while each slice consistently tells one servo to move in a certain way, most send values that flucate based on angle for truly omnidirectional movement as opposed to just 6 directions
                                                                            //as an imporvement on my previous milestone, the maximum speed for each slice has been restricted by the intensity
                                                                            //once again Serial Monitor is used for dubugging help
        if (0<angle && angle<=1.05)
        {
          myservoA.writeMicroseconds(map2(angle,0,1.05,clockwiseIntensityApplied,1500));
          myservoB.writeMicroseconds(counterIntensityApplied);
          myservoC.writeMicroseconds(map2(angle,0,1.05,1500,clockwiseIntensityApplied));
          Serial.println("SLICE ONE");
        }
        else if (1.05<angle && angle<=2.09)
        {
          myservoA.writeMicroseconds(map2(angle,1.05,2.09,1500,counterIntensityApplied));
          myservoB.writeMicroseconds(map2(angle,1.05,2.09,counterIntensityApplied,1500));
          myservoC.writeMicroseconds(clockwiseIntensityApplied);
          Serial.println("SLICE TWO");
        }
        else if (2.09<angle && angle<=3.14)
        {
          myservoA.writeMicroseconds(counterIntensityApplied);
          myservoB.writeMicroseconds(map2(angle,2.09,3.14,1500,clockwiseIntensityApplied));
          myservoC.writeMicroseconds(map2(angle,2.09,3.14,clockwiseIntensityApplied,1500));
          Serial.println("SLICE THREE");
        }
        else if (3.14<angle && angle<=4.19)
        {
          myservoA.writeMicroseconds(map2(angle,3.14,4.19,counterIntensityApplied,1500));
          myservoB.writeMicroseconds(clockwiseIntensityApplied);
          myservoC.writeMicroseconds(map2(angle,3.14,4.19,1500,counterIntensityApplied));
          Serial.println("SLICE FOUR");
        }
        else if (4.19<angle && angle<=5.23)
        {
          myservoA.writeMicroseconds(map2(angle,4.19,5.23,1500,clockwiseIntensityApplied));
          myservoB.writeMicroseconds(map2(angle,4.19,5.23,clockwiseIntensityApplied,1500));
          myservoC.writeMicroseconds(counterIntensityApplied);
          Serial.println("SLICE FIVE");
        }
        else if (5.23<angle && angle<=6.28)
        {
          myservoA.writeMicroseconds(clockwiseIntensityApplied);
          myservoB.writeMicroseconds(map2(angle,5.23,6.28,1500,counterIntensityApplied));
          myservoC.writeMicroseconds(map2(angle,5.23,6.28,counterIntensityApplied,1500));
          Serial.println("SLICE SIX");
        }
        
                                                                            //this block deals with rotation and comes after movement as I prioritised movement over rotation
                                                                            //this means that if both sticks are moved, the arduino will only care about the right one
                                                                            //roation is much simpler and first just figures out if it should rotate to the left or to the right
                                                                            //and the figures out how far left or right the stick is being pushed to determine how quickly to rotate

        else if(leftXValue<0)
        {
          myservoA.writeMicroseconds(counterLIntensityApplied);
          myservoB.writeMicroseconds(counterLIntensityApplied);
          myservoC.writeMicroseconds(counterLIntensityApplied);
        }
        else if(leftXValue>0)
        {
          myservoA.writeMicroseconds(clockwiseLIntensityApplied);
          myservoB.writeMicroseconds(clockwiseLIntensityApplied);
          myservoC.writeMicroseconds(clockwiseLIntensityApplied); 
        }
        
                                                                            //if no command is recieved the omnibot should stop moving
        else
        {
          myservoA.writeMicroseconds(1500);
          myservoB.writeMicroseconds(1500);
          myservoC.writeMicroseconds(1500);
          Serial.println("NO SLICE");
        }
        
              
 
 
    delay(50);
     
}
                                                                            //the typical map value only works with long integers and so I had to make my own map2 function to work with decimals
double map2(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


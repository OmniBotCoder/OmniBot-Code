#include <PS2X_lib.h>                                                       //import the code from the PS2X library alowing the use of many specialized functions

PS2X ps2x;                                                                  //create PS2 Controller Class

                                                                            //right now, the library does NOT support hot pluggable controllers, meaning 
                                                                            //you must always either restart your Arduino after you conect the controller, 
                                                                            //or call config_gamepad(pins) again after connecting the controller.

                                                                            //declare all of the variables
int error = 0;                                                              //needed when we configure the gamepad 
byte type = 0;                                                              //this library allows for the use of a guitar hero controller too and so we need to figure out the type of controller

byte vibrate = 0;                                                           //for vibration

                                                                            //declare all of the variables for the pins for the different colored LEDs; they all start out negative and the arduino has no negative pins so nothing will turn on until the value is made positive
int red =-13;                                                               //set the red LED for what will become pin 13
int green =-12;                                                             //set the green LED for what will become pin 12 
int pink=-3;                                                                //set the pink LED for what will become pin 3
int blue=-10;                                                               //set the blue LED for what will become pin 10

void setup(){
 Serial.begin(57600);                                                       //start Serial Monitor
 
                                                                            //This block sets up some of the pins to output; though all of the pins declared above are being used for output. To get the color right I used LEDs from different companies and so they vary greatly in brightness. My blue and pink pins were obnoixously bright and not making those pins dedicated for output balances their brightness to match the other LEDs
 pinMode(abs(red),OUTPUT);                                                  //set the pin for the red LED to be output
 pinMode(abs(green),OUTPUT);                                                //set the pin for the green LED to be output
  
 error = ps2x.config_gamepad(9,8,7,10, true, true);                         //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
 
 if(error == 0){                                                            //if there is no error
   Serial.println("Found Controller, configured successful");
   Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
  Serial.println("holding L1 or R1 will print out the analog stick values.");
  Serial.println("Go to www.billporter.info for updates and to report bugs.");
 }
                                                                            //for the various different types of errors
  else if(error == 1)
   Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
   
  else if(error == 2)
   Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
   
  else if(error == 3)
   Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
   
   type = ps2x.readType();                                                  //figure out the type of controller being used
     switch(type) {
       case 0:
        Serial.println("Unknown Controller type");
       break;
       case 1:
        Serial.println("DualShock Controller Found");
       break;
       case 2:
         Serial.println("GuitarHero Controller Found");
       break;
     }
  
}

void loop()      
  { 
 if(error == 1)                                                             //skip loop if no controller found
  return; 
                                                                            //unnecessary code regarding the guitar hero controller has been removed
 else {                                                                     //DualShock Controller
  
    ps2x.read_gamepad(false, vibrate);                                      //read controller and set large motor to spin at 'vibrate' speed
  
    if(map(ps2x.Analog(PSS_RY),0,255,255,0)*2-255>0)                        //for whatever reason the controller will still vibrate when given a negative value and this fixes that
    {
      vibrate = map(ps2x.Analog(PSS_RY),0,255,255,0)*2-255;                 //set the large motor vibrate speed based on the value of how hard the right analog control is pushed up
                                                                            //the map function works to invert the values so pushing up returns a higher value - more intuitive this way
                                                                            //subtraction is necessary otherwise it would half buzz at rest
                                                                            //multiplication is then needed to get the full range of vibration which the subtraction just restricted
    }      
   
                                                                            //serial monitor is used to confirm that the controller is recieving data properly
                                                                            //in the event that the code fails this will help identify which part of the code or hardware is causing the problem
                                                                            //all the colors are then multiplied by negative one if pressed to allow them to either be turned on by an existent pin or to push them to a non-existent pin
   
    if(ps2x.ButtonPressed(PSB_RED))                                         //if red pressed
         {
           red=red*-1;                          
           Serial.println(red);                 
           
         }
         
      if(ps2x.ButtonPressed(PSB_PINK))                                      //if pink pressed
         {
           pink=pink*-1;
           Serial.println(pink);
           Serial.println(ps2x.Analog(PSS_RY));
         }     
    
    if(ps2x.ButtonPressed(PSB_BLUE))                                        //if blue pressed
         {  
           blue=blue*-1;
           Serial.println(blue);
         }
    if(ps2x.ButtonPressed(PSB_GREEN))                                       //if green pressed
         {
           green=green*-1;
           Serial.println(green);
         }  
    
    digitalWrite(red, HIGH);                                                //if red is positive, this will turn the LED on
    digitalWrite(-red, LOW);                                                //if red is negative, this will then make it positive and turn the LED off
    digitalWrite(blue, HIGH);                                               //if blue is positive, this will turn the LED on
    digitalWrite(-blue, LOW);                                               //if blue is negative, this will then make it positive and turn the LED off
    analogWrite(pink, (map(ps2x.Analog(PSS_RY),0,255,255,0)));              //if pink is positive, this will turn the LED on with a brightness relative to the inverted value of the right analog controls Y value (Up/down)
    analogWrite(-pink, 0);                                                  //if pink is negative, this will make it positive and turn the LED off
    digitalWrite(green, HIGH);                                              //if green is positive, this will turn the LED on
    digitalWrite(-green, LOW);                                              //if green is negative, this will make it positive and turn the LED off
 }
 
 
 delay(50);                                                                 //slow down the program to only run every .05 second which is still very responsive
     
}


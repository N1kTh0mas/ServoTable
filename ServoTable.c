//Servo Table By Nik Thomas
//Made for MicroScreen LLC 2021

//Define Robot Pulse Pins
#define RobotPin 2
#define RobotOut 8

//define Buttons
#define GoPin 4 //GreenButton
#define YellowButton 7
#define RedButton 10

//Define Light Pins
#define RedLight 13
#define GreenLight 12
#define YellowLight 9

//Define Buzzer Pins
#define Buzzer 11
#define echoPin 5
#define trigPin 3

//Defining Notes for Buzzer
#define NOTE_C4 262
#define NOTE_G3 196
#define NOTE_A3 220

//Servo Library
#include <Servo.h>
#include "AccelStepper.h"

//Buzzer Vars
long duration;
long distance;

//Logic Vars
bool RBactive;
int phasenum;

//Servo Vars
AccelStepper ServoMotor()
Servo myservo;
int ServoPos;
bool ServoReverse;
int ReqDistance;


void setup()
{
  pinMode(RobotPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RedLight, OUTPUT);
  pinMode(GreenLight, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(GoPin, INPUT);
  pinMode(YellowButton, INPUT);
  pinMode(RedButton, INPUT);
  pinMode(RobotOut, OUTPUT);
  pinMode(YellowLight, OUTPUT);

    //Setting up Servos and tracking
  myservo.attach(6, 500, 2500);
  myservo.write(0);
  ServoPos = 0;
  ServoReverse = false;
    //Settin Defaults
  phasenum = 0;
  //Robot Active (If Robot is welding or not)
  RBactive = false;

  ReqDistance = 30;
  
  //Turns off all lights
  LightChange(false, false, false);
  //Plays Song once all setup is done!
  Song();
}

void loop()
{

//Phase 0 is deafault pos. Ready for User Input
//Phase 1 is First side welding and then roatation of Servo
//Phase 2 is Second Side Welding and then roation back of Servo
//Phase 3 is manual fliping (Not part of main Operation)
  if (phasenum == 0){
    
   //Checks if robot is in standby
    if (RBactive == false){
        //Changes Light to Green
        LightChange(false, true, false);

            //If Green Button is Pressed Then...
            if (digitalRead(GoPin) == HIGH){
                //RBout Sends Pulse to Robot
            RBout();
                //Sets Robot Activ to true
            RBactive = true;
                // Goes to Phase 1
            phasenum = 1;
            
            }

            //If Yellow Button is Pressed then...
            if (digitalRead(YellowButton) == HIGH){
                // Goes to Phase 3 (Manual Flip Mode)
                phasenum = 3;
            }
    }
    //Checks if Robot is Active
    if (RBactive == true){
        //turns all lights on (error message)
      LightChange(true, true, true);
    }
 
    } //End of Phase 0
  
  //Phase 1
  if (phasenum == 1){
   //Checks if Robot is active
    if (RBactive == true){
        //Changes Light to Yellow
        LightChange(false, false, true);
        
        //Checks if Robot has sent finished pulse back
        if (digitalRead(RobotPin) == HIGH) {
            //Sets Robot Active to false
            RBactive = false;
            //Changes Light to Red
            LightChange(true, false, false);
        
        }
    } // End of RBactive true

    //Checks if Robot is in standby
    if (RBactive == false){
        //Flips Table Over to 180 Deg.
        myservo.write(180);
        delay(2000);
        
        //Useing the Prox sensor to tell when table has completed its flip
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
  
        duration = pulseIn(echoPin, HIGH);
        
        distance = duration * .034 / 2;

        //Checks if distance readout is less than Required Distance (cm)
        if (distance < ReqDistance){
            //Goes to Phase 2
            phasenum = 2; 
            //Sets Robot Active to True
            RBactive = true;
            //Sends Pulse to Robot to start welding
            RBout();
        }
      
    } // End of RBactive false
    
  } //End of Phase 1

    //Phase 2
  if (phasenum == 2){
    
    //Checks it Robot Active is true
    if(RBactive == true){
        //Changes Light to Yellow
        LightChange(false, false, true);

        //Checks if Robot has send Completion Pulse
        if(digitalRead(RobotPin) == HIGH){
            //Sets Robot Active to False
            RBactive = false;
            //Changes Light to Red
            LightChange(true, false, false);
        }
  
    } // End RB Active true

    if (RBactive == false){

        //Flips Servo back to 0 Deg.
        myservo.write(0);   
        delay(2000);
      
        //Useing the Prox sensor to tell when table has completed its flip
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        
        duration = pulseIn(echoPin, HIGH);
        
        distance = duration * .034 / 2;

        //Checks if distance readout is less than Required Distance (cm)
        if (distance < ReqDistance){
            //Goes to Phase 0
            phasenum = 0; 
            //Plays Song to note Completion
            Song();  
            }

        } //End of RBactive false

    } //End of Phase 2

  //Phase 3 (Manual Mode)
  //Moves Servo every 45 Deg.
  if (phasenum == 3) {
      //Changes Light to Red and Yellow
    LightChange(true, false, true);

    //Checks if Yellow Button is pressed
    if (digitalRead(YellowButton) == HIGH) {
        //Checks if Servo is Less than or equal to 135 and it is going forward.
        if (ServoPos <= 135 && ServoReverse == false && ServoPos != 180){
            //Adds 45 Deg to Servo Position and Sends it
            ServoPos = ServoPos + 45;
            delay(500);
        }

        //Checks if Servo is at 180 Deg.
        else if (ServoPos == 180) {
            //Reverses the Direction of the Servo
            ServoReverse = true;
            delay(500);
        }

        //Checks if Servo is Greater than or equal to 45 and the Servo is going in reverse
        if (ServoPos >= 45 && ServoReverse == true ){
            //Subtracts 45 Degs from Servo Pos
            ServoPos = ServoPos - 45;
            delay(500);
        }

        //Checks if Servo is at 0 Deg
        else if (ServoPos == 0) {
            //Makes the Servo go in the forward direction
            ServoReverse = false;
            delay(500);
        }

        //Writes Final ServoPos to the Servo.    
        myservo.write(ServoPos);
      
    } // End of Yellow Button if Statment
    
    //Checks if Red Button is pushed and Servo is at 0 Deg.
    if (digitalRead(RedButton) == HIGH && ServoPos == 0){
        //Goes to Phase 0 (Standby)
        phasenum = 0;
    }

    //Checks if Red Button is pushed and the Servo is not at 0 Deg.
    else if (digitalRead(RedButton) == HIGH && ServoPos != 0){
        //Sets Servo Tracking to 0 and then moves Servo to 0
        ServoPos = 0;
        myservo.write(0);
        delay(1000);
        //Goes to Phase 0 (Standby)
        phasenum = 0;
        }

    } //End of Phase 3

} // End of Loop ()

//Method to Send Pulse to Robot
void RBout() {

//Temp Code (Simulates a LED light on)
 digitalWrite(RobotOut, HIGH);
  delay(100);
 digitalWrite(RobotOut, LOW);
}
  
//Methif to Change Light Colors or Buttons
//r = Red
//g = Green
//y = Yellow
void LightChange (bool r, bool g, bool y){
    //ON
    if (r == true){
      digitalWrite(RedLight, HIGH);
    }
    //OFF
    if (r == false){
      digitalWrite(RedLight, LOW);
    }
    //ON
    if (g == true){
      digitalWrite(GreenLight, HIGH);
    }
    //OFF
    if (g == false){
      digitalWrite(GreenLight, LOW);
    }
    //ON
    if (y == true){
      digitalWrite(YellowLight, HIGH);
    }
    //OFF
    if (y == false){
      digitalWrite(YellowLight, LOW);
    }
  
} // End of LightChange ()


//Method to play Song/Chime
void Song() {

    //Define the Order of Notes
    int melody[] = {
        NOTE_C4, NOTE_G3, NOTE_A3, NOTE_G3, NOTE_C4 
    };

    //Define How long Each not will be played
    int noteDurations[] = {
    4,4,4,4,4
    };
  
  //Runs Through all notes and plays them
  for (int thisNote = 0; thisNote < 8; thisNote++) {
   	int noteDuration  = 1000 / noteDurations[thisNote];
    tone(Buzzer, melody[thisNote], noteDuration);
    
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    
    noTone(Buzzer);
  }
} //End of Song()
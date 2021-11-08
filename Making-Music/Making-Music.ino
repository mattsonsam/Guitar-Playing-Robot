//-------------------------------------SETUP VARIABLES----------------------------------------------------------------------------------------------------
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Servo.h>

AccelStepper fretting(1,54,55); //using x-step pins
AccelStepper strumming(1,60,61); //using y-step pins
Servo muting;
Servo majorminor;

int frethome=3; //pin for limit switch for fretter
int strumhome=14; //pin for limit switch for strummer

long fret_strokelengthmm = 400; //the length of the entire fretting linear rail
double fret_step_per_rev=800; // steps per revolution of stepper
double fret_mm_per_rev = 72; //linear distance travelled in one rotation of stepper
double fret_mmPerStep = fret_mm_per_rev/fret_step_per_rev; //linear distance travelled in one step of the stepper

long strum_strokelengthmm; //these need to be determined
double strum_step_per_rev;
double strum_mm_per_rev;
double strum_mmPerStep = strum_mm_per_rev/strum_step_per_rev;

int fretHomingSpeed = -500;
int strumHomingSpeed = -200;

int majorminor_down = 30;
int majorminor_up = 0;
//int majorminor_angle = 45;
int mute_down = 35; //this needs to be determined
int mute_up = 60; //this needs to be determined 

int strumPosLeft = 10;
int strumPosRight = 1800;

//-----------------------------------Declare fretting chord positions, and timing relevent to hard coding songs-----------------------------------------


int E=0;
int F=40;
int Fs; //the "s" in Fs stands for "sharp"
int G;
int Gs;
int A;
int As;
int B;
int C;
int Cs;
int D;
int Ds;

int majorminor_time;
int muting_time; //variables representing the time in seconds it takes for the servos to move in either direction (assuming its the same time for both directions)



//------------------------------------SETUP FUNCTION-------------------------------------------------------------------------------------------------------
void setup() { 
  Serial.begin(9600);  
  pinMode(strumhome,INPUT_PULLUP); //sets two limit switches as inputs with internal resistors
  pinMode(frethome,INPUT_PULLUP);
  majorminor.attach(11);  //attaches majorminor servo to pin 11 
  muting.attach(4);
  servosChangingFrets();
  fretting.setMaxSpeed(1000000.0); //set arbitrarily high max speed, in units of [pulses/sec] (max reliable is about 4000 according to accelstepper).Prevents speed from being limited by code, we will not use this
  strumming.setMaxSpeed(1000000.0);
  delay(1000);
  
  goHome(fret_strokelengthmm, fret_mmPerStep, fretting, frethome, fretHomingSpeed);
  goHome(strum_strokelengthmm, strum_mmPerStep, strumming, strumhome, strumHomingSpeed);
}
//-------------------------------------END SETUP------------------------------------------------------------------


//-------------------------------------START VOID LOOP------------------------------------------------------------

void loop(){ //here is where we will call all our functions

  gotochord(E, 5, false);
  strum(700,strumPosRight);
  strum(700,strumPosLeft);
  muting.write(mute_down);
  delay(1000);
  gotochord(E, 5, true);
  strum(700,strumPosRight);
  strum(700,strumPosLeft);
  muting.write(mute_down);
  delay(1000);
  gotochord(F, 5, false);
  strum(700,strumPosRight);
  strum(700,strumPosLeft);
  muting.write(mute_down);
  delay(1000);
  gotochord(F, 5, true);
  strum(700,strumPosRight);
  strum(700,strumPosLeft);
  muting.write(mute_down);
  delay(1000);
  
}

//------------------------------------END VOID LOOP---------------------------------------------------------------


//------------------------------------START FUNCTIONS-------------------------------------------------------------
//---servos---
void servosUp(){ //both servos up
  majorminor.write(majorminor_up); 
  muting.write(mute_up);
}


void servosChangingFrets(){
  majorminor.write(majorminor_up); 
  muting.write(mute_down);
}

//---go home----
//Written by Amanda: gohome funct, will need to pass values in when called, may not be worth it to reduce # of functions by 1 to instead pass in values
void goHome(long strokeLengthmm, long mmPerStep, AccelStepper stepper, int limitSwitch, int homingSpeed){
  long strokelength=ceil(strokeLengthmm/mmPerStep); //calculates number of steps to traverse entire rail
  stepper.moveTo(strokelength);

  while(digitalRead(limitSwitch)==0){  
    stepper.setSpeed(homingSpeed); 
    stepper.run(); 
  }
  stepper.setCurrentPosition(0); //once home, set position to zero for reference
  delay(1000);
}
//---go to position----
  void gotoPositionAtSpeed(AccelStepper stepper, long posmm, long mmPerStep, float spedmm, float accelmm){ //function to tell the stepper to go to a  position at a specific speed with speed in units of mm/s
    long numSteps=ceil(posmm/mmPerStep);
    float Speed=spedmm/mmPerStep;
    float acceleration=accelmm/mmPerStep;
    stepper.setSpeed(Speed);
    stepper.setAcceleration(acceleration);
    stepper.moveTo(numSteps);
    while(stepper.currentPosition()!=stepper.targetPosition()){
      stepper.run();
      //Serial.println(stepper.currentPosition());
    }
}

void strum(int strumSpeed, int position){
  strumming.setSpeed(strumSpeed);
  strumming.setAcceleration(4000);
  strumming.runToNewPosition(position);
}

void moveFretter(int position){
  fretting.setSpeed(500);
  fretting.setAcceleration(4000);
  fretting.runToNewPosition(position);
  
}

void gotoPositionAtSpeedsteps(AccelStepper stepper, long mmPerStep, long posmm, float sped, float accel){ //function to tell the stepper to go to a position at a specific speed with speed in units of steps/s
    long numSteps=ceil(posmm/mmPerStep);
    //float Speed=spedmm/mmPerStep;
    //float acceleration=accelmm/mmPerStep;
    stepper.setSpeed(sped);
    stepper.setAcceleration(accel);
    stepper.moveTo(numSteps);
    while(stepper.currentPosition()!=stepper.targetPosition()){
      stepper.run();
      //Serial.println(stepper.currentPosition());
    }
}

//---motion math---
void trianglemotion (long posmm, long mmPerStep, float t, AccelStepper stepper){ //plans and executes the triangular motion profile to get the carriage to the desired position in the desired amount of time, limited by the max velocity of 4000 steps/s
  long targetsteps=floor(posmm/mmPerStep); //number of steps needed to get to abolute desired position
  float v_avg= abs((stepper.currentPosition()-targetsteps)/t); //avg speed to get to desired position from current position
  float v_max= 2*v_avg; //max velocity assuming equal time accelerating and decelerating
  float accel = (2*v_max)/t; //necessaey accel to get to v_max at time t/2
  //Serial.println(v_max);
  stepper.setSpeed(v_max); //set the target speed of travel
  stepper.setAcceleration(accel); //set the necessary accel/decel
  stepper.moveTo(targetsteps); //assign target location
  while(stepper.currentPosition()!=stepper.targetPosition()){ //while not at the target position, execute steps
    stepper.run();
  }
}

void trapmotion(long posmm, long mmPerStep, float t, AccelStepper stepper){     // this function uses a trapezoidal motion profile with equal times of acceleration, constant speed, and deceleration. from this link: 
  long targetsteps=floor(posmm/mmPerStep); //number of steps needed to get to abolute desired position
  float accel = (4.5*(stepper.currentPosition()-targetsteps))/pow(t,2);   //calc accel needed
  float v_max = (1.5*(stepper.currentPosition()-targetsteps))/t;       //calc max (target) velocity
  stepper.setSpeed(v_max);                 // set motion parameters
  stepper.setAcceleration(accel);
  stepper.moveTo(targetsteps);
  while(stepper.currentPosition()!=stepper.targetPosition()){ //while not at the target position, execute steps
    stepper.run();
  }
}

void gotochord( long chord, float t, bool major){ 
  servosChangingFrets();    
  long targetsteps=floor(chord/fret_mmPerStep); 
  float accel = (4.5*(fretting.currentPosition()-targetsteps))/pow(t,2);   
  float v_max = (1.5*(fretting.currentPosition()-targetsteps))/t;      
  fretting.setSpeed(v_max);                 
  fretting.setAcceleration(accel);
  fretting.moveTo(targetsteps);
  while(fretting.currentPosition()!=fretting.targetPosition()){ //while not at the target position, execute steps
    fretting.run();
  }
  muting.write(mute_up);
  if(major==true){
    majorminor.write(majorminor_down);
  }
}


void hotelcalifornia(){ //hard code hotel california chord progression
  
  
}

//-----------------------------------------END FUNCTIONS--------------------------------------------------------------


//-----------------------------------------EXPERIMENTAL---------------------------------------------------------------

//---------------------------DEFUNCT----------------------
//functions that likely are defunction but good to keep around in case we need to reference then later, can be added to a library of old stuff at some point to clean this up
/*void gotoangle(int a){ //this is unneeded as servo.write goes to a position and does not need to worry about delay
  //pos=90;
  while (frettingservo.read()!=a){
    if(a>frettingservo.read()){
      pos++;
    }
    if(a<frettingservo.read()){
      pos--;
    }
    frettingservo.write(pos);
    delay(15);
  }
}


void fretGoHome(){
  long fret_strokelength=ceil(fret_strokelengthmm/fret_mmPerStep); //calculates number of steps to traverse entire rail
  
  fretting.moveTo(fret_strokelength);

  while(digitalRead(frethome)==0){  
    fretting.setSpeed(-300); 
    fretting.run(); 
  }
  fretting.setCurrentPosition(0); //once home, set position to zero for reference
  delay(1000);
}


void strumGoHome(){ //need to adjust set speed and direction to properly approach limit switch
  long strum_strokelength=ceil(strum_strokelengthmm/strum_mmPerStep); //calculates number of steps to traverse entire rail
  strumming.moveTo(strum_strokelength);

  while(digitalRead(strumhome)==0){  
    strumming.setSpeed(-300); 
    strumming.run(); 
  }
  fretting.setCurrentPosition(0); //once home, set position to zero for reference
  delay(1000);
}
*/

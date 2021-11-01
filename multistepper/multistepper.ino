/*#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Servo.h>

AccelStepper stepper(1,54,55); //1 is a default setting, 54 is the pin connected to the "DIR+" or direction pin on the driver, 55 is the pin connected to "PUL+" or pulse pin
Servo frettingservo; //initialize servo                    
long strokelengthmm = 400; //long for maximum travel distance of fretting rail in mm. Accel stepper requires that inputs to movement functions is long (i think)
double step_per_rev=800; //number of steps per revolution of motor, given by stepper driver. Stepper drive has small DIP switches to select this
double mm_per_rev = 72; //Exerimentally determined number of mm traveled per revolution. This is used to convert from mm distances to number of steps.
double mmPerStep = mm_per_rev/step_per_rev; //conversion factor
int switch1 = 3; //signal pin for home limit switch (near headstock)
int switch2 = 9; //signal pin for end stop limit switch (near body)
int pos=45; //variable to set initial position of frettingservo upon startup, in degrees, of the fretting servo
void setup() { //runs once upon power up
  Serial.begin(9600);  
  pinMode(switch1,INPUT_PULLUP); //define limit switch pins as inputs, with internal pullup resistors used on arduino. Makes it so that no external resistor is needed with switch
  pinMode(switch2,INPUT_PULLUP);
  frettingservo.attach(11);  //defines signal pin for servo
  frettingservo.write(pos);  //move servo to angle defined by pos (intended to be not pressing string at first)
  stepper.setMaxSpeed(1000000.0); //set arbitrarily high max speed, in units of [pulses/sec] (max reliable is about 4000 according to accelstepper)
  delay(1000); //wait for things to finish moving
  gohome(); //do homing routine (see below)
  
  
}

void loop() {
trapmotion(100,2);

 
}
void gohome(){
  long strokelength=ceil(strokelengthmm/mmPerStep); // convert stroke length from mm to number of steps. This way the carriage will always travel far enough to hit the home switch.
  stepper.moveTo(strokelength);  //sets the target position in steps, of the stepper motor. It does not actually tell it to move yet. 
  while(digitalRead(switch1)==0){  //while the switch is not pressed, do the following
    stepper.setSpeed(-300); //setting the speed, with negative being towards the home switch. Setting the position in each instance of a loop, makes it travel at a constant speed. If is confusion talk to sam for a shitty explanation
    stepper.run(); //Executes a step in each iteration of the loop. Does math to achieve desired speed and accel
    //Serial.println(stepper.currentPosition());
  }
  stepper.setCurrentPosition(0); //once home, set position to zero for reference
  delay(1000);
}

void gotoPositionAtSpeed(long posmm, float spedmm, float accelmm){ 
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

void gotoPositionAtSpeedsteps(long posmm, float sped, float accel){
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

void trianglemotion (long posmm,float t){ //plans and executes the triangular motion profile to get the carriage to the desired position in the desired amount of time, limited by the max velocity of 4000 steps/s
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

void trapmotion( long posmm, float t){     // this function uses a trapezoidal motion profile with equal times of acceleration, constant speed, and deceleration. from this link: 
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

void gotoangle(int a){
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
}*/

#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Servo.h>

AccelStepper fretting(1,54,55); //using x-step pins
AccelStepper strumming(1,60,61); //using y-step pins
Servo muting;
Servo majorminor;

int strumhome=3;
int frethome=14;

long fret_strokelengthmm = 400; 
double fret_step_per_rev=800; 
double fret_mm_per_rev = 72; 
double fret_mmPerStep = fret_mm_per_rev/fret_step_per_rev;

long strum_strokelengthmm;
double strum_step_per_rev;
double strum_mm_per_rev;
double strum_mmPerStep =  strum_mm_per_rev/strum_step_per_rev;

int majorminor_angle = 45;
int mute_angle = 0;

void setup() { 
  Serial.begin(9600);  
  pinMode(strumhome,INPUT_PULLUP); 
  pinMode(frethome,INPUT_PULLUP);
  majorminor.attach(11);  
  majorminor.write(majorminor_angle);  
  muting.attach(4);
  muting.write(mute_angle);
  stepper.setMaxSpeed(1000000.0); //set arbitrarily high max speed, in units of [pulses/sec] (max reliable is about 4000 according to accelstepper)
  delay(1000); 
  gohome();  
}

void gohome(){
  long fret_strokelength=ceil(fret_strokelengthmm/fret_mmPerStep);
  long strum_strokelength=ceil(strum_strokelengthmm/strum_mmPerStep);

  fretting.moveTo(fret_strokelength);
  strumming.moveTo(strum_strokelength);
  
  
}
  /*long strokelength=ceil(strokelengthmm/mmPerStep); 
  stepper.moveTo(strokelength);  
  while(digitalRead(switch1)==0){  
    stepper.setSpeed(-300); 
    stepper.run(); 
  }
  stepper.setCurrentPosition(0); //once home, set position to zero for reference
  delay(1000);
}

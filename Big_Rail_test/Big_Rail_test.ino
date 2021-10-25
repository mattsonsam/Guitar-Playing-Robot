#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Servo.h>

AccelStepper stepper(1,54,55); //dir then pulse
Servo servo;
long strokelengthmm = 400; //mm
double step_per_rev=800; //default is 1600
double mm_per_rev = 72;
double mmPerStep = mm_per_rev/step_per_rev;
int switch1 = 3;
int switch2 = 9;
int pos=45;
void setup() {
  Serial.begin(9600);
  pinMode(switch1,INPUT_PULLUP);
  pinMode(switch2,INPUT_PULLUP);
  servo.attach(11);
  servo.write(pos);
  stepper.setMaxSpeed(1000000.0);
  delay(1000);
  //gohome();
  //Serial.print("hello");
  
}

void loop() {
//gotoangle(20);
//delay(1000);
//gotoangle(40);
//delay(1000);



 
}
void gohome(){
  long strokelength=ceil(strokelengthmm/mmPerStep);
  stepper.moveTo(strokelength);
  while(digitalRead(switch1)==0){
    stepper.setSpeed(-300);
    stepper.run();
    //Serial.println(stepper.currentPosition());
  }
  stepper.setCurrentPosition(0);
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
  while (servo.read()!=a){
    if(a>servo.read()){
      pos++;
    }
    if(a<servo.read()){
      pos--;
    }
    servo.write(pos);
    delay(15);
  }
}

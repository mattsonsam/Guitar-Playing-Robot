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

int strumPosLeft = 150;
int strumPosRight = 1680;


//---------------------------------------Experimental variables, these may change----------------------------------------------------------------------


//-----------------------------------Declare fretting chord positions, and timing relevent to hard coding songs-----------------------------------------

int E=0; int EPos = 0;//all other positions are based off E to accomodate for any moves in the limit switch
//no E sharp
int F=40+E; int FPos =1;
int Fs=62+E; int FsPos = 2;//the "s" in Fs stands for "sharp"
int G=97+E; int GPos =3;
int Gs=128+E; int GsPos = 4;
int A=155+E; int APos = 5;
int As=181+E; int AsPos = 6;
int B=204+E; int BPos = 7;
//no B sharp
int C=226+E; int CPos = 8;
int Cs=245+E; int CsPos = 9;
int D=268+E; int DPos = 10;
int Ds=289+E; int DsPos = 11;

int chordMatrix[] = {E, F, Fs, G, Gs, A, As, B, C, Cs, D, Ds}; //matrix of all chords, chord position in matrix corresponds to pos variable

int majorminor_time;
int muting_time; //variables representing the time in seconds it takes for the servos to move in either direction (assuming its the same time for both directions)

//-------------------------------------SONG MATRIX-------------------------------------------------------------------------------------------------------
//the song needs to be input once as a string matrix and once as a numerical matrix
/*//HOTEL CALIFORNIA CHORD PROGRESSION
int songMatrixNums[] = {A, E, G, D, F, C, D, E}; //do not include major minor here
char *SongMatrixStrings[]= {"Am", "E", "G", "D", "F", "C", "Dm", "E"};
int numOfNotes = 7;//sizeof(songMatrixNums);
*/
//I CANT HELP FALLING IN LOVE WITH YOU CHORD PROGRESSION
int songMatrixNums[] = {C, G, A, C, G, C, E, A, F, C, G, F, G, A, F, C, G, C, F, G, A, F, C, G, C, E, B, E, B, E, B, E, A, D, G}; //do not include major minor here
char *SongMatrixStrings[]= {"C", "G", "Am", "C", "G", "C", "Em", "Am", "F", "C", "G", "F", "G", "Am", "F", "C", "G", "C", "F", "G", "Am", "F", "C", "G", "C", "Em", "B", "Em", "B", "Em", "B", "Em", "Am", "Dm", "G"};
int numOfNotes = 34;//sizeof(songMatrixNums);

int currentNote;
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
  
  for(int i = 0; i<numOfNotes; i++){
     Serial.print(numOfNotes);
     currentNote = songMatrixNums[i];
     gotochord(currentNote, i, 0.2);
     strum(3000,strumPosRight);
     delay(10);
     strum(3000,strumPosLeft);
     delay(10);
  }

  /*gotochord(E, 5, false); //this wont work anymore bc i changed how gotochord works - amanda
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
  
  for(int i=0; i<=11; i++){ //for loop to loop through all notes, just for testing, not needed to keep
    chord = chordMatrix[i];
    gotochord(chord, 5, temp_majorMinorBool);
    strum(700,strumPosRight);
    strum(700,strumPosLeft);
    if(temp_majorMinorBool == true){
      temp_majorMinorBool = false;
    } else {
      temp_majorMinorBool = true;
      i=i-1;
    }
    
  } */
  
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

void strum(int strumSpeed, int position){
  strumming.setSpeed(strumSpeed);
  strumming.setAcceleration(8000);
  strumming.runToNewPosition(position);
}


void gotochord(long chordAsNum, int posInSongMatrixStrings, float t){ //posInSongMatrixStrings will be the counter in a for loop
  servosChangingFrets();
  bool major = majorMinorBoolean(posInSongMatrixStrings);
  long targetsteps=floor(chordAsNum/fret_mmPerStep); 
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


bool majorMinorBoolean(int currentNotePos){ //returns if the note currently about to be played is major or minor (ex d would return true, dm would return false)
  bool majorMinorBoolValue;
  String currentNote = SongMatrixStrings[currentNotePos];
  Serial.print("Current Note: ");
  Serial.println(currentNote);
  int noteNameLength = currentNote.length(); //store the length(# of characters) in the note name so we can check if the last letter is m for minor, noteNameLength=noteName.length();
  char noteNameLastLetter = currentNote.charAt(noteNameLength); //store the last letter of the note name with noteName.charAt(noteNameLength); use if(noteNameLastLetter == 'm') for major minor
  
  if(noteNameLastLetter == 'm'){
    majorMinorBoolValue = false;
  } else {
    majorMinorBoolValue = true;
  }
  return(majorMinorBoolValue);
}


void hotelcalifornia(){ //hard code hotel california chord progression
  
  
}
//-----------------------------------------END FUNCTIONS--------------------------------------------------------------


//-----------------------------------------EXPERIMENTAL---------------------------------------------------------------

//---------------------------DEFUNCT/NOT IN USE----------------------
//functions that likely are defunction but good to keep around in case we need to reference then later, can be added to a library of old stuff at some point to clean this up
/*
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

void moveFretter(int position){
  fretting.setSpeed(500);
  fretting.setAcceleration(4000);
  fretting.runToNewPosition(position);
  
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
*/

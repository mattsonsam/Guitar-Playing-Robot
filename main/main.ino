//-------------------------------------SETUP VARIABLES----------------------------------------------------------------------------------------------------
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Servo.h>
#include <LiquidCrystal.h>

AccelStepper strumming(1, 54, 55); //using x-step pins
AccelStepper fretting(1, 60, 61); //using y-step pins
Servo muting;
Servo majorminor;

int frethome = 3; //pin for limit switch for fretter
int strumhome = 18; //pin for limit switch for strummer

//LCD displays
int rsPin = 16;
//rw is to ground
int enablePin = 17;
int d4Pin = 23;
int d5Pin = 25;
int d6Pin = 27;
int d7Pin = 29;

LiquidCrystal lcd(rsPin, enablePin, d4Pin, d5Pin, d6Pin, d7Pin);

int yellowButton = 31;
int greenButton = 33;
int blueButton = 35;

long fret_strokelengthmm = 400; //the length of the entire fretting linear rail
double fret_step_per_rev = 200; // steps per revolution of stepper
double fret_mm_per_rev = 72; //linear distance travelled in one rotation of stepper
double fret_mmPerStep = fret_mm_per_rev / fret_step_per_rev; //linear distance travelled in one step of the stepper

long strum_strokelengthmm; //these need to be determined
double strum_step_per_rev = 200;
double strum_mm_per_rev = 72;
double strum_mmPerStep = strum_mm_per_rev / strum_step_per_rev;

int fretHomingSpeed = -200;
int strumHomingSpeed = -200;

int majorminor_down = 30;
int majorminor_up = 0;
//int majorminor_angle = 45;
int mute_down = 35; //this needs to be determined
int mute_up = 60; //this needs to be determined

int strumPosLeft = 0;
int strumPosRight = 83;
int strum_mid = (strumPosRight + strumPosLeft) / 2;
int strumTraversalDistance = strumPosRight - strumPosLeft; //1530 steps for 150 and 1680

double servosChangingFretsTime = 0.5;
double majorminor_time = 0.4;
int muting_time = 0.4; //variables representing the time in seconds it takes for the servos to move in either direction (assuming its the same time for both directions)


int rotationValue = 1; // start at the first song

int numOfSongs = 4; //number of songs we can play, is summed up later


//---------------------------------------Experimental variables, these may change----------------------------------------------------------------------



//-----------------------------------Declare fretting chord positions, and timing relevent to hard coding songs-----------------------------------------

int E = 0; int EPos = 0; //all other positions are based off E to accomodate for any moves in the limit switch
//no E sharp
int F = 38 + E; int FPos = 1;
int Fs = 62 + E; int FsPos = 2; //the "s" in Fs stands for "sharp"
int G = 97 + E; int GPos = 3;
int Gs = 128 + E; int GsPos = 4;
int A = 155 + E; int APos = 5;
int As = 181 + E; int AsPos = 6;
int B = 204 + E; int BPos = 7;
//no B sharp
int C = 226 + E; int CPos = 8;
int Cs = 245 + E; int CsPos = 9;
int D = 268 + E; int DPos = 10;
int Ds = 289 + E; int DsPos = 11;

int chordMatrix[] = {E, F, Fs, G, Gs, A, As, B, C, Cs, D, Ds}; //matrix of all chords, chord position in matrix corresponds to pos variable

//-------------------------------------SONG MATRIX-------------------------------------------------------------------------------------------------------
//the song needs to be input once as a string matrix and once as a numerical matrix

//I CANT HELP FALLING IN LOVE WITH YOU CHORD PROGRESSION
int cant_help_falling[] = {C, E, A, F, C, G, F, G, A, F, C, G, C, C, E, A, F, C, G, F, G, A, F, C, G, C};
int cant_help_falling_majorminor[] = {1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1};
int cant_help_falling_timing[] = {1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1};
const int cant_help_falling_numchords = sizeof(cant_help_falling) / sizeof(cant_help_falling[0]);


//HOTEL CALIFORNIA CHORD PROGRESSION

int hotelcalifornia[] = {B, Fs, A, E, G, D, E, Fs, B, Fs, A, E, G, D, E, Fs, G, D, Fs, B, G, D, E, Fs, B, Fs, A, E, G, D, E, Fs, B, Fs, A, E, G, D, E, Fs, G, D, Fs, B, G, D, E, Fs, B, Fs, A, E, G, D, E, Fs, B, Fs, A, E, G, D, E, Fs};
int hotelcalifornia_majorminor[] = {0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1};
int hotelcalifornia_timing[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; //now, each element in the array represents how many measures the given chord lasts for, rather than the number of the measure at which its struck
const int hotelcalifornia_numchords = sizeof(hotelcalifornia) / (sizeof(hotelcalifornia[0]));

//IRONMAN CHORD PROGRESSION
int ironman[100] = {E, G, G, A, A, C, B, C, B, C, G, G, A, A, E, G, G, A, A, C, B, C, B, C, G, G, A, A}; //{B, D, D,E,E,G,Fs,G,Fs,G,D,D,E,E}
int ironman_majorminor[100] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int ironman_timing[100] = {4, 4, 2, 2, 4, 1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 2, 2, 4, 1, 1, 1, 1, 2, 2, 2, 2, 4}; //{.25,.25,.125,.125,.25,.0625, .0625,.0625,.0625,.125,.125,.125,.125,.25}
const int ironman_numchords = sizeof(ironman) / sizeof(ironman[0]);

int shot_the_sheriff[]={G, C, G, G, C, G, Ds, D, G, Ds, D, G, Ds, D, G, Ds, D, G, Ds, D, G, G, C, G, G,C,G,Ds,D,G,Ds,D,G,Ds,D,G,Ds,D,G,Ds,D,G,G,C,G,G,C,G,Ds,D,G,Ds,D,G,Ds,D,G,Ds,D,G,Ds,D,G,G,C,G,G,C,G};
int shot_the_sheriff_majorminor[]={0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0};
int shot_the_sheriff_timing[]=    {2,2,3,2,2,3,1,1,2,1,1,2,1,1,2,1,1,2,1,1,2,2,2,3,2,2,3,1,1,2,1,1,2,1,1,2,1,1,2,1,1,2,2,2,3,2,2,3,1,1,2,1,1,2,1,1,2,1,1,2,1,1,2,2,2,3,2,2,3};
const int shot_the_sheriff_numchords= sizeof(shot_the_sheriff)/sizeof(shot_the_sheriff[0]);

//------------------------------------SETUP FUNCTION-------------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  pinMode(strumhome, INPUT_PULLUP); //sets two limit switches as inputs with internal resistors
  pinMode(frethome, INPUT_PULLUP);

  pinMode(yellowButton, INPUT_PULLUP);
  pinMode(greenButton, INPUT_PULLUP);
  pinMode(blueButton, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");

  majorminor.attach(11);  //attaches majorminor servo to pin 11
  muting.attach(4);
  servosChangingFrets();
  fretting.setMaxSpeed(1000000.0); //set arbitrarily high max speed, in units of [pulses/sec] (max reliable is about 4000 according to accelstepper).Prevents speed from being limited by code, we will not use this
  strumming.setMaxSpeed(1000000.0);
  delay(1000);

  goHome(fret_strokelengthmm, fret_mmPerStep, fretting, frethome, fretHomingSpeed);
  goHome(strum_strokelengthmm, strum_mmPerStep, strumming, strumhome, strumHomingSpeed);

  Serial.print("Hotel California # chords: "); Serial.println(hotelcalifornia_numchords);
  Serial.print("Falling In Love # chords: "); Serial.println(cant_help_falling_numchords);
  Serial.print("Shot Cher oof: "); Serial.println(shot_the_sheriff_numchords);
  Serial.print("Rotation Value: "); Serial.println(rotationValue);
  Serial.print("Number of Songs: "); Serial.println(numOfSongs);

}

//-------------------------------------END SETUP------------------------------------------------------------------


//-------------------------------------START VOID LOOP------------------------------------------------------------

void loop() { //here is where we will call all our functions
  switch (rotationValue) { //use a switch statement to determine which song to display
    case 1:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hotel California");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("I Can't Help Falling");
      lcd.setCursor(0, 1);
      lcd.print("In Love With You");
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ironman");
      break;
    case 4:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("I Shot the Sheriff");
      break;
    default:
      Serial.println("You done fucked up somehow...");
      break;
  }

  if (digitalRead(yellowButton) == LOW) {
    //yellow button pressed, decrease value
    Serial.println("Yellow Button Pressed");
    Serial.println(rotationValue);
    if (rotationValue == 1) {
      rotationValue = numOfSongs;
      Serial.println("resetting value to 2");
    } else {
      rotationValue = rotationValue - 1;
      Serial.println("Decreasing value");
    }
    Serial.println(rotationValue);
    delay(200);
  }

  if (digitalRead(blueButton) == LOW) {
    Serial.println("Blue Button Pressed");
    Serial.println(rotationValue);
    //blue button pressed, increase value
    if (rotationValue == numOfSongs) {
      rotationValue = 1;
      Serial.println("resetting value to 1");
    } else {
      rotationValue = rotationValue + 1;
      Serial.println("increasing value to 2");
    }
    Serial.println(rotationValue);
    delay(200);
  }

  if (digitalRead(greenButton) == LOW) {
    Serial.println("Green Button Pressed, Playing Song");
    Serial.println(rotationValue);
    switch (rotationValue) { //use a switch statement to determine which song to display
      case 1:
        strumPosLeft = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Playing:");
        lcd.setCursor(0, 1);
        lcd.print("Hotel California");
        playsong(hotelcalifornia, hotelcalifornia_majorminor, hotelcalifornia_timing, 74, 4, hotelcalifornia_numchords);
        break;
      case 2:
        strumPosLeft = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Playing:");
        lcd.setCursor(0, 1);
        lcd.print("Can't Help Falling");
        playsong(cant_help_falling, cant_help_falling_majorminor, cant_help_falling_timing, 100, 3, cant_help_falling_numchords);
        break;
      case 3:
        strumPosLeft = 30;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Playing:");
        lcd.setCursor(0, 1);
        lcd.print("Ironman");
        playIronman(ironman,ironman_majorminor, ironman_timing,600, 4, ironman_numchords);
        break;
      case 4:
        strumPosLeft = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Playing:");
        lcd.setCursor(0, 1);
        lcd.print("I Shot The Sheriff");
        playsong(shot_the_sheriff, shot_the_sheriff_majorminor, shot_the_sheriff_timing, 100, 4, shot_the_sheriff_numchords);
        break;
        
      default:
        Serial.println("You done fucked up somehow...");
        break;
    }
  }

}


//------------------------------------END VOID LOOP---------------------------------------------------------------


//------------------------------------START FUNCTIONS-------------------------------------------------------------
//---servos---
void servosUp() { //both servos up
  majorminor.write(majorminor_up);
  muting.write(mute_up);
}

void servosChangingFrets() {
  majorminor.write(majorminor_up);
  muting.write(mute_down);
}

//---go home----
void goHome(long strokeLengthmm, long mmPerStep, AccelStepper stepper, int limitSwitch, int homingSpeed) {
  long strokelength = ceil(strokeLengthmm / mmPerStep); //calculates number of steps to traverse entire rail
  stepper.moveTo(strokelength);

  while (digitalRead(limitSwitch) == 0) {
    stepper.setSpeed(homingSpeed);
    stepper.run();
    //Serial.println(stepper.currentPosition());
  }
  stepper.setCurrentPosition(0); //once home, set position to zero for reference
  delay(1000);
}


//---strum---
void strum(float strumTime, int positionmm) {
  long target = floor(positionmm / strum_mmPerStep);
  float strumSpeed = target / strumTime;

  strumming.setSpeed(strumSpeed);
  strumming.setAcceleration(10000);
  strumming.runToNewPosition(target);
}


void gotochord(int chordAsNum, bool major, double t) { //posInSongMatrixStrings will be the counter in a for loop
  servosChangingFrets();
  bool is_major = major;
  //bool major = majorMinorBoolean(arrayposition, major_minor_array);
  long targetsteps = floor(chordAsNum / fret_mmPerStep);
  float accel = (4.5 * (fretting.currentPosition() - targetsteps)) / pow(t, 2);
  float v_max = (1.5 * (fretting.currentPosition() - targetsteps)) / t;
  fretting.setSpeed(v_max);
  fretting.setAcceleration(accel);
  fretting.moveTo(targetsteps);
  while (fretting.currentPosition() != fretting.targetPosition()) { //while not at the target position, execute steps
    fretting.run();
  }
  muting.write(mute_up);
  if (is_major == true) {
    majorminor.write(majorminor_down);
  }
  muting.write(mute_up);
}

//----------------PLAY A SONG----------------

void playsong(int songchords[], int song_majorminor[], int songtiming[], int tempo, int time_sig_numerator, int numchords) {
  //---------------------calculate constants and stuff---------------//
  double BPS = tempo / 60; //beats per second
  double SPB = 1 / BPS; //seconds per beat
  double secs_per_measure = time_sig_numerator * SPB; //multiplies the time of each beat by the number of beats in a measure
  Serial.print("Secs per measure: ");  Serial.println(secs_per_measure);
 // if (rotationValue == 3) {
 //   double strum_time = .25;
  //} else {
    double strum_time = (secs_per_measure / 4);
 // }

  Serial.print("strum time in secs: "); Serial.println(strum_time);
  double pauseBetweenStrums = ((secs_per_measure - (2 * strum_time)) / 3);
  Serial.print("pause time in secs: "); Serial.println(pauseBetweenStrums);

  double strum_timemillis = strum_time * 1000;
  double pauseBetweenStrumsMillis = pauseBetweenStrums * 1000;
  Serial.print("Strum time in millis: "); Serial.println(strum_timemillis);
  Serial.print("Pause time in millis: "); Serial.println(pauseBetweenStrumsMillis);

  bool firstChord_mmstate; //false = minor, true = major
  bool nextChord_mmstate; //false=minor, true =major
  //---------------------preparing to play--------------------//
  servosChangingFrets();
  delay(500);
  strum(0.5, strumPosRight); //move the strummer to the right to start
  delay(500);

  if (song_majorminor[0] == 1) {
    firstChord_mmstate = true;
  } else {
    firstChord_mmstate = false;
  }

  gotochord(songchords[0], firstChord_mmstate, 1);
  //----------------------begin playing---------------------//
  for (int i = 0; i < numchords; i++) {
    Serial.println(i); ///************************* it appears to play the first two or three notes, then quickly iterate through i=2 to i=25

    double current_chord_time = songtiming[i] * secs_per_measure; //seconds that current chord lasts for
    double current_chord_timeMillis = current_chord_time * 1000;
    double time_let_ring_millis = current_chord_timeMillis - pauseBetweenStrumsMillis; //chord plays for entire length minus the time of one pause, spends that pause time moving to the next chord

    //double time_let_ring = (1 - transition_ratio) * current_chord_time; //time to let the current chord be played for, 75% of time the current chord plays for
    //int time_let_ring_millis = (time_let_ring * 1000);
    Serial.print("Time to play the chord for aka let ring: "); Serial.println(time_let_ring_millis);

    //double transition_time = (current_chord_time * transition_ratio); //time to let the chords change, 25% of total time , replaced by pauseBetweenStrumsMillis


    Serial.print("Time to play the chord for aka let ring: "); Serial.println(time_let_ring_millis);


    if (song_majorminor[i + 1] == 1) { //determine major minor servo position
      nextChord_mmstate = true;
    } else {
      nextChord_mmstate = false;
    }

    unsigned int timeBeforeStrum = millis();
    Serial.print("Time before strum: "); Serial.println(timeBeforeStrum);


    int timeToCompleteStrumMillis = (strum_timemillis + pauseBetweenStrumsMillis) * 2;
    Serial.print("Time to complete one strum: "); Serial.println(timeToCompleteStrumMillis);

    int numStrumLoops = floor(time_let_ring_millis / timeToCompleteStrumMillis);
    Serial.print("Loops: "); Serial.println(numStrumLoops);
    for (int strumLoops = 0; strumLoops < numStrumLoops; strumLoops++) {
      strum(strum_time, strumPosLeft);
      delay(pauseBetweenStrumsMillis);
      strum(strum_time, strumPosRight);

      if (strumLoops == (numStrumLoops - 1)) {
        delay(0);
      } else {
        delay(pauseBetweenStrumsMillis);
      }
    }

    Serial.println("Completed strum");
    unsigned int timeAfterStrum = millis();
    Serial.print("Time after strum: "); Serial.println(timeAfterStrum);

    int timeSpentStrumming = (timeAfterStrum - timeBeforeStrum);
    Serial.print("Strummed for this many millis: "); Serial.println(timeSpentStrumming);
    double remainingTime = time_let_ring_millis - timeSpentStrumming;

    Serial.print("This much time remaining on chord: "); Serial.println(remainingTime);

    int remainingTimeinSec = remainingTime / 1000;
    gotochord(songchords[i + 1], nextChord_mmstate, remainingTimeinSec);
    Serial.print("Moved to chord: "); Serial.println(chordMatrix[songchords[i + 1]]);
  }

  Serial.println("Completed song");
}


//----------END PLAYING A SONG--------------
//-----------PLAY IRONMAN-------
void playIronman(int songchords[], int song_majorminor[], int songtiming[], int tempo, int time_sig_numerator, int numchords){
  //---------------------calculate constants and stuff---------------//
  double strum_time=0.25; //time to make strummer move across the strings in seconds
  double BPS=tempo/60;  //beats per second
  double SPB=1/BPS; //seconds per beat
  double secs_per_measure= time_sig_numerator*SPB; //multiplies the time of each beat by the number of beats in a measure
  double transition_ratio=0.25; //what fraction of the time dedicated to each chord is given to transitioning to the next chord
  double transition_time;
  int last_chord;
//---------------------preparing to play--------------------//
  servosChangingFrets();
  delay(1000);
  strum(0.5,strumPosRight);
  delay(1000);
  bool first_state=false;
  if(song_majorminor[0]==1){
    first_state=true;
  }
  gotochord(songchords[0],first_state,1);
//----------------------begin playing---------------------//
  for(int i=0; i<numchords; i++){ ////*************************** not sure if num_chords is working
    Serial.println(i); ///************************* it appears to play the first two or three notes, then quickly iterate through i=2 to i=25
    

      
      int next_chord=songchords[i+1];
      bool next_chord_state=false; //false=minor, true =major
      //int current_num_measures= songtiming[i+1]-songtiming[i]; //number of measures that current chord takes up
      int current_num_measures=songtiming[i];
      double current_chord_time= current_num_measures*secs_per_measure; //seconds that current chord takes up
      if(song_majorminor[i+1]==1){
        next_chord_state=true;
      }
      double time_let_ring=(1-transition_ratio)*current_chord_time; //time to let the current chord be played for

      /*if(next_chord_state=true){
        transition_time= (current_chord_time*transition_ratio)-(servosChangingFretsTime+muting_time+majorminor_time);
      }
      if(next_chord_state=false){
        transition_time= (current_chord_time*transition_ratio)-(servosChangingFretsTime+muting_time);
      }*/
      transition_time=(current_chord_time*transition_ratio);
     

      if(i%2==0){ // assumes that strummer starts at strumPosRight
        strum(strum_time, strumPosLeft);
      }
      if(i%2!=0){
        strum(strum_time,strumPosRight);
      }
      

      delay((time_let_ring-(strum_time))*1000); //let the chord ring out

      gotochord(next_chord,next_chord_state,transition_time);

      last_chord=i+1;
    
  }
  if((last_chord%2==0)&&(strumming.currentPosition()>strum_mid)){ // assumes that strummer starts at strumPosRight
    strum(strum_time, strumPosLeft);
  }
  if((last_chord%2!=0)&&(strumming.currentPosition()<strum_mid)){
    strum(strum_time,strumPosRight);
  }
}

//-----------------------------------------END FUNCTIONS--------------------------------------------------------------

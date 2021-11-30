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
int numOfSongs = 2; //number of songs we can play

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
int cant_help_falling[100] = {C, E, A, F, C, G, F, G, A, F, C, G, C, C, E, A, F, C, G, F, G, A, F, C, G, C};
int cant_help_falling_majorminor[100] = {1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1};
//int cant_help_falling_timing[100]= {1,2,3,5,6,7,9,10,11,12,13,14,15,17,18,19,21,22,23,25,26,27,28,29,30,31};
int cant_help_falling_timing[100] = {1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1};
const int cant_help_falling_numchords = sizeof(cant_help_falling) / sizeof(cant_help_falling[0]);

int hotelcalifornia[100] = {B, Fs, A, E, G, D, E, Fs, B, Fs, A, E, G, D, E, Fs, G, D, Fs, B, G, D, E, Fs, B, Fs, A, E, G, D, E, Fs, B, Fs, A, E, G, D, E, Fs, G, D, Fs, B, G, D, E, Fs, B, Fs, A, E, G, D, E, Fs, B, Fs, A, E, G, D, E, Fs};
int hotelcalifornia_majorminor[100] = {0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1};
int hotelcalifornia_timing[100] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; //now, each element in the array represents how many measures the given chord lasts for, rather than the number of the measure at which its struck
const int hotelcalifornia_numchords = sizeof(hotelcalifornia) / (sizeof(hotelcalifornia[0]));


/*String cant_help_falling_majorminor[]={"C","Em","Am","F","C","G","F","G","Am","F","C","G","C","C","Em","Am","F","C","G","F","G","Am","F","C","G","C"};


  /*int songMatrixNums[] = {C, G, A, C, G, C, E, A, F, C, G, F, G, A, F, C, G, C, F, G, A, F, C, G, C, E, B, E, B, E, B, E, A, D, G}; //do not include major minor here
  char *SongMatrixStrings[] = {"C", "G", "Am", "C", "G", "C", "Em", "Am", "F", "C", "G", "F", "G", "Am", "F", "C", "G", "C", "F", "G", "Am", "F", "C", "G", "C", "Em", "B", "Em", "B", "Em", "B", "Em", "Am", "Dm", "G"};
  int measures[]={1,2,3,5,6,7,9,10,11,12,13,14,15,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33};*/


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
  lcd.print("Loading...");

  majorminor.attach(11);  //attaches majorminor servo to pin 11
  muting.attach(4);
  servosChangingFrets();
  fretting.setMaxSpeed(1000000.0); //set arbitrarily high max speed, in units of [pulses/sec] (max reliable is about 4000 according to accelstepper).Prevents speed from being limited by code, we will not use this
  strumming.setMaxSpeed(1000000.0);
  delay(1000);

  goHome(fret_strokelengthmm, fret_mmPerStep, fretting, frethome, fretHomingSpeed);
  goHome(strum_strokelengthmm, strum_mmPerStep, strumming, strumhome, strumHomingSpeed);

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
    delay(150);
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
    delay(150);
  }

  if (digitalRead(greenButton) == LOW) {
    Serial.println("Green Button Pressed, Playing Song");
    Serial.println(rotationValue);
    switch (rotationValue) { //use a switch statement to determine which song to display
      case 1:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Playing:");
        lcd.setCursor(0, 1);
        lcd.print("Hotel California");
        playsong(hotelcalifornia, hotelcalifornia_majorminor, hotelcalifornia_timing, 74, 4, hotelcalifornia_numchords);

        break;
      case 2:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Playing:");
        lcd.setCursor(0, 1);
        lcd.print("Can't Help Falling");
        playsong(cant_help_falling, cant_help_falling_majorminor, cant_help_falling_timing, 100, 3, cant_help_falling_numchords);
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

void playsong(int songchords[], int song_majorminor[], int songtiming[], int tempo, int time_sig_numerator, int numchords) {
  //---------------------calculate constants and stuff---------------//
  double strum_time = 0.75; //time to make strummer move across the strings in seconds
  double BPS = tempo / 60; //beats per second
  double SPB = 1 / BPS; //seconds per beat
  double secs_per_measure = time_sig_numerator * SPB; //multiplies the time of each beat by the number of beats in a measure
  double transition_ratio = 0.25; //what fraction of the time dedicated to each chord is given to transitioning to the next chord
  double transition_time;
  int last_chord;
  //---------------------preparing to play--------------------//
  servosChangingFrets();
  delay(1000);
  strum(0.5, strumPosRight);
  delay(1000);
  bool first_state = false;
  if (song_majorminor[0] == 1) {
    first_state = true;
  }
  gotochord(songchords[0], first_state, 1);
  //----------------------begin playing---------------------//
  for (int i = 0; i < numchords; i++) { ////*************************** not sure if num_chords is working
    Serial.println(i); ///************************* it appears to play the first two or three notes, then quickly iterate through i=2 to i=25



    int next_chord = songchords[i + 1];
    bool next_chord_state = false; //false=minor, true =major
    //int current_num_measures= songtiming[i+1]-songtiming[i]; //number of measures that current chord takes up
    int current_num_measures = songtiming[i];
    double current_chord_time = current_num_measures * secs_per_measure; //seconds that current chord takes up
    if (song_majorminor[i + 1] == 1) {
      next_chord_state = true;
    }
    double time_let_ring = (1 - transition_ratio) * current_chord_time; //time to let the current chord be played for

    /*if(next_chord_state=true){
      transition_time= (current_chord_time*transition_ratio)-(servosChangingFretsTime+muting_time+majorminor_time);
      }
      if(next_chord_state=false){
      transition_time= (current_chord_time*transition_ratio)-(servosChangingFretsTime+muting_time);
      }*/
    transition_time = (current_chord_time * transition_ratio);


    if ((i % 2 == 0) && (strumming.currentPosition() > strum_mid)) { // assumes that strummer starts at strumPosRight
      strum(strum_time, strumPosLeft);
    }
    if ((i % 2 != 0) && (strumming.currentPosition() < strum_mid)) {
      strum(strum_time, strumPosRight);
    }


    delay((time_let_ring - (strum_time)) * 1000); //let the chord ring out

    gotochord(next_chord, next_chord_state, transition_time);

    last_chord = i + 1;

  }
  if ((last_chord % 2 == 0) && (strumming.currentPosition() > strum_mid)) { // assumes that strummer starts at strumPosRight
    strum(strum_time, strumPosLeft);
  }
  if ((last_chord % 2 != 0) && (strumming.currentPosition() < strum_mid)) {
    strum(strum_time, strumPosRight);
  }
}


/*bool majorMinorBoolean(int currentNotePos,String major_minor_array) { //returns if the note currently about to be played is major or minor (ex d would return true, dm would return false)
  bool majorMinorBoolValue;
  String currentNote = major_minor_array[currentNotePos];
  int noteNameLength = currentNote.length(); //store the length(# of characters) in the note name so we can check if the last letter is m for minor, noteNameLength=noteName.length();
  char noteNameLastLetter = currentNote.charAt(noteNameLength); //store the last letter of the note name with noteName.charAt(noteNameLength); use if(noteNameLastLetter == 'm') for major minor

  if (noteNameLastLetter == 'm') {
    majorMinorBoolValue = false;
  } else {
    majorMinorBoolValue = true;
  }
  return (majorMinorBoolValue);
  }*/



//-----------------------------------------END FUNCTIONS--------------------------------------------------------------

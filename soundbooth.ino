// youtube demo link - checkpoint 1: https://youtu.be/PANbVQ5PNMc
// youtube demo link - checkpoint 2: https://youtu.be/8gxXdXVgVvs
// youtube demo link - final Submission: https://youtu.be/1ZH6zH03xgg 

#include "Timer.h"
#include "Adafruit_LiquidCrystal.h"
// Depending on the LiquidCrystal library you are able to install, it might be:
// #include "LiquidCrystal.h"
#include "pitches.h"

// Sound Variables  
int buzzer = 8;

// == Song 1 ==
int song1[] = {
NOTE_E4, NOTE_C5, NOTE_B1, NOTE_F3, NOTE_C4, 
NOTE_A4, NOTE_A4, NOTE_GS5, NOTE_C5, NOTE_CS4, 
NOTE_AS4, NOTE_C5, NOTE_DS4, NOTE_CS5, NOTE_GS4, 
NOTE_C3, NOTE_E3, NOTE_DS5, NOTE_D4, NOTE_D3
};
int song1_time[] = {
2, 1, 2, 1, 1, 4, 8, 16, 8, 4, 4, 1, 8, 4, 2, 4, 4, 16, 4, 2
};

// == Song 2 ==

int song2[] = {
  NOTE_FS5, NOTE_D2, NOTE_DS5, NOTE_G2, NOTE_B3, 
  NOTE_C2, NOTE_G5, NOTE_D6, NOTE_CS5, NOTE_AS4, 
  NOTE_DS6, NOTE_D3, NOTE_CS4, NOTE_E5, NOTE_DS6,
   NOTE_E4, NOTE_B4, NOTE_F4, NOTE_E6, NOTE_DS4
};

int song2_time[] = {
  2, 2, 4, 8, 1, 8, 4, 4, 16, 8, 2, 4, 16, 8, 2, 4, 16, 4, 8, 1
};

// == Song 3 == 

int song3[] = {
  NOTE_A5, NOTE_D4, NOTE_D6, NOTE_DS3, NOTE_G4, 
  NOTE_B2, NOTE_F2, NOTE_A3, NOTE_AS2, NOTE_B5, 
  NOTE_C6, NOTE_C3, NOTE_GS3, NOTE_G2, NOTE_FS5, 
  NOTE_AS4, NOTE_GS2, NOTE_CS3, NOTE_C3, NOTE_AS2
};

int song3_time[] = {
  1, 2, 16, 4, 16, 2, 16, 1, 1, 2, 1, 8, 2, 16, 8, 1, 16, 4, 1, 2
};


// LCD variables
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
Adafruit_LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
// Depending on the LiquidCrystal library you are able to install, it might be:
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



// Task Structure Definition
typedef struct task {
   int state;                  // Tasks current state
   unsigned long period;       // Task period
   unsigned long elapsedTime;  // Time elapsed since last task tick
   int (*TickFct)(int);        // Task tick function
} task;


const unsigned char tasksNum = 4;
task tasks[tasksNum]; // We have 4 tasks

// Task Periods

const unsigned long periodLCDOutput = 100;
const unsigned long periodJoystickInput = 100;
const unsigned long periodSoundOutput = 100;
const unsigned long periodController = 200;

// Controller variables
int cursorFlag = 1;
int songSelectFlag = 0; 
int startFlag = 0; 
int sw = 10;
int soundStartFlag = 0; 
int songStartedPausePlayFlag = 0; 
int pauseTheSongFlag = 0; 
int playTheSongFlag = 0;

// Sound output variables 
int counter = 0;
int note = 0;
int lengthCnt = 0; 
int finishedSongFlag = 0;
int tempFlag = 0; 
int finishedSongControllerFlag = 0;


// GCD 
const unsigned long tasksPeriodGCD = 100;

// Task Function Definitions
int TickFct_LCDOutput(int state);
int TickFct_JoystickInput(int state);
int TickFct_SoundOutput(int state);
int TickFct_HomeScreen(int state);

// Task Enumeration Definitions
enum LO_States {LO_init, LO_Home, LO_Hold, LO_S1, LO_S2, LO_S3};
enum JI_States {JI_init, JI_Sample};
enum SO_States {SO_init, SO_SoundOn1, SO_SoundOn2, SO_SoundOn3, SO_SoundOff1, SO_SoundOff2, SO_SoundOff3, Off};
enum C_States {C_init, C_S1, C_S2, C_S3, C_Start, C_On, C_Off, C_S1Pause, C_S1Play, C_PlayingOn, C_PlayingOff};



void TimerISR() { // TimerISR actually defined here
  unsigned char i;
  for (i = 0; i < tasksNum; ++i) { // Heart of the scheduler code
     if ( tasks[i].elapsedTime >= tasks[i].period ) { // Ready
        tasks[i].state = tasks[i].TickFct(tasks[i].state);
        tasks[i].elapsedTime = 0;
     }
     tasks[i].elapsedTime += tasksPeriodGCD;
  }
}


void LCDWriteLines(String line1, String line2) {
  lcd.clear();          
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}

// Task Function Definitions

int menuOption = 0;

// Task 1
int TickFct_LCDOutput(int state) {
  switch (state) { // State Transitions
    case LO_init:
      state = LO_Home;
      break;
    case LO_Home:
      state = LO_Hold;
      counter = 0;
      note = 0;
      lengthCnt = 0; 
      soundStartFlag = 0; 
      tempFlag = 0; 
      break;
    case LO_Hold:
      if(startFlag == 1){
        state = LO_S1;
        soundStartFlag = 1;
        startFlag = 0;
      }
      else if(startFlag == 2){
        state = LO_S2;
        soundStartFlag = 2;
        startFlag = 0; 
      }
      else if(startFlag == 3){
        state = LO_S3;
        soundStartFlag = 3;
        startFlag = 0;
      }
      else if(finishedSongFlag == 1){
        finishedSongFlag = 0; 
        state = LO_Home;
      }
      break;
    case LO_S1:
      state = LO_Hold;
      break;
    case LO_S2:
      state = LO_Hold;
      break;
    case LO_S3:
      state = LO_Hold;
      break;
  }

  switch (state) { // State Actions
    case LO_Home:
      LCDWriteLines("Song 1  Song 2", "Song 3  Start");
      finishedSongFlag = 0;
      Serial.println("Home");
      break;
    case LO_S1:
      LCDWriteLines("Playing  Song 1", "Pause    Play");
      Serial.println("S1");
      break;
    case LO_S2:
      LCDWriteLines("Playing  Song 2", "Pause    Play");
      Serial.println("S2");
      break;
    case LO_S3:
      LCDWriteLines("Playing  Song 3", "Pause    Play");
      Serial.println("S3");
      break;
    case LO_Hold:
      break;
  }
  return state;
}
// Task 2
int TickFct_JoystickInput(int state) {
  switch (state) { // State Transitions
    case JI_init:
      state = JI_Sample;
      break;
    case JI_Sample:
      break;
  }

   switch (state) { // State Actions
    case JI_Sample:
      int inputY = analogRead(A0);
      int inputX = analogRead(A1);
      if (inputX < 200) {
        menuOption = 1; // up
      }
      else if (inputX > 800) {
        menuOption = 2; // down
      }
      else if(inputY > 800){
        menuOption = 3; // left
      }
      else if(inputY < 200){
        menuOption = 4; // right
      }
      else {
        menuOption = 0;
      }
      break;
  }
  return state;
}

// Sound Output
int TickFct_SoundOutput(int state) {
  switch (state) { // State Transitions
    case SO_init:
      state = Off;
      break;
    case SO_SoundOn1:
      if(soundStartFlag == 1 && lengthCnt < 20){
        if(counter >= song1_time[note]) {
          state = SO_SoundOff1;
          if(pauseTheSongFlag == 0){
            note++;
            lengthCnt++;
            counter = 0;
            note = note % 20;
          }
        }
      }
      else {
        state = Off;
      }
      break;
    case SO_SoundOn2:
      if(soundStartFlag == 2 && lengthCnt < 20){
        if(counter >= song2_time[note]) {
          state = SO_SoundOff2;
          if(pauseTheSongFlag == 0){
            note++;
            lengthCnt++;
            counter = 0;
            note = note % 20;
          }
        }
      }
      else {
        state = Off;
      }
      break;
    case SO_SoundOn3:
      if(soundStartFlag == 3 && lengthCnt < 20){
        if(counter >= song3_time[note]) {
          state = SO_SoundOff3;
          if(pauseTheSongFlag == 0){
            note++;
            lengthCnt++;
            counter = 0;
            note = note % 20;
          }
        }
      }
      else {
        state = Off;
      }
      break;
    case SO_SoundOff1:
      state = SO_SoundOn1;
      break;
    case SO_SoundOff2:
      state = SO_SoundOn2;
      break;
    case SO_SoundOff3:
      state = SO_SoundOn3;
      break;
    case Off:
      if(soundStartFlag == 1 && lengthCnt < 20){
        state = SO_SoundOn1;
      }
      else if(soundStartFlag == 2){
        state = SO_SoundOn2;
      }
      else if(soundStartFlag == 3){
        state = SO_SoundOn3;
      }
      if(lengthCnt >= 20){
        if(tempFlag == 0){
          finishedSongFlag = 1; 
          finishedSongControllerFlag = 1;
          startFlag = 0;
          tempFlag = 1;
        }
      }
      break;

  }
   switch (state) { // State Actions
    case SO_SoundOn1:
      if(pauseTheSongFlag == 0){
        tone(buzzer, song1[note], periodSoundOutput* song1_time[note]);
        counter++;
      }
      break;
    case SO_SoundOn2:
      if(pauseTheSongFlag == 0){
        tone(buzzer, song2[note], periodSoundOutput* song2_time[note]);
        counter++;
      }
      break;
    case SO_SoundOn3:
      if(pauseTheSongFlag == 0){
        tone(buzzer, song3[note], periodSoundOutput* song3_time[note]);
        counter++;
      }
      break;
    case SO_SoundOff1:
      noTone(buzzer);
      break;
    case SO_SoundOff2:
      noTone(buzzer);
      break;
    case SO_SoundOff3:
      noTone(buzzer);
      break;
    case Off:
      noTone(buzzer);
      break;

  }
  return state;
}


// Task 4 (Unused)
int TickFct_HomeScreen(int state) {
  switch (state) { // State Transitions
    case C_init:
      state = C_S1;
      break;
    case C_S1:
      if(menuOption == 2){ // down
        state = C_S3;
        cursorFlag = 3;
      }
      else if(menuOption == 4){ // right
        state = C_S2;
        cursorFlag = 2;
      }
      else if(menuOption == 0){ // no movement
        state = C_On;
        cursorFlag = 1;
      }
      break;
    case C_S2:
      if(menuOption == 2){ // down
        state = C_Start;
        cursorFlag = 4;
      }
      else if(menuOption == 3){ // left
        state = C_S1;
        cursorFlag = 1;
      }
      else if(menuOption == 0){ // no movement
        state = C_On;
        cursorFlag = 2;
      }
      break;
    case C_S3:
      if(menuOption == 1){ // up
        state = C_S1;
        cursorFlag = 1;
      } 
      else if (menuOption == 4){ // right
        state = C_Start;
        cursorFlag = 4;
      }
      else if(menuOption == 0){ // no movement
        state = C_On;
        cursorFlag = 3;
      }
      break;
    case C_Start:
      if(menuOption == 3){ // left
        state = C_S3;
        cursorFlag = 3;
      }
      else if(menuOption == 1){ // up
        state = C_S2;
        cursorFlag = 2;
      }
      else if(menuOption == 0){ // no movement
        state = C_On;
        cursorFlag = 4;
      }
      break;
    case C_On:
      if(menuOption == 0){
        state = C_Off;
      }
      else if(menuOption != 0){
        if(cursorFlag == 1){
          state = C_S1;
        }
        else if(cursorFlag == 2){
          state = C_S2;
        }
        else if(cursorFlag == 3){
          state = C_S3;
        }
        else if(cursorFlag == 4){
          state = C_Start;
        }
      }
      if(songStartedPausePlayFlag == 1){
        state = C_S1Pause;
      }
      break;
    case C_Off:
      if(menuOption == 0){
        state = C_On;
      }
      else if(menuOption != 0){
        if(cursorFlag == 1){
          state = C_S1;
        }
        else if(cursorFlag == 2){
          state = C_S2;
        }
        else if(cursorFlag == 3){
          state = C_S3;
        }
        else if(cursorFlag == 4){
          state = C_Start;
        }
      }
      break;
    case C_S1Pause: 
      if(finishedSongFlag == 0){
        if(menuOption == 4){
          state = C_S1Play;
        }
        else if(menuOption == 0){
          state = C_PlayingOn;  
        }
      }
      else{
        state = C_On;
      }
      break;
    case C_S1Play:
      if(finishedSongFlag == 0){
        if(menuOption == 3){
          state = C_S1Pause;
        }
        else if(menuOption == 0){
          state = C_PlayingOn;
        }
      }
      else{
        state = C_On;
      }
      break;
    case C_PlayingOn:
      songStartedPausePlayFlag = 0;
      if(finishedSongControllerFlag == 0){
        if(menuOption == 3){
          state = C_S1Pause;
        }
        else if(menuOption == 4){
          state = C_S1Play;
        }
        else if(menuOption == 0){
          state = C_PlayingOff;
        }
      }
      else if (finishedSongControllerFlag == 1){
        state = C_On;
        finishedSongControllerFlag = 0;
      }
      break;
    case C_PlayingOff:
      if(finishedSongControllerFlag == 0){
        state = C_PlayingOn;
      }
      else if(finishedSongControllerFlag == 1){
        state = C_On;
        finishedSongControllerFlag = 0;
      }
      break;
  }

   switch (state) { // State Actions
    case C_S1:
      lcd.setCursor(6, 0);
      break;
    case C_S2:
      lcd.setCursor(7, 0);
      break;
    case C_S3:
      lcd.setCursor(6, 1);
      break;
    case C_Start:
      lcd.setCursor(7, 1);
      break;
    case C_On:
      lcd.blink();
      if((digitalRead(sw) == 0) && (cursorFlag == 1)){
        if(songSelectFlag == 1){
          lcd.setCursor(6, 0);
          lcd.print(" ");
          songSelectFlag = 0; 
          Serial.println("1 deselected");
        }
        else {
          lcd.print("*");
          songSelectFlag = 1;
          Serial.println("1 selected");
        }
      }
      if((digitalRead(sw) == 0) && (cursorFlag == 2)){
        if(songSelectFlag == 2){
          lcd.setCursor(7, 0);
          lcd.print(" ");
          songSelectFlag = 0; 
          Serial.println("2 deselected");
        }
        else {
          lcd.print("*");
          songSelectFlag = 2;
          Serial.println("2 selected");
        }
      }
      if((digitalRead(sw) == 0) && (cursorFlag == 3)){
        if(songSelectFlag == 3){
          lcd.setCursor(6, 1);
          lcd.print(" ");
          songSelectFlag = 0; 
          Serial.println("3 deselected");
        }
        else {
          lcd.print("*");
          songSelectFlag = 3;
          Serial.println("3 selected");
        }
      }
      if((digitalRead(sw) == 0) && (cursorFlag == 4)){
        songStartedPausePlayFlag = 1; 
        if(songSelectFlag == 1){
          startFlag = 1; 
          Serial.println("Started 1");
        }
        else if(songSelectFlag == 2){
          startFlag = 2; 
          Serial.println("Started 2");
        }
        else if(songSelectFlag == 3){
          startFlag = 3; 
          Serial.println("Started 3");
        }
      }
      break;
    case C_Off:
      lcd.noBlink();
      break;
    case C_S1Pause:
      lcd.setCursor(6, 1);
      Serial.println("Pause");
      break;
    case C_S1Play:
      lcd.setCursor(7, 1);
      Serial.println("Play");
      break;
    case C_PlayingOn:
      lcd.blink();
      if(digitalRead(sw) == 0 && pauseTheSongFlag == 0){
        pauseTheSongFlag = 1;
        lcd.setCursor(7,1);
        lcd.print(" "); 
        lcd.setCursor(6,1);
        lcd.print("*");
        Serial.println("Paused song");
      }
      else if(digitalRead(sw) == 0 && pauseTheSongFlag == 1){
        lcd.setCursor(6,1);
        lcd.print(" ");
        lcd.setCursor(7,1);
        lcd.print("*");
        pauseTheSongFlag = 0;
        Serial.println("Played song");
      }
      break;
    case C_PlayingOff:
      lcd.noBlink();
      break;

  }
  return state;
}



void InitializeTasks() {
  unsigned char i=0;
  tasks[i].state = LO_init;
  tasks[i].period = periodLCDOutput;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_LCDOutput;
  ++i;
  tasks[i].state = JI_init;
  tasks[i].period = periodJoystickInput;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_JoystickInput;
  ++i;
  tasks[i].state = SO_init;
  tasks[i].period = periodSoundOutput;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_SoundOutput;
  ++i;
  tasks[i].state = C_init;
  tasks[i].period = periodController;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_HomeScreen;

}

void setup() {
  // put your setup code here, to run once:
  InitializeTasks();

  TimerSet(tasksPeriodGCD);
  TimerOn();
  Serial.begin(9600);
  // Initialize Outputs
  lcd.begin(16, 2);
  // Initialize Inputs
  pinMode(10, INPUT_PULLUP);
  // added so astericks might work

}

void loop() {
  // put your main code here, to run repeatedly:
  // Task Scheduler with Timer.h
  
}

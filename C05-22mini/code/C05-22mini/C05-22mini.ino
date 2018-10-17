#include "LedControl.h"
#include <Servo.h>

//C05 - 22mini
//(tutumini)
//Diseñada por Colectivo 22bits (http://22bits.org)
//Bárbara Molina & Matías Serrano
//Programada por Elías Zacarías (http://blog.damnsoft.org/)

// 22machines, por Bárbara Molina & Matías Serrano se distribuye bajo una Licencia Creative Commons Atribución-NoComercial-CompartirIgual 4.0 Internacional.
// Colectivo 22bits promueve cualquier tipo de modificación y hackeo a la máquina, no así el uso comercial que se le pueda dar sin expresa autorización de sus creadores. 
// Si te fabricas una, la utilizas en tu set, o la usas para algo nuevo, haznos saber y te compartiremos (si te parece) :)


// Comment the following line if the whole hardware is available (Clock In, Out, Speed control).
// #define MINIMAL_HARDWARE
// Comment the following line to disable servo output and have a standard pulse instead.
#define SERVO_OUTPUT
// This setting optionally inverts the output when SERVO_OUTPUT is not defined
// #define INVERTED_OUTPUT
// Uncomment the following line for a 22 mini
#define _22_MINI

// *******************************************************************************
// **                                                                           **
// **        C O N S T A N T S   &   P I N / B U T T O N   M A P P I N G        **
// **                                                                           **
// *******************************************************************************
#ifdef _22_MINI
const byte TOTAL_BUTTONS          = 8; // Connected to 4021
const byte TOTAL_STEPS            = 8;
const byte TOTAL_TRACKS           = 2;
const byte LED_MATRIX_BRIGHTNESS  = 1;
#else
const byte TOTAL_BUTTONS          = 16;
const byte TOTAL_STEPS            = 16;
const byte TOTAL_TRACKS           = 4;
#endif

const byte TOTAL_STEP_COLS        = TOTAL_STEPS/2;
const int  MIN_STEP_TIME          = 100;
const int  MAX_STEP_TIME          = 1000;
const int  RESET_PRESS_TIME       = 1000;
const byte CLK_SIGNAL_TIME        = 15;

#ifdef SERVO_OUTPUT
const byte HIT_SERVO_ANGLE        = 45;
const byte SERVO_START_ANGLE       = 5; // Never set to 0. May be outside of the range of some servos!
const byte SERVO_DELAY            = 50;
const byte SERVO_UPDATE_INT       = 10;
const byte SERVO_INCREMENT        = 4;
#endif

// Pin mapping ----------------------
#define bttn_latchPin         8
#define bttn_dataPin          9
#define bttn_clockPin         7
#define matrix_data           6
#define matrix_clock          5
#define matrix_load           4

#define speed_analog_in       A0
#define sync_clock_in         A2
#define sync_clock_out        A3
#define mtx_intensity_in      A1

// Output pins ----------------------
#ifdef _22_MINI
const int outputPin[TOTAL_TRACKS]        = { 13, 12};
#else
const int outputPin[TOTAL_TRACKS]        = { 13, 12, 11, 10};
#endif

// Button mapping (4021) ------------
#ifdef _22_MINI
const byte step_buttons[TOTAL_STEP_COLS] = { 1, 2, 3, 5};
const byte track_buttons[TOTAL_TRACKS]   = { 0, 4};
const byte MUTE_BUTTON                   = 7;
const byte PAGE_SEL_BUTTON               = 6;
const byte PLAY_BUTTON_PIN               = 2;
#else
const byte step_buttons[TOTAL_STEP_COLS] = { 9, 10, 11, 7, 6, 5, 4, 0};
const byte track_buttons[TOTAL_TRACKS]   = { 8, 12, 13, 14};
const byte PLAY_BUTTON                   = 2;
const byte PAGE_SEL_BUTTON               = 1;
const byte MUTE_BUTTON                   = 15;
#endif
//                            G  C  E  DP D  F [A, B (Unconnected)]
const byte LED_ROW_REMAP[] = {7, 3, 5, 0, 4, 6, 1, 2};
//                           D1  D5 D7 D3 D2 D7 D4 D0
const byte LED_COL_REMAP[] = {1, 5, 7, 3, 2, 6, 4, 0};

const byte OFFBEAT_LED_ROW = 6; // SEG_A: Check previous mapping.
const byte OFFBEAT_LED_COL = 0; // DIG_1: Check previous mapping.

#ifdef _22_MINI
const byte REF_LED_ROW = 4;
#else
const byte REF_LED_ROW = TOTAL_TRACKS;
#endif

// *******************************************************************************
// **                                                                           **
// **                      D A T A   D E C L A R A T I O N S                    **
// **                                                                           **
// *******************************************************************************
const byte TOTAL_BBYTES   = (TOTAL_BUTTONS + (8 - (TOTAL_BUTTONS % 8))) / 8;
byte buttonData[TOTAL_BBYTES];
byte prevBttnData[TOTAL_BBYTES];
#ifdef _22_MINI
boolean miniPlayBttnData = false;
boolean miniPlayBttnPrevData = false;
#endif

word stepData[TOTAL_TRACKS];
bool isTrackActive[TOTAL_TRACKS];
bool isStepActive[TOTAL_STEPS];

volatile int            stepInterval;
volatile byte           currentStep = TOTAL_STEPS-1;
volatile word           msCounter;
volatile byte           currentTrack = 0;
volatile boolean        playing = true;
volatile byte           currentSequencePage = 0;
volatile unsigned long  lastClockIn;
volatile word           extClockInterval;
LedControl ledMatrix  = LedControl (matrix_data, matrix_clock, matrix_load, 1);

// *******************************************************************************
// **                                                                           **
// **                        B U T T O N   H A N D L I N G                      **
// **                                                                           **
// *******************************************************************************
boolean getButtonPressed (byte b) {
  if (b >= TOTAL_BUTTONS) return false;
  return buttonData[b / 8] & (byte)(1 << (b % 8));
}

boolean getButtonPrevPressed (byte b) {
  if (b >= TOTAL_BUTTONS) return false;
  return prevBttnData[b / 8] & (byte)(1 << (b % 8));
}

boolean getButtonHit (byte b){
  return getButtonPressed(b) && !(getButtonPrevPressed(b));
}

boolean getButtonReleased (byte b){
  return !getButtonPressed(b) && (getButtonPrevPressed(b));
}

boolean getPlayButtonHit (){
#ifdef _22_MINI
  return miniPlayBttnData && !miniPlayBttnPrevData;
#else
  getButtonHit(PLAY_BUTTON);
#endif
}

boolean getPlayButtonPressed (){
#ifdef _22_MINI
  return miniPlayBttnData;
#else
  getButtonPressed(PLAY_BUTTON);
#endif
}

boolean getPlayButtonReleased (){
#ifdef _22_MINI
  return !miniPlayBttnData && miniPlayBttnPrevData;
#else
  getButtonReleased(PLAY_BUTTON);
#endif
}

void setButton (byte b, boolean val) {
  if (b >= TOTAL_BUTTONS) return;
  if (val) {
    buttonData[b / 8] |= (byte)(1 << (b % 8));
  }else {
    buttonData[b / 8] &= ~(byte)(1 << (b % 8));
  }
}

void readButtons() { 
  byte i, b;

  // Keep a record of the previous state of every button
  for (int b =  0; b < TOTAL_BBYTES;  b++) prevBttnData[b] = buttonData[b];
#ifdef _22_MINI
  miniPlayBttnPrevData = miniPlayBttnData;
  miniPlayBttnData = digitalRead(PLAY_BUTTON_PIN);
#endif

  //Pulse the latch pin:
  digitalWrite(bttn_latchPin, 1);
  delayMicroseconds(20);
  digitalWrite(bttn_latchPin, 0);

  for (i=0; i<TOTAL_BUTTONS; i++){
    digitalWrite(bttn_clockPin, 0);
    setButton (i, digitalRead(bttn_dataPin)); 
    digitalWrite(bttn_clockPin, 1);
  }
}

// *******************************************************************************
// **                                                                           **
// **                    S T E P   D A T A   H A N D L I N G                    **
// **                                                                           **
// *******************************************************************************
void enableAllSteps(bool enabled){
	for (byte s =  0; s < TOTAL_STEPS;  s++) isStepActive[s] = enabled;
}


boolean getStepData (byte track, byte stepN){
  if (track >= TOTAL_TRACKS || stepN > TOTAL_STEPS) return false;
  return stepData[track] & (word)(1 << stepN);
}

void setStepData (byte track, byte stepN, boolean val){
  if (track >= TOTAL_TRACKS || stepN > TOTAL_STEPS) return;
  if (val) {
    stepData[track] |= (word)(1 << stepN);
  }else {
    stepData[track] &= ~(word)(1 << stepN);
  }
}

void resetSteps (){
  for (byte t =  0; t < TOTAL_TRACKS;  t++) stepData[t] = 0;
}

void toggleStepData (byte track, byte stepN){
  boolean curVal = getStepData (track, stepN);
  setStepData (track, stepN, !curVal);
}

void toggleStepEnabled (byte stepN){
  isStepActive[stepN] = !isStepActive[stepN];
}

byte nextEnabledStep (byte fromStep){
  byte st;
  for (byte b = 1; b < TOTAL_STEPS; b++) {
    st = (fromStep + b) % TOTAL_STEPS;
    if (isStepActive[st]) return st;
  }
  return 0;
}
// *******************************************************************************
// **                                                                           **
// **               L E D   M A T R I X   P R O C E D U R E S                   **
// **                                                                           **
// *******************************************************************************
void displayStepData (byte track, byte stepN, boolean val){
  ledMatrix.setLed(0, LED_COL_REMAP[stepN], LED_ROW_REMAP[track], val);
}

void refreshStepData(){
  for (byte t=0; t < TOTAL_TRACKS; t++){
    for (byte s=0; s < TOTAL_STEP_COLS; s++){
      displayStepData (t, s, getStepData (t, s*2 + currentSequencePage));
    }
  }
}

void refreshStepReferenceRow (){
  byte curStepLed = 255; // Initialized to a number outside of the valid range.

  if (isStepActive[currentStep] && ((currentStep % 2) == currentSequencePage)) curStepLed = currentStep/2;

  for (byte i = 0; i < TOTAL_STEP_COLS; i++) {
    ledMatrix.setLed(0, LED_COL_REMAP[i], LED_ROW_REMAP[REF_LED_ROW], (i == curStepLed));
  }
}

void showActiveSteps (){
  // We will  show the enabled/active steps using the step reference row
  for (byte i = 0; i < TOTAL_STEP_COLS; i++) {
    ledMatrix.setLed(0, LED_COL_REMAP[i], LED_ROW_REMAP[REF_LED_ROW], isStepActive[i*2 + currentSequencePage]);
  }
}

void displayTrackSelection (byte track, boolean val){
  // The total_tracks+1 rows is an 8 pixel "row" laid out as a column, with 2 LEDs
  // per track, the first one being the mute track indicator, and the second one
  // if the track is currently active.
  // This mapping is inverted in 22 Mini
#ifdef _22_MINI
  ledMatrix.setLed(0, LED_COL_REMAP[track*2], LED_ROW_REMAP[REF_LED_ROW+1], val);
#else
  ledMatrix.setLed(0, LED_COL_REMAP[track*2+1], LED_ROW_REMAP[REF_LED_ROW+1], val);
#endif
}

void displayTrackEnabled (byte track, boolean enabled){
#ifdef _22_MINI
  ledMatrix.setLed(0, LED_COL_REMAP[track*2+1], LED_ROW_REMAP[REF_LED_ROW+1], !enabled);
#else
  ledMatrix.setLed(0, LED_COL_REMAP[track*2], LED_ROW_REMAP[REF_LED_ROW+1], !enabled);
#endif
}

void toggleTrackEnabled (byte t){
  isTrackActive[t] = !isTrackActive[t];
  displayTrackEnabled (t, isTrackActive[t]);
}

void enableAllTracks(bool enabled){
	for (byte t =  0; t < TOTAL_TRACKS;  t++) {
	  isTrackActive[t] = enabled;
    displayTrackEnabled (t, isTrackActive[t]);
	}
}
// *******************************************************************************
// **                                                                           **
// **                       S E R V O   C O N T R O L                           **
// **                                                                           **
// *******************************************************************************
#ifdef SERVO_OUTPUT
class Sweeper {
  int pos;                  // current servo position 
  bool moving;
  int  updateInterval;      // interval between updates
  unsigned long lastUpdate; // last update of position
  int incstep;              // increment to move for each interval
  unsigned long servoMovementStart;
  int servoStartDelay;
 
public: 
  Servo servo;              // the servo

  Sweeper(){
    servoMovementStart = -1;
    pos = -1;
    setParams (0, 0);
  }

  void setTargetPos (int p){
    if (pos == p) return;
    pos = p;
    servo.write (p);
  }
  void setParams (int interval, int is){
    updateInterval = interval;
    incstep = is;
    moving = false;
  }

  void attach(int pin){
    servo.attach(pin);
    setTargetPos (SERVO_START_ANGLE);
  }

  void beat(){
    servoMovementStart = millis();
    // Fixed delay, based on "SERVO_DELAY"
    //servoStartDelay = max(extClockInterval - SERVO_DELAY, 0);

    // Compute auto servo delay, based on the current update values, the starting angle, and the ending position
    int servoTime = (updateInterval*(HIT_SERVO_ANGLE - pos)) / incstep;
    servoStartDelay = constrain(extClockInterval - servoTime, 0, extClockInterval - updateInterval);
  }
  
  void update(){
    unsigned long curTime = millis();
    int targetPos;

    // Are we waiting the delay before we move?
    if (servoMovementStart != -1) {
      if (curTime - servoMovementStart >= servoStartDelay) {
        servoMovementStart = -1;
        lastUpdate = curTime;
        moving = true;
      }
    }

    if (moving){
      if((curTime - lastUpdate) >= updateInterval){
        lastUpdate = curTime;
        targetPos = constrain(pos + incstep, SERVO_START_ANGLE, HIT_SERVO_ANGLE+1);
        if (pos < HIT_SERVO_ANGLE) {
          setTargetPos (targetPos);
        }else{
          moving = false;
        }
      }
    }else {
      setTargetPos max(targetPos-1, SERVO_START_ANGLE);
    }
  }
};
#else

class Sweeper {
  unsigned long beatStart;
  int outpin;

public: 
  Sweeper(){
    beatStart = millis() + CLK_SIGNAL_TIME + 1;
  }

  void off(){
#ifdef INVERTED_OUTPUT
    digitalWrite(outpin, LOW);
#else
    digitalWrite(outpin, HIGH);
#endif  
  }

  void on() {
#ifdef INVERTED_OUTPUT
    digitalWrite(outpin, HIGH);
#else
    digitalWrite(outpin, LOW);
#endif
  }

  void attach(int pin){
    outpin = pin;
    pinMode (outpin, OUTPUT);
    off();
  }

  void beat(){
    beatStart = millis();
    on();
  }
  
  void update(){
    if (millis() - beatStart >= CLK_SIGNAL_TIME) off();
  }
};
#endif

Sweeper sweeper[TOTAL_TRACKS];
// *******************************************************************************
// **                                                                           **
// **   A R D U I N O   P R O C S   &   I N T E R R U P T   H A N D L I N G     **
// **                                                                           **
// *******************************************************************************
// TIMER0 INTERRUPT --------------------------------------------
volatile int clkCounter = -1;
volatile bool lastClockVal = HIGH;
volatile bool curClockVal;
volatile unsigned long rightNow;

// This interrupt is called once a millisecond.
SIGNAL(TIMER0_COMPA_vect) {
  rightNow = millis();

  for (byte s=0; s<TOTAL_TRACKS;s++){
    sweeper[s].update();
  }

#ifdef MINIMAL_HARDWARE
  // Use the output as input in minimal hardware config
  curClockVal = digitalRead (sync_clock_out);
#else
  // Otherwise read the actual input
  curClockVal = digitalRead (sync_clock_in);
#endif

  if (curClockVal == true && lastClockVal == false){
    extClockInterval = rightNow - lastClockIn;
    lastClockIn = rightNow;
    if (playing) currentStep = nextEnabledStep(currentStep);
  }
  lastClockVal = curClockVal;

  msCounter++;
  if ((msCounter >= stepInterval) && (clkCounter == -1)){
    clkCounter = 0;
    msCounter = 0;
    digitalWrite (sync_clock_out, HIGH);
  }else {
    clkCounter++;
    if (clkCounter >= CLK_SIGNAL_TIME){
      digitalWrite (sync_clock_out, LOW);
      clkCounter = -1;
    }
  }
}

// SETUP -------------------------------------------------------
void setup() {
  //start serial
  Serial.begin(9600);

  //define pin modes
  pinMode(bttn_latchPin, OUTPUT);
  pinMode(bttn_clockPin, OUTPUT); 
  pinMode(bttn_dataPin, INPUT);
  pinMode(speed_analog_in, INPUT);
  pinMode(sync_clock_in, INPUT);
  digitalWrite(sync_clock_in, HIGH); // Enable pullup resistors
  pinMode(mtx_intensity_in, INPUT);
  pinMode(sync_clock_out, OUTPUT);

#ifdef _22_MINI
  pinMode(PLAY_BUTTON_PIN, OUTPUT);
#endif

  ledMatrix.shutdown (0,false);
  ledMatrix.clearDisplay(0);

  // Reset data
  resetSteps ();
  enableAllSteps(true);
  enableAllTracks(true);

  // Show starting track
  displayTrackSelection (currentTrack, true);

  // Initialize sweepers
  for (byte t=0; t<TOTAL_TRACKS; t++){
    sweeper[t].attach(outputPin[t]);
#ifdef SERVO_OUTPUT
    sweeper[t].setParams(SERVO_UPDATE_INT, SERVO_INCREMENT);
#endif
  }

  // All tracks and steps start active
  for (byte s=0; s < TOTAL_STEPS;  s++) isStepActive[s] = true;
  for (byte t=0; t < TOTAL_TRACKS; t++) isTrackActive[t] = true;

  // Servo update interrupt
  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

// LOOP --------------------------------------------------------
unsigned long playPressedTime = 0;
boolean       canReset = true;
int           lastProcessedStep = -1;
unsigned long currentTime;
bool          muteButtonPressed = false;

void loop() {
  byte realStep;

  currentTime = millis();

  readButtons();
#ifdef MINIMAL_HARDWARE
  stepInterval = MIN_STEP_TIME + (MAX_STEP_TIME - MIN_STEP_TIME)/2;
#else
  int speedCtrlVal = analogRead(speed_analog_in);
 #ifdef _22_MINI
  // Invert the speed control value
  speedCtrlVal = 1023 - speedCtrlVal;
  // Hard-coded matrix intensity
  byte mtxIntensity = LED_MATRIX_BRIGHTNESS;
 #else
  // Read the intensity pot
  byte mtxIntensity = map (analogRead(mtx_intensity_in), 0, 1023, 0, 15);
 #endif
  // Read the speed selection pot
  stepInterval = map(speedCtrlVal, 0, 1023, MIN_STEP_TIME, MAX_STEP_TIME);
  ledMatrix.setIntensity (0, mtxIntensity);
#endif
  // Some operations depend on the state of the mute button.
  muteButtonPressed = getButtonPressed (MUTE_BUTTON);

  // Track selection / Track mute
  for (byte t=0;t<TOTAL_TRACKS;t++){
    if (getButtonHit(track_buttons[t])) {
      if (muteButtonPressed) {
        toggleTrackEnabled (t);
      }else {
        // Update the selected track.
        // Turn OFF the led of the previous track
        displayTrackSelection (currentTrack, false);
        // Update the track value
        currentTrack = t;
      }
    }
  }

  // Turn ON the led of the new track
  displayTrackSelection (currentTrack, true);

  // Read step buttons and modify the current sequence if needed
  for (byte s=0;s<TOTAL_STEP_COLS;s++){
    if (getButtonHit(step_buttons[s])) {
      realStep = s*2 + currentSequencePage;
      if (muteButtonPressed){
        toggleStepEnabled (realStep);
      }else {
        toggleStepData (currentTrack, realStep);
        displayStepData (currentTrack, s, getStepData(currentTrack, realStep));
      }
    }
  }

  // Control buttons ########################
  // Play / Pause / Reset ----
  if (getPlayButtonReleased()) {
      // canReset is set to true when the button is not pressed
    if (canReset) playing = !playing;
  }else if (getPlayButtonHit()){
    playPressedTime = currentTime;
  }else if (getPlayButtonPressed()) {
    if ((currentTime - playPressedTime >= RESET_PRESS_TIME) && canReset) {
        // Full reset
        resetSteps ();
		enableAllSteps(true);
		enableAllTracks(true);
        refreshStepData();
        playing = true;
        canReset = false;
    }
  }else {
    canReset = true;
    playPressedTime = 0;
  }

  // Page selection button -----
  if (getButtonHit(PAGE_SEL_BUTTON)){
    currentSequencePage = (currentSequencePage + 1) % 2;
    ledMatrix.setLed(0, LED_COL_REMAP[OFFBEAT_LED_COL], LED_ROW_REMAP[OFFBEAT_LED_ROW], currentSequencePage);
    refreshStepData();
  }

  // Refresh the step reference bar if needed with the relevant information
  if (muteButtonPressed){
    showActiveSteps ();
  }else {
    refreshStepReferenceRow();
  }
 
  // Sequence Control ########################
  // Let's re-use the realStep variable. This time to tell us the step that the servos will execute
  // which will always be the step ahead.
  realStep = nextEnabledStep(currentStep);

  if (lastProcessedStep != realStep) {
    lastProcessedStep = realStep;
    // Execute step
    for (byte t=0; t<TOTAL_TRACKS;t++){
      if (isTrackActive[t] && isStepActive[realStep] && getStepData(t, realStep)) sweeper[t].beat();
    }
  }
}

#include <Servo.h>


#define SERVO_RANGE     30
#define SERVO_MINSTART  5
#define SERVO_MAXSTART  (120 - SERVO_RANGE)

#define SERVO_SPEED_DEGREES_PER_TICK  2
#define SERVO_TICK_MS                 6

#define TOTAL_SERVOS   4 
const int INPUT_PINS[]  = {A0, A1, A2, A3};
const int OUTPUT_PINS[] = {11,  10,  6, 5};
const int ADJUST_PINS[] = {A4, A5, A6, A7};


byte lastInput[TOTAL_SERVOS];
// *******************************************************************************
// **                                                                           **
// **                       S E R V O   C O N T R O L                           **
// **                                                                           **
// *******************************************************************************
class SimpleSweeper {
  int pos;                  // current servo position 
  int targetPos;
  int servoStartPos;
  int startPos;

public: 
  Servo servo;              // the servo

  SimpleSweeper(){
    pos = -1;
    targetPos = -1;
    startPos = SERVO_MINSTART;
  }

  void setTargetPosition (int p){
    if (targetPos == p) return;
    targetPos = p;
  }

  void attach(int pin){
    servo.attach(pin);
  }

  void setStartPosition (int p){
    if (targetPos == startPos || targetPos == -1) targetPos = p;
    startPos = p;
  }

  void beat(){
    setTargetPosition (startPos + SERVO_RANGE);
  }
  
  void tick(){
    int newPos;
 
    if (pos == targetPos) {
      if (targetPos != startPos) {
        targetPos = startPos;
      } else {
        return;
      }
    }

    if (targetPos - pos > 0){
      newPos = pos + SERVO_SPEED_DEGREES_PER_TICK;
      if (newPos > targetPos) newPos = targetPos;
    } else {
      newPos = pos - SERVO_SPEED_DEGREES_PER_TICK;
      if (newPos < targetPos) newPos = targetPos;
    }
    servo.write (newPos);
    pos = newPos;
  }
};

SimpleSweeper servo[TOTAL_SERVOS];
// *******************************************************************************
// **                                                                           **
// **   A R D U I N O   P R O C S   &   I N T E R R U P T   H A N D L I N G     **
// **                                                                           **
// *******************************************************************************
volatile long lastUpdate = millis();

// This interrupt is called once a millisecond.
SIGNAL(TIMER0_COMPA_vect) {
  long curTime = millis();

  if (curTime - lastUpdate < SERVO_TICK_MS) return;
  lastUpdate = curTime;

  for (byte s=0; s<TOTAL_SERVOS;s++){
    servo[s].tick();
  }

}

void setup() {
  for (byte s = 0; s < TOTAL_SERVOS; s++) {
    pinMode (INPUT_PINS[s], INPUT);
    pinMode (ADJUST_PINS[s], INPUT);
    servo[s].attach(OUTPUT_PINS[s]);
  }

  // Servo update interrupt
  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

void loop() {
  for (byte s=0; s<TOTAL_SERVOS;s++){
    int v = analogRead (ADJUST_PINS[s]);
    servo[s].setStartPosition (map(v, 0, 1023, SERVO_MINSTART, SERVO_MAXSTART));
    byte in = digitalRead(INPUT_PINS[s]);
    if (lastInput[s] == 0 && in != 0) servo[s].beat();
    lastInput[s] = in;
  }
}

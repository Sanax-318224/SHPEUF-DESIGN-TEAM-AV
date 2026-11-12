// =====================================================================
//  SHPE AV v2 — full sketch skeleton (all functions + variables)
//  One setup(), one loop(). Each subsystem owns its section below.
//  Pins marked TBD are suggestions — confirm against the pin guide.
// =====================================================================

#include <Wire.h>
#include <Servo.h>
#include "Adafruit_VL53L0X.h"

// ---------------------------------------------------------------------
//  PINS
// ---------------------------------------------------------------------
// DC motors via L298N (ENA/ENB MUST be PWM pins, and NOT 9 or 10)
const int ENA_PIN = 5;     // TBD  PWM, left motor speed
const int ENB_PIN = 6;     // TBD  PWM, right motor speed
const int IN1_PIN = 7;     // TBD  only if IN1-IN4 go to the Arduino
const int IN2_PIN = 8;     // TBD
const int IN3_PIN = 12;    // TBD
const int IN4_PIN = 13;    // TBD

// Servo (any digital pin; Servo library disables PWM on 9 and 10)
const int SERVO_PIN = 9;   // TBD

// Line tracker (TCRT5000 digital OUT)
const int LEFT_PIN  = 2;   // TBD
const int RIGHT_PIN = 4;   // TBD

// VL53L0X uses I2C: fixed at A4 (SDA) and A5 (SCL) on an Uno — no constant needed

// ---------------------------------------------------------------------
//  DC MOTORS — Nicholas / Mauricio
// ---------------------------------------------------------------------
// (no state variables needed)

void MotorSetup();               // pinMode ENA/ENB (+ IN1-IN4) as OUTPUT, set forward
void DriveMotors(int speed);     // analogWrite(ENA, speed); analogWrite(ENB, speed)  [0-255]
void StopMotors();               // analogWrite(ENA, 0);     analogWrite(ENB, 0)

// ---------------------------------------------------------------------
//  SERVO STEERING — Nicholas / Mauricio
// ---------------------------------------------------------------------
const int STEER_CENTER = 90;     // degrees, measure on the car
const int STEER_MIN    = 60;     // degrees, measure on the car
const int STEER_MAX    = 120;    // degrees, measure on the car

Servo steeringServo;

void SteeringSetup();                 // attach(SERVO_PIN), write(STEER_CENTER)
void SetSteering(float correction);   // + = right, - = left (flip sign if car turns wrong way)

// ---------------------------------------------------------------------
//  DISTANCE SENSOR (VL53L0X) — Tony / Daniel
// ---------------------------------------------------------------------
const uint16_t STOP_DISTANCE_MM  = 200;   // stop when closer than this
const uint16_t CLEAR_DISTANCE_MM = 250;   // resume only once farther than this (prevents stop/go chatter)

const unsigned long SENSOR_TIMEOUT_MS = 500;   // no new reading this long -> treat sensor as failed
const uint16_t OUT_OF_RANGE_MM = 8000;        // continuous mode reports ~8190 when nothing is in range

Adafruit_VL53L0X lox;
uint16_t distanceMM       = 0;      // latest reading
bool     obstacleDetected = false;  // current decision, held between readings
bool     distanceSensorOK = false;  // false if lox.begin() failed
unsigned long lastReadingMs = 0;    // millis() of the last completed reading

void DistanceSetup();      // Wire.begin(), lox.begin(), lox.startRangeContinuous()
bool ObstacleAhead();      // if lox.isRangeComplete(): read + update obstacleDetected;
                           // always return obstacleDetected. Never calls the motors.

// ---------------------------------------------------------------------
//  LINE TRACKER — Jacob
// ---------------------------------------------------------------------
const int   DRIVE_SPEED = 150;   // PWM 0-255 sent to DriveMotors
const float TURN_SPEED  = 30.0;  // max steering correction (degrees)
const float RAMP_RATE   = 60.0;  // correction increase (degrees/sec)

int   leftIR            = 0;     // 1 = black, 0 = white
int   rightIR           = 0;
float correction        = 0;     // right +, left - (degrees)
float turnStartTime     = 0;     // seconds
int   turnDirection     = 0;     // right = 1, left = -1, straight = 0
int   previousDirection = 0;
float lastUpdateTime    = 0;     // seconds, last time LineTrackerUpdate() ran
const float RESUME_GAP  = 0.2;   // if paused longer than this (e.g. obstacle stop), restart the ramp

void LineTrackerSetup();          // pinMode LEFT_PIN / RIGHT_PIN as INPUT
int  Read_IR(int sensorPin);      // return digitalRead(sensorPin)
void LineTrackerUpdate();         // your "Repeat continuously" body;
                                  // "Continue to next loop here" -> return;

// ---------------------------------------------------------------------
//  OPTIONAL — debugging
// ---------------------------------------------------------------------
const bool DEBUG = true;
void PrintTelemetry();   // Serial.print distanceMM, leftIR, rightIR, correction (throttle to ~10 Hz)

// =====================================================================
//  MAIN
// =====================================================================
void setup() {
  if (DEBUG) Serial.begin(115200);
  MotorSetup();
  SteeringSetup();
  DistanceSetup();
  LineTrackerSetup();
}

void loop() {
  if (ObstacleAhead()) {       // highest priority: safety stop
    StopMotors();
    return;                    // skip line tracking this pass
  }
  LineTrackerUpdate();         // drives + steers the car
  if (DEBUG) PrintTelemetry();
}

// =====================================================================
//  FUNCTION BODIES — each owner fills in their own (no delay() anywhere)
// =====================================================================
void MotorSetup()                  { /* TODO Nicholas / Mauricio */ }
void DriveMotors(int speed)        { /* TODO Nicholas / Mauricio */ }
void StopMotors()                  { /* TODO Nicholas / Mauricio */ }

void SteeringSetup()               { /* TODO Nicholas / Mauricio */ }
void SetSteering(float correction) { /* TODO Nicholas / Mauricio */ }

// ---------------- DISTANCE SENSOR — Tony / Daniel ----------------
void DistanceSetup() {
  Wire.begin();
  distanceSensorOK = lox.begin();
  if (!distanceSensorOK) {
    if (DEBUG) Serial.println(F("VL53L0X not found - check wiring (SDA=A4, SCL=A5, 5V, GND)"));
    return;
  }
  lox.startRangeContinuous();          // sensor measures on its own; we just collect results
  lastReadingMs = millis();
}

bool ObstacleAhead() {
  // Fail safe: no sensor means no driving.
  if (!distanceSensorOK) {
    obstacleDetected = true;
    return obstacleDetected;
  }

  // Non-blocking: only read when a new measurement is ready.
  if (lox.isRangeComplete()) {
    distanceMM = lox.readRange();
    lastReadingMs = millis();

    bool inRange = (distanceMM > 0 && distanceMM < OUT_OF_RANGE_MM);

    if (!obstacleDetected && inRange && distanceMM < STOP_DISTANCE_MM) {
      obstacleDetected = true;         // something got too close: stop
    }
    else if (obstacleDetected && (!inRange || distanceMM > CLEAR_DISTANCE_MM)) {
      obstacleDetected = false;        // clearly gone: resume
    }
  }

  // Fail safe: sensor stopped reporting.
  if (millis() - lastReadingMs > SENSOR_TIMEOUT_MS) {
    obstacleDetected = true;
  }

  return obstacleDetected;
}

// ---------------- LINE TRACKER — Jacob ----------------
void LineTrackerSetup() {
  pinMode(LEFT_PIN, INPUT);
  pinMode(RIGHT_PIN, INPUT);
}

int Read_IR(int sensorPin) {
  return digitalRead(sensorPin);           // 1 = black, 0 = white
}

void LineTrackerUpdate() {
  leftIR  = Read_IR(LEFT_PIN);
  rightIR = Read_IR(RIGHT_PIN);
  float now = millis() / 1000.0;

  // If loop() skipped us for a while (obstacle stop), restart the steering ramp
  // instead of jumping straight to full correction on the first pass back.
  if (now - lastUpdateTime > RESUME_GAP) {
    previousDirection = 0;
  }
  lastUpdateTime = now;

  // Decide direction from the two sensors.
  // If the car steers AWAY from the line on the bench, swap the 1 and -1 below.
  if (leftIR == 0 && rightIR == 0) {
    turnDirection = 0;                     // both white: go straight
  }
  else if (leftIR == 1 && rightIR == 0) {
    turnDirection = 1;                     // left sees black: steer right
  }
  else if (leftIR == 0 && rightIR == 1) {
    turnDirection = -1;                    // right sees black: steer left
  }
  else {                                   // both black: stop
    StopMotors();
    SetSteering(0);
    correction = 0;
    turnDirection = 0;
    previousDirection = 0;
    return;                                // "continue to next loop"
  }

  // Steering correction: ramps up the longer we hold a turn, capped at TURN_SPEED.
  if (turnDirection == 0) {
    correction = 0;
  }
  else {
    if (turnDirection != previousDirection) {
      turnStartTime = now;                 // new turn (or flipped side): restart ramp
    }
    correction = turnDirection * min(RAMP_RATE * (now - turnStartTime), TURN_SPEED);
  }

  SetSteering(correction);
  DriveMotors(DRIVE_SPEED);
  previousDirection = turnDirection;
}

void PrintTelemetry()              { /* optional */ }

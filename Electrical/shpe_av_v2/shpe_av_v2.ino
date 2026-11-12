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

Adafruit_VL53L0X lox;
uint16_t distanceMM       = 0;      // latest reading
bool     obstacleDetected = false;  // current decision, held between readings

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

void DistanceSetup()               { /* TODO Tony / Daniel */ }
bool ObstacleAhead()               { /* TODO Tony / Daniel */ return obstacleDetected; }

void LineTrackerSetup()            { /* TODO Jacob */ }
int  Read_IR(int sensorPin)        { return digitalRead(sensorPin); }
void LineTrackerUpdate()           { /* TODO Jacob */ }

void PrintTelemetry()              { /* optional */ }

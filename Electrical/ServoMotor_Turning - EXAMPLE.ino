#include <Servo.h>

Servo servoMotor;

const int leftSensor = 2;
const int rightSensor = 3;

int position = 90;          
const int turnAmount = 2;     
const int minAngle = 0;
const int maxAngle = 180;


void setup() {
  pinMode(leftSensor, INPUT);
  pinMode(rightSensor, INPUT);
  servoMotor.attach(9);
  servoMotor.write(position);
}

void loop() {
  int leftSide = digitalRead(leftSensor);
  int rightSide = digitalRead(rightSensor);

  // low= line detected
  // high= no line detected
  bool leftDetected = (leftSide == LOW);
  bool rightDetected = (rightSide == LOW);

  if (leftDetected && !rightDetected) {
    // if the line is more under left it will move left
    position -= turnAmount;
  }
  else if (rightDetected && !leftDetected) {
    //if the line is more under right it will move right
    position += turnAmount;
  }
  else if (leftDetected && rightDetected) {
    // both sensors see the line so it does nothing
  }
  else {
    // no sensor sees line so it lost the lines and does nothing
  }
//prevents motor from moving too far and not being able to return to a normal range of turning
  if (position < minAngle){
    position = minAngle;
  }
  if (position > maxAngle){
    position = maxAngle;
  }

  servoMotor.write(position);

  delay(30);
}
#include <Servo.h>

int pos = 0;
float prev = 0;

// Sensors are looked at from the front
// left sensor
const int trigPinLeft = 2;
const int echoPinLeft = 3;
// center sensor
const int trigPinCenter = 7;
const int echoPinCenter = 8;
// right sensor
const int trigPinRight = 4;
const int echoPinRight = 5;

float left_value = 0;
float center_value = 0;
float right_value = 0;
int direction_trim = 8;

// defining the servo and the motor
Servo speed_control;
Servo direction_control;

void setup() {
  // Put the motor to Arduino pin #9
  speed_control.attach(9);
  // Put the servo to Arduino pin #10
  direction_control.attach(10);
  // starts the serial monitor output
  Serial.begin(9600);
}

//distance detection function
float getDistance(int tPin, int ePin, float prev) {
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(tPin, OUTPUT);
  digitalWrite(tPin, LOW);
  delayMicroseconds(2);
  digitalWrite(tPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(tPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(ePin, INPUT);

  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  float distance = pulseIn(ePin, HIGH) / 29 / 2;

//  return 0.6 * prev + 0.4 * distance;
  return distance;
}

void loop() {
  // read the ultrasound sensors
  left_value = getDistance(trigPinLeft, echoPinLeft, left_value);
  center_value = getDistance(trigPinCenter, echoPinCenter, center_value);
  right_value = getDistance(trigPinRight, echoPinRight, right_value);
  //prints the filtered value

  int new_speed;
  int new_direction;
  int diff;
  if (center_value < 30) {
    new_speed = 30;
    new_direction = 90;
    diff = 0;
  } else {
    new_speed = 100;
    if (left_value > 150) left_value = 150;
    if (right_value > 150) right_value = 150;
    // '?' is a small if statement, exactly the same as if, then do this, else do that
    int min_value = (left_value < right_value) ? left_value : right_value;
    diff = left_value - right_value;
    if (min_value < 30) {
      new_direction = 90 - diff * 120.0 / 100;
      if (new_direction < 30) new_direction = 30;
      else if (new_direction > 115) new_direction = 115;
      
    } else if (min_value < 80) {
      new_direction = 90 - diff * 80.0 / 100;
      if (new_direction < 50) new_direction = 30;
    else if (new_direction > 110) new_direction = 115;
      
    } else {
      new_direction = 90 - diff * 40 / 100;
      if (new_direction < 75) new_direction = 30;
      else if (new_direction > 100) new_direction = 115;
      
    }
    //if (new_direction < 30) new_direction = 30;
    //else if (new_direction > 115) new_direction = 115;
  }
  Serial.println("Sensor left: "+ String(left_value) + " Sensor center: "+ String(center_value) + " Sensor right: "+ String(right_value) + " => speed = " + String(new_speed) + ", direction = " + String(new_direction) + ", diff = " + String(diff));
  speed_control.write(new_speed);
  direction_control.write(new_direction);

  delay(10);
}

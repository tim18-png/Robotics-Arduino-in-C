#include <Servo.h>
#include <EEPROM.h>

Servo leftServo; // Servo objects
Servo rightServo;

#define LEFT_BUTTON 4 // Button pins
#define RIGHT_BUTTON 2

#define LED_GREEN 7  // LED pins
#define LED_YELLOW 12
#define LED_RED 13

#define RIGHT_LDR_PIN A0  // LDR pins
#define MIDDLE_LDR_PIN A1
#define LEFT_LDR_PIN A2

#define EEPROM_LEFT_SERVO_STOP 0
#define EEPROM_RIGHT_SERVO_STOP 2
#define EEPROM_LEFT_THRESHOLD 6
#define EEPROM_MIDDLE_THRESHOLD 10
#define EEPROM_RIGHT_THRESHOLD 14

const int LEFT_SERVO_PIN = 6; // Servo pins
const int RIGHT_SERVO_PIN = 5;

int LEFT_THRESHOLD = 301; // Line threshold values 
int MIDDLE_THRESHOLD = 364;
int RIGHT_THRESHOLD = 348;

int LEFT_STOP = 89; // Servo speed values
int RIGHT_STOP = 90; 

const int IR_TX_PIN = 3; // IR transmit 
const int IR_RX_PIN = 2; // IR recieve
const int IR_FREQ = 38000; // IR frequency in Hz

int printCounter = 0;

bool recording = false; // Record and playback variables for sign plate detection
bool playback = false;

#define NUM_MOVEMENTS 100 // part of array to store the playback movements
int recordedMovements[NUM_MOVEMENTS];
int movementCount = 0;

#define DEBUG_LEFT_BUTTON // Preprocessor used to debug robot hardware
#define DEBUG_SERVO
#define DEBUG_LDR
#define DEBUG_IR_SENSOR
#define DEBUG_LED
#define DEBUG_LINE_FOLLOWING

void setup() {
  Serial.begin(9600);

  leftServo.attach(LEFT_SERVO_PIN); // Servo pinmordes
  rightServo.attach(RIGHT_SERVO_PIN);

  pinMode(RIGHT_LDR_PIN, INPUT); // LDR pinmodes
  pinMode(MIDDLE_LDR_PIN, INPUT);
  pinMode(LEFT_LDR_PIN, INPUT);

  pinMode(LEFT_BUTTON, INPUT); // BUTTON pinmodes
  pinMode(RIGHT_BUTTON, INPUT);

  pinMode (IR_RX_PIN, INPUT); // IR pinmodes
  pinMode(IR_TX_PIN, OUTPUT); // Setup IR LED pin as output
  tone(IR_TX_PIN, IR_FREQ); 

  pinMode (LED_GREEN, OUTPUT); // LEdD pinmodes
  pinMode (LED_YELLOW, OUTPUT);
  pinMode (LED_RED, OUTPUT); 
  
  LEFT_STOP = EEPROM.read(EEPROM_LEFT_SERVO_STOP);
  RIGHT_STOP = EEPROM.read(EEPROM_RIGHT_SERVO_STOP);
  
  LEFT_THRESHOLD = (EEPROM.read(EEPROM_LEFT_THRESHOLD) << 8) | EEPROM.read(EEPROM_LEFT_THRESHOLD + 1);
  MIDDLE_THRESHOLD = (EEPROM.read(EEPROM_MIDDLE_THRESHOLD) << 8) | EEPROM.read(EEPROM_MIDDLE_THRESHOLD + 1);
  RIGHT_THRESHOLD = (EEPROM.read(EEPROM_RIGHT_THRESHOLD) << 8) | EEPROM.read(EEPROM_RIGHT_THRESHOLD + 1);
  
  EEPROM.write(EEPROM_LEFT_SERVO_STOP, LEFT_STOP);
  EEPROM.write(EEPROM_RIGHT_SERVO_STOP, RIGHT_STOP);
  
  EEPROM.write(EEPROM_LEFT_THRESHOLD, (LEFT_THRESHOLD >> 8) & 0xFF);
  EEPROM.write(EEPROM_LEFT_THRESHOLD + 1, LEFT_THRESHOLD & 0xFF);
  EEPROM.write(EEPROM_MIDDLE_THRESHOLD, (MIDDLE_THRESHOLD >> 8) & 0xFF);
  EEPROM.write(EEPROM_MIDDLE_THRESHOLD + 1, MIDDLE_THRESHOLD & 0xFF);
  EEPROM.write(EEPROM_RIGHT_THRESHOLD, (RIGHT_THRESHOLD >> 8) & 0xFF);
  EEPROM.write(EEPROM_RIGHT_THRESHOLD + 1, RIGHT_THRESHOLD & 0xFF);
}

void loop() {
  if (detectObstacle()) {
    stopMoving(); // Robot stops moving if obstacle detected 
    flashYellowLED(); // Yellow LED will flash to insticate obsticle detected
    Serial.println("Obstacle Detected"); // Detection message will be shown in serial monitor
   } else {
  digitalWrite(LED_YELLOW, LOW); // Yellow LED is turned off if no obstacle detected
  followLine(); // Robot will start to follow the line again
  }
}

void followLine() { 
  int leftLDRValue  = analogRead(LEFT_LDR_PIN); // Read left LDR
  int middleLDRValue  = analogRead(MIDDLE_LDR_PIN); // Read middle LDR
  int rightLDRValue  = analogRead(RIGHT_LDR_PIN); // Read riight LDR

  if (leftLDRValue < LEFT_THRESHOLD && middleLDRValue < MIDDLE_THRESHOLD && rightLDRValue < RIGHT_THRESHOLD) { // Checks if all LDRS detect a dark surface which could be a bar
    unsigned long startTime = millis();
    while (millis() - startTime < 2000) { // Check for another bar to initiate record or playback 
      if (analogRead(LEFT_LDR_PIN) < LEFT_THRESHOLD && 
          analogRead(MIDDLE_LDR_PIN) < MIDDLE_THRESHOLD && 
          analogRead(RIGHT_LDR_PIN) < RIGHT_THRESHOLD) {
       if (!recording) {
          recording = true;
          digitalWrite(LED_RED, HIGH);
          Serial.println("Start Recording"); // If dark surface still present recording beings and LED singals recording
       } else {
          recording = false;
          digitalWrite(LED_RED, LOW);
          Serial.println("Stop Recording"); // If dark surface is not present recording is stopped and LED is off and playback begins
       }
       delay(1000); // Delay should prevent multiple detections of sign plate 
       return;
      }
    }
  }
 
 if (printCounter++ % 10 == 0) { // Prints for every 10 iteratioins to keep serial monitor more readable
  Serial.print("Left LDR: "); // Print values for LEFT LDR in serial monitor
  Serial.print(leftLDRValue);
  Serial.print(" Middle LDR: "); // Print values for MIDDLE LDR in serial monitor
  Serial.print(middleLDRValue);
  Serial.print(" Right LDR: "); // Print values for RIGHT LDR in serial monitor
  Serial.println(rightLDRValue);
 }

  if (middleLDRValue < MIDDLE_THRESHOLD) { // Line markings found under middle LDR
    moveForwards();
    digitalWrite(LED_RED, LOW);
    Serial.println("Moving Forward"); // Robots action forward movement printed in serial montitor 
  } else if (leftLDRValue < LEFT_THRESHOLD) { // Line markings found under middle LDR
    turnLeft();
    digitalWrite(LED_RED, LOW);
    Serial.println("Turning Left"); // Robots action Left movement printed in serial montitor
  } else if (rightLDRValue < RIGHT_THRESHOLD) { // Line markings found under middle LDR
    turnRight();
    digitalWrite(LED_RED, LOW);
    Serial.println("Turning Right"); // Robots action forward movement printed in serial montitor
  } else { 
    stopMoving(); // No line markings found under any LDR
    digitalWrite(LED_RED, HIGH);
    Serial.println("No Line Detected"); // Prints in serial monitor when no line is detected by LDR
  }
}

 void moveForwards() { // Servo values to make robot move forwards
   leftServo.write(LEFT_STOP + 10); 
   rightServo.write(RIGHT_STOP - 10); 

 } 
 void turnLeft() { // Servo values to make robot turn left
  leftServo.write(LEFT_STOP - 10); 
  rightServo.write(RIGHT_STOP - 10); 

 } 
 void turnRight() { // Servo values to make robot turn righ
  leftServo.write(LEFT_STOP + 10); 
  rightServo.write(RIGHT_STOP + 10); 

 } 
 void stopMoving() { // Servo values needed to make robot to stop moving
  leftServo.write(LEFT_STOP); 
  rightServo.write(RIGHT_STOP); 
 }

 bool detectObstacle() {
  tone(IR_TX_PIN, IR_FREQ); // Start sending IR pulses at 38kHz on pin 3
  delay(50);

  bool obstacleDetected = (digitalRead(IR_RX_PIN) == LOW);

  if (obstacleDetected) {
    Serial.println("Obstacle detected");
    digitalWrite(LED_YELLOW, HIGH); // Yellow LED turned on if obstacle detected by IR
  } else {
    Serial.println("No obstacle detected");
    digitalWrite(LED_YELLOW, LOW); // Yellow LED turned off if no obstacle detected by IR
  }
  return obstacleDetected;
}

void flashYellowLED() {
  digitalWrite(LED_YELLOW, HIGH);
  delay(250);
  digitalWrite(LED_YELLOW, LOW);
  delay(250);
}


    


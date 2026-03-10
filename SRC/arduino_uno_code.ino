#include <AFMotor.h>
#include <Servo.h>
#include <Wire.h>

// ================= I2C =================
#define I2C_ADDRESS 0x08
volatile bool trashDetected = false;   // LATCHED FLAG
bool isBusy = false;

// ================= MOTORS =================
AF_DCMotor rightFront(1);
AF_DCMotor rightRear(2);
AF_DCMotor leftFront(3);
AF_DCMotor leftRear(4);

// ================= SERVOS =================
Servo baseServo, armServo1, armServo2, endEffector;

// ================= ULTRASONIC =================
#define TRIG_PIN A1
#define ECHO_PIN A0
#define DISTANCE_THRESHOLD 10.0   // cm

// ================= ARM POSITIONS =================
// Home
int b1 = 150, s9_1 = 80,  s10_1 = 60, e13_1 = 90;
// Reach
int b2 = 35,  s9_2 = 100, s10_2 = 50, e13_2 = 90;
// Collect
int b3 = 10,  s9_3 = 60,  s10_3 = 20, e13_3 = 90;
// Grab
int b4 = 10,  s9_4 = 35,  s10_4 = 70, e13_4 = 35;

// ================= SPEED LIMITS =================
struct MotorLimits {
  uint8_t min;
  uint8_t max;
};

MotorLimits LF = {75, 100};
MotorLimits LR = {75, 100};
MotorLimits RF = {75, 100};
MotorLimits RR = {105, 120};

int stepDelay = 35;

// =================================================
// ===================== SETUP =====================
// =================================================
void setup() {
  Serial.begin(9600);

  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  baseServo.attach(2);
  armServo1.attach(9);
  armServo2.attach(10);
  endEffector.attach(13);

  // Initial position
  baseServo.write(b1);
  armServo1.write(s9_1);
  armServo2.write(s10_1);
  endEffector.write(e13_1);

  Serial.println("SYSTEM READY");
}

// =================================================
// ====================== LOOP =====================
// =================================================
void loop() {
  if (isBusy) return;

  long distance = getDistance();

  if (distance > 0 && distance < DISTANCE_THRESHOLD) {
    stopAll();
    delay(200);   // stabilization

    if (trashDetected) {
      Serial.println("STATE: TRASH DETECTED -> ARM ACTION");

      isBusy = true;
      runFullArmCycle();
      isBusy = false;

      trashDetected = false;   // RESET ONLY AFTER ARM FINISHES
    } else {
      Serial.println("STATE: OBSTACLE NO TRASH -> TURN LEFT");
      turnLeftHighSpeed(900);
    }
  }
  else {
    moveConstantForward();
  }

  delay(30);
}

// =================================================
// ================= I2C RECEIVE ===================
// =================================================
void receiveEvent(int howMany) {
  while (Wire.available()) {
    char c = Wire.read();
    Serial.print("I2C Received: ");
    Serial.println(c);

    if (c == '1') {
      trashDetected = true;   // LATCH
    }
  }
}

// =================================================
// ================= ARM SEQUENCE ==================
// =================================================
void runFullArmCycle() {
  executeSequence(b1, s9_1, s10_1, e13_1, b2, s9_2, s10_2, e13_2);
  executeSequence(b2, s9_2, s10_2, e13_2, b3, s9_3, s10_3, e13_3);
  executeSequence(b3, s9_3, s10_3, e13_3, b4, s9_4, s10_4, e13_4);

  executeSequenceNoGrip(b4, s9_4, s10_4, b3, s9_3, s10_3);
  executeSequenceNoGrip(b3, s9_3, s10_3, b2, s9_2, s10_2);
  executeSequenceNoGrip(b2, s9_2, s10_2, b1, s9_1, s10_1);

  moveSmooth(endEffector, e13_4, e13_1);
}

// =================================================
// ================= MOVEMENT ======================
// =================================================
void moveConstantForward() {
  leftFront.run(FORWARD);
  leftRear.run(FORWARD);
  rightFront.run(FORWARD);
  rightRear.run(FORWARD);
  applyIndividualSpeeds(1.0);
}

void turnLeftHighSpeed(int duration) {
  leftFront.run(BACKWARD);
  leftRear.run(BACKWARD);
  rightFront.run(FORWARD);
  rightRear.run(FORWARD);

  leftFront.setSpeed(150);
  leftRear.setSpeed(150);
  rightFront.setSpeed(150);
  rightRear.setSpeed(200);

  delay(duration);
  stopAll();
}

void stopAll() {
  leftFront.run(RELEASE);
  leftRear.run(RELEASE);
  rightFront.run(RELEASE);
  rightRear.run(RELEASE);
}

void applyIndividualSpeeds(float percentage) {
  leftFront.setSpeed(LF.min + (LF.max - LF.min) * percentage);
  leftRear.setSpeed(LR.min + (LR.max - LR.min) * percentage);
  rightFront.setSpeed(RF.min + (RF.max - RF.min) * percentage);
  rightRear.setSpeed(RR.min + (RR.max - RR.min) * percentage);
}

// =================================================
// ================= SERVO HELPERS =================
// =================================================
void executeSequence(int oldB, int old9, int old10, int oldE,
                     int newB, int new9, int new10, int newE) {
  moveSmooth(armServo1, old9, new9);
  moveSmooth(armServo2, old10, new10);
  moveSmooth(baseServo, oldB, newB);
  moveSmooth(endEffector, oldE, newE);
}

void executeSequenceNoGrip(int oldB, int old9, int old10,
                           int newB, int new9, int new10) {
  moveSmooth(armServo1, old9, new9);
  moveSmooth(armServo2, old10, new10);
  moveSmooth(baseServo, oldB, newB);
}

void moveSmooth(Servo &s, int from, int to) {
  if (from == to) return;

  if (from < to) {
    for (int p = from; p <= to; p++) {
      s.write(p);
      delay(stepDelay);
    }
  } else {
    for (int p = from; p >= to; p--) {
      s.write(p);
      delay(stepDelay);
    }
  }
}

// =================================================
// ================= ULTRASONIC ====================
// =================================================
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  if (duration == 0) return -1;

  return duration * 0.034 / 2;
}

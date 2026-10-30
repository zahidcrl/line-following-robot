// Line Following Robot

int THRESHOLD = 1000;
int SPEED = 140;

int counter = 0;
int memory = 0;

// Motors
const int AIN1 = 8;  // Motor A
const int AIN2 = 7;  // Motor A
const int PWMA = 6;  // PWM Motor A
const int BIN1 = 9;  // Motor B
const int BIN2 = 10; // Motor B
const int PWMB = 11; // PWM Motor B
const int STBY = 12; // Standby

// Debounce
byte stableReading = 0;
byte previewReading = 0;
int filterCounter = 0;
const int FILTER_THRESHOLD = 3;


// Setup
void setup() {
  Serial.begin(9600);

  pinMode(AIN1, OUTPUT);    // AIN1
  pinMode(AIN2, OUTPUT);    // AIN2
  pinMode(PWMA, OUTPUT);    // PWMA
  pinMode(BIN1, OUTPUT);    // BIN1
  pinMode(BIN2, OUTPUT);    // BIN2
  pinMode(PWMB, OUTPUT);    // PWMB
  pinMode(STBY, OUTPUT);    // STBY
  
  digitalWrite(STBY, HIGH); // STBY

  moveMotor(0, 0);
}


// Function
void moveMotor(int speedLeft, int speedRight) { // From -255 to 255
  motorController(speedLeft, AIN1, AIN2, PWMA);
  motorController(speedRight, BIN1, BIN2, PWMB);
}

void motorController(int speed, int in1, int in2, int pwm) {

  if (speed == 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, HIGH);
    analogWrite(pwm, 0);
    return;
  }

  // Forward
  if (speed > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(pwm, constrain(speed, 0, 255));
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(pwm, constrain(abs(speed), 0, 255));
  }
}

void sensor() {

  byte currentReading = 0;
  
  if (analogRead(A0) > THRESHOLD) currentReading |= (1 << 7); // Left
  if (analogRead(A1) > THRESHOLD) currentReading |= (1 << 6);
  if (analogRead(A2) > THRESHOLD) currentReading |= (1 << 5);
  if (analogRead(A3) > THRESHOLD) currentReading |= (1 << 4); // Center Left
  if (analogRead(A4) > THRESHOLD) currentReading |= (1 << 3); // Center Right
  if (analogRead(A5) > THRESHOLD) currentReading |= (1 << 2);
  if (analogRead(A6) > THRESHOLD) currentReading |= (1 << 1);
  if (analogRead(A7) > THRESHOLD) currentReading |= (1 << 0); // Right

  // Debounce filter
  if (currentReading == previewReading) {
    filterCounter++;
    if (filterCounter >= FILTER_THRESHOLD) stableReading = currentReading;
  } else {
    filterCounter = 0;
    previewReading = currentReading;
  }

  stableReading = currentReading;

  switch (stableReading) {
    
    // Center
    case 0b00011000:
    case 0b00010000:
    case 0b00001000:
      moveMotor(255, 255); 
    break;

    // Right
    case 0b00001100:
      // memory = 1; moveMotor(255, 140);
      memory = 1; moveMotor(255, 0);
    break;

    case 0b00000110: 
    case 0b00000100: 
      // memory = 1; moveMotor(255, 40);
      memory = 1; moveMotor(255, 0);
    break;

    case 0b00000011:
    case 0b00000001:
      // memory = 1; moveMotor(200, -100);
      memory = 1; moveMotor(255, -255);
    break;

    // Left
    case 0b00110000:
      // memory = 2; moveMotor(140, 255);
      memory = 2; moveMotor(0, 255);
    break;

    case 0b01100000: 
    case 0b01000000: 
      // memory = 2; moveMotor(40, 255);
      memory = 2; moveMotor(0, 255);
    break;

    case 0b11000000:
    case 0b10000000:
      // memory = 2; moveMotor(-100, 200);
      memory = 2; moveMotor(-255, 255);
    break;


    // Stop
    case 0b11111111:
      // moveMotor(0, 0);
      moveMotor(255, 255);
    break;

    // Flip-Flop
    case 0b00000000:
      if (memory == 1) moveMotor(180, -120);
      else if (memory == 2) moveMotor(-120, 180); 
      else moveMotor(100, 100);
    break;

    // Default
    default:
      // moveMotor(120, 120);
      moveMotor(255, 255);
    break;
  }
}

void drawSensor() {
  counter++;
  if (counter >= 150) {
    // Serial.print(analogRead(A0)); Serial.print(" ");
    // Serial.print(analogRead(A1)); Serial.print(" ");
    // Serial.print(analogRead(A2)); Serial.print(" ");
    // Serial.print(analogRead(A3)); Serial.print(" ");
    // Serial.print(analogRead(A4)); Serial.print(" ");
    // Serial.print(analogRead(A5)); Serial.print(" ");
    // Serial.print(analogRead(A6)); Serial.print(" ");
    // Serial.print(analogRead(A7)); Serial.print(" | ");
    // Serial.println(memory);

    Serial.print(analogRead(A0) > THRESHOLD); Serial.print(" ");
    Serial.print(analogRead(A1) > THRESHOLD); Serial.print(" ");
    Serial.print(analogRead(A2) > THRESHOLD); Serial.print(" ");
    Serial.print(analogRead(A3) > THRESHOLD); Serial.print(" ");
    Serial.print(analogRead(A4) > THRESHOLD); Serial.print(" ");
    Serial.print(analogRead(A5) > THRESHOLD); Serial.print(" ");
    Serial.print(analogRead(A6) > THRESHOLD); Serial.print(" ");
    Serial.print(analogRead(A7) > THRESHOLD); Serial.print(" | ");
    Serial.println(memory);
    counter = 0;
  }
}

// Loop
void loop() {

  sensor();
  drawSensor(); // Debug
}

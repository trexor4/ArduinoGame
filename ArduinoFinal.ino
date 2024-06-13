#include <Adafruit_CircuitPlayground.h>
#include <math.h>

#define LEFT_BUTTON_PIN 4
#define RIGHT_BUTTON_PIN 5
#define SWITCH_PIN 7

// Define game states
#define LEVEL1 1
#define LEVEL2 2
#define FINAL 3

int currentLevel = LEVEL1;
int score = 0;
bool gameWon = false;

// Accelerometer threshold for dice roll and shake detection
#define ROLL_THRESHOLD 30
#define SHAKE_THRESHOLD 50

float X, Y, Z, totalAccel;

// Dice pixel patterns
uint8_t dicePixels[6][6] = {
  { 2, 0, 0, 0, 0, 0 },
  { 4, 9, 0, 0, 0, 0 },
  { 0, 4, 7, 0, 0, 0 },
  { 1, 3, 6, 8, 0, 0 },
  { 0, 2, 4, 5, 9, 0 },
  { 0, 2, 4, 5, 7, 9 }
};

// Function prototypes
void leftButtonISR();
void rightButtonISR();
void sparkle(int speed, int len);
void playTone(float vroom);
void displayDiceRoll(int rollNumber);
void displayShakeStrength(int strength);
void winGame();
void displayScore();

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  CircuitPlayground.setAccelRange(LIS3DH_RANGE_8_G);
  
  pinMode(LEFT_BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLDOWN);

  // Attach interrupts to buttons
  attachInterrupt(digitalPinToInterrupt(LEFT_BUTTON_PIN), leftButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_BUTTON_PIN), rightButtonISR, FALLING);
}

void loop() {
  if (gameWon) {
    winGame();
    return;
  }
  
  // Compute total acceleration
  X = 0;
  Y = 0;
  Z = 0;
  for (int i = 0; i < 10; i++) {
    X += CircuitPlayground.motionX();
    Y += CircuitPlayground.motionY();
    Z += CircuitPlayground.motionZ();
    delay(1);
  }
  X /= 10;
  Y /= 10;
  Z /= 10;

  totalAccel = sqrt(X * X + Y * Y + Z * Z);
  
  switch (currentLevel) {
    case LEVEL1:
      if (totalAccel > ROLL_THRESHOLD) {
        int rollNumber = random(1, 7); // Roll a number from 1 to 6
        displayDiceRoll(rollNumber);
        if (rollNumber == 6) {
          score++;
          displayScore();
        }
      }
      break;
    case LEVEL2:
      if (totalAccel > SHAKE_THRESHOLD) {
        int shakeStrength = map(totalAccel, SHAKE_THRESHOLD, 100, 1, 10);
        displayShakeStrength(shakeStrength);
        if (shakeStrength >= 7) {
          score++;
          displayScore();
        }
      }
      break;
  }

  delay(100);
}

void leftButtonISR() {
  delay(20);
  if (currentLevel > LEVEL1) {
    currentLevel--;
  }
  Serial.println("Left button pressed");
}

void rightButtonISR() {
  delay(20);
  if (currentLevel < FINAL) {
    currentLevel++;
  } else if (currentLevel == FINAL && score == 2) {
    gameWon = true;
  }
  Serial.println("Right button pressed");
}

void sparkle(int speed, int len) {
  for (int i = 0; i < 5; i++) {
    int randomIndex = random(len);
    speed = random(1, 4);
    CircuitPlayground.setPixelColor(randomIndex, 255, 255, 255);
    delay(speed * 100);
    CircuitPlayground.clearPixels();
  }
}

void playTone(float vroom) {
  CircuitPlayground.playTone(vroom, 200);
}

void displayDiceRoll(int rollNumber) {
  CircuitPlayground.clearPixels();
  for (int p = 0; p < rollNumber; p++) {
    CircuitPlayground.setPixelColor(dicePixels[rollNumber - 1][p], 0xEA6292);
  }
  CircuitPlayground.playTone(1000, 100);
  delay(500);
  CircuitPlayground.clearPixels();
}

void displayShakeStrength(int strength) {
  CircuitPlayground.clearPixels();
  for (int i = 0; i < strength; i++) {
    CircuitPlayground.setPixelColor(i, 0x00FF00);
  }
  CircuitPlayground.playTone(1000, 100);
  delay(500);
  CircuitPlayground.clearPixels();
}

void displayScore() {
  Serial.print("Score: ");
  Serial.println(score);
  CircuitPlayground.clearPixels();
  for (int i = 0; i < score && i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0xFFFF00);  // Yellow color for score display
  }
  delay(1000);
  CircuitPlayground.clearPixels();
}

void winGame() {
  Serial.println("You win!");
  sparkle(2, 10);
  CircuitPlayground.playTone(2000, 1000);
  while (true); // Stop the game
}


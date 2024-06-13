#include <Adafruit_CircuitPlayground.h>
#include <math.h>

#define LEFT_BUTTON_PIN 4
#define RIGHT_BUTTON_PIN 5
#define SWITCH_PIN 7

// Define game states
#define LEVEL1 1
#define LEVEL2 2
#define FINAL 3

int currentLevel = LEVEL1; // Initialize the current level to LEVEL1
int score = 0; // Initialize score to 0
bool gameWon = false; // Initialize gameWon flag to false

// Accelerometer threshold for dice roll and shake detection
#define ROLL_THRESHOLD 30
#define SHAKE_THRESHOLD 50

float X, Y, Z, totalAccel; // Variables to hold accelerometer values

// Dice pixel patterns for each dice face (1 to 6)
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
  Serial.begin(9600); // Initialize serial communication at 9600 bps
  CircuitPlayground.begin(); // Initialize the Circuit Playground
  CircuitPlayground.setAccelRange(LIS3DH_RANGE_8_G); // Set accelerometer range to 8G
  
  pinMode(LEFT_BUTTON_PIN, INPUT_PULLDOWN); // Set left button pin as input with pull-down resistor
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLDOWN); // Set right button pin as input with pull-down resistor

  // Attach interrupts to buttons
  attachInterrupt(digitalPinToInterrupt(LEFT_BUTTON_PIN), leftButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_BUTTON_PIN), rightButtonISR, FALLING);
}

void loop() {
  // Check if the game is won
  if (gameWon) {
    winGame(); // Call winGame function if game is won
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

  totalAccel = sqrt(X * X + Y * Y + Z * Z); // Calculate the total acceleration
  
  // Switch between levels based on currentLevel
  switch (currentLevel) {
    case LEVEL1:
      // Level 1: Dice Roll
      if (totalAccel > ROLL_THRESHOLD) {
        int rollNumber = random(1, 7); // Roll a number from 1 to 6
        displayDiceRoll(rollNumber); // Display the roll number using LEDs
        if (rollNumber == 6) {
          score++; // Increment score if roll number is 6
          displayScore(); // Display the current score
        }
      }
      break;
    case LEVEL2:
      // Level 2: Shake Detection
      if (totalAccel > SHAKE_THRESHOLD) {
        int shakeStrength = map(totalAccel, SHAKE_THRESHOLD, 100, 1, 10); // Map the shake strength to a value between 1 and 10
        displayShakeStrength(shakeStrength); // Display the shake strength using LEDs
        if (shakeStrength >= 7) {
          score++; // Increment score if shake strength is 7 or higher
          displayScore(); // Display the current score
        }
      }
      break;
  }

  delay(100); // Small delay for debounce
}

// ISR for left button press
void leftButtonISR() {
  delay(20); // Debounce delay
  if (currentLevel > LEVEL1) {
    currentLevel--; // Decrement level if not already at the first level
  }
  Serial.println("Left button pressed");
}

// ISR for right button press
void rightButtonISR() {
  delay(20); // Debounce delay
  if (currentLevel < FINAL) {
    currentLevel++; // Increment level if not already at the final level
  } else if (currentLevel == FINAL && score == 2) {
    gameWon = true; // Set gameWon flag if all levels are completed and score is 2
  }
  Serial.println("Right button pressed");
}

// Function to create a sparkle effect on LEDs
void sparkle(int speed, int len) {
  for (int i = 0; i < 5; i++) {
    int randomIndex = random(len);
    speed = random(1, 4);
    CircuitPlayground.setPixelColor(randomIndex, 255, 255, 255);
    delay(speed * 100);
    CircuitPlayground.clearPixels();
  }
}

// Function to play a tone
void playTone(float vroom) {
  CircuitPlayground.playTone(vroom, 200);
}

// Function to display the dice roll using LEDs
void displayDiceRoll(int rollNumber) {
  CircuitPlayground.clearPixels();
  for (int p = 0; p < rollNumber; p++) {
    CircuitPlayground.setPixelColor(dicePixels[rollNumber - 1][p], 0xEA6292);
  }
  CircuitPlayground.playTone(1000, 100);
  delay(500);
  CircuitPlayground.clearPixels();
}

// Function to display the shake strength using LEDs
void displayShakeStrength(int strength) {
  CircuitPlayground.clearPixels();
  for (int i = 0; i < strength; i++) {
    CircuitPlayground.setPixelColor(i, 0x00FF00);
  }
  CircuitPlayground.playTone(1000, 100);
  delay(500);
  CircuitPlayground.clearPixels();
}

// Function to display the current score using LEDs
void displayScore() {
  Serial.print("Score: ");
  Serial.println(score);
  CircuitPlayground.clearPixels();
  for (int i = 0; i < score && i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0xFFFF00); // Yellow color for score display
  }
  delay(1000);
  CircuitPlayground.clearPixels();
}

// Function to handle the winning condition
void winGame() {
  Serial.println("You win!");
  sparkle(2, 10);
  CircuitPlayground.playTone(2000, 1000);
  while (true); // Stop the game
}


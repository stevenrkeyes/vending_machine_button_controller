#include "limits.h"
#include <SoftPWM.h>

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

#define BUTTON_COOLDOWN_MS 700
const int buttonPins[] = {A0, A1, A2, A3, A4, A5, 11}; // Define input pin numbers
const int outputPins[] = {2, 3, 4, 5, 6, 7, 8}; // Define output pin numbers

long lastPressedTimesMs[ARRAY_LENGTH(buttonPins)] = {ULONG_MAX};
// Array of whether each button has ever been pressed (for handling startup)
bool buttonEverPressed[ARRAY_LENGTH(buttonPins)] = {false};

// Returns the y-value of a trapezoid whose x points are 0, t1, t2, t3 and whose height is 255
unsigned char trapezoid(long t1, long t2, long t3, long currentTime) {
    if (currentTime < 0) {
        return 0;
    } else if (currentTime >= 0 && currentTime < t1) {
        // Interpolate on the line from (0, 0) to (t1, 255)
        return (unsigned char)((255.0 * (currentTime - t1)) / (t2 - t1));
    } else if (currentTime >= t1 && currentTime < t2) {
        // Interpolate on the line from (0, 0) to (t1, 255)
        return 255;
    } else if (currentTime >= t2 && currentTime < t3) {
        // Interpolate on the line from (t2, 255) to (t3, 0)
        return (unsigned char)(255 - (255.0 * (currentTime - t2)) / (t3 - t2));
    } else {
        return 0;
    }
}

void setup() {
  // Configure input pins with pull-ups
  for (int i = 0; i < ARRAY_LENGTH(buttonPins); i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Configure output pins to drive LEDs built into buttons
  for (int i = 0; i < ARRAY_LENGTH(outputPins); i++) {
    pinMode(outputPins[i], OUTPUT);
  }

  SoftPWMBegin(SOFTPWM_NORMAL);

  // Initialize serial communication
  Serial.begin(9600);
}

void loop() {
  long currentTimeMs = millis();
  
  // Read buttons
  for (int i = 0; i < ARRAY_LENGTH(buttonPins); i++) {
    int inputValue = digitalRead(buttonPins[i]);
    // Check if button has been pressed, pulling pin low
    if (inputValue == LOW) {
      if (currentTimeMs > lastPressedTimesMs[i] + BUTTON_COOLDOWN_MS) {
        lastPressedTimesMs[i] = currentTimeMs;
        buttonEverPressed[i] = true;
        Serial.println(String(i));
      }
    }
  }

  // Do button animations
  for (int i = 0; i < ARRAY_LENGTH(buttonPins); i++) {
    if (buttonEverPressed[i]) {
      SoftPWMSet(outputPins[i], trapezoid(100, 500, 900, currentTimeMs - lastPressedTimesMs[i]));
    }
  }
}

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TimeLib.h>

// Set the LCD address to 0x27 for a 20 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 2);

// Define button pins
const int increaseMinuteButtonPin = 2; // Pin for increment minute button
const int decreaseMinuteButtonPin = 3; // Pin for decrement minute button
const int increaseHourButtonPin = 4;   // Pin for increment hour button
const int decreaseHourButtonPin = 5;   // Pin for decrement hour button

// Variables to store button state
bool lastIncreaseMinuteButtonState = HIGH;
bool lastDecreaseMinuteButtonState = HIGH;
bool lastIncreaseHourButtonState = HIGH;
bool lastDecreaseHourButtonState = HIGH;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  
  // Set the initial time (e.g., 20:29:30 on August 14, 2024)
  setTime(19, 29, 0, 2, 9, 2024);

  // Initialize button pins
  pinMode(increaseMinuteButtonPin, INPUT_PULLUP);
  pinMode(decreaseMinuteButtonPin, INPUT_PULLUP);
  pinMode(increaseHourButtonPin, INPUT_PULLUP);
  pinMode(decreaseHourButtonPin, INPUT_PULLUP);

  Serial.println("Enter time in the format HH:MM MM/DD/YYYY to set the time.");
}

void loop() {
  // Read the state of the buttons
  bool increaseMinuteButtonState = digitalRead(increaseMinuteButtonPin);
  bool decreaseMinuteButtonState = digitalRead(decreaseMinuteButtonPin);
  bool increaseHourButtonState = digitalRead(increaseHourButtonPin);
  bool decreaseHourButtonState = digitalRead(decreaseHourButtonPin);

  // Check if the increase minute button is pressed (button state is LOW)
  if (lastIncreaseMinuteButtonState == HIGH && increaseMinuteButtonState == LOW) {
    incrementMinutes();
    delay(200); // Debounce delay
  }

  // Check if the decrease minute button is pressed (button state is LOW)
  if (lastDecreaseMinuteButtonState == HIGH && decreaseMinuteButtonState == LOW) {
    decrementMinutes();
    delay(200); // Debounce delay
  }

  // Check if the increase hour button is pressed (button state is LOW)
  if (lastIncreaseHourButtonState == HIGH && increaseHourButtonState == LOW) {
    incrementHours();
    delay(200); // Debounce delay
  }

  // Check if the decrease hour button is pressed (button state is LOW)
  if (lastDecreaseHourButtonState == HIGH && decreaseHourButtonState == LOW) {
    decrementHours();
    delay(200); // Debounce delay
  }

  // Update the button states
  lastIncreaseMinuteButtonState = increaseMinuteButtonState;
  lastDecreaseMinuteButtonState = decreaseMinuteButtonState;
  lastIncreaseHourButtonState = increaseHourButtonState;
  lastDecreaseHourButtonState = decreaseHourButtonState;

  // Display the current time on the LCD
  displayTime();

  // Check for serial input to set time and date
  if (Serial.available()) {
    handleSerialInput();
  }

  // Print the current date and time to the serial monitor for debugging
  Serial.print("Month: ");
  Serial.print(month());
  Serial.print(" Day: ");
  Serial.print(day());
  Serial.print(" Year: ");
  Serial.print(year());
  Serial.print(" Time: ");
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.println();

  // Wait for a short period before updating the display
  delay(100);
}

void incrementMinutes() {
  // Increment minutes
  int currentMinutes = minute();
  currentMinutes++;
  if (currentMinutes >= 60) {
    currentMinutes = 0;
  }
  setTime(hour(), currentMinutes, second(), day(), month(), year());
}

void decrementMinutes() {
  // Decrement minutes
  int currentMinutes = minute();
  currentMinutes--;
  if (currentMinutes < 0) {
    currentMinutes = 59;
  }
  setTime(hour(), currentMinutes, second(), day(), month(), year());
}

void incrementHours() {
  // Increment hours
  int currentHour = hour();
  currentHour++;
  if (currentHour >= 24) {
    currentHour = 0;
  }
  setTime(currentHour, minute(), second(), day(), month(), year());
}

void decrementHours() {
  // Decrement hours
  int currentHour = hour();
  currentHour--;
  if (currentHour < 0) {
    currentHour = 23;
  }
  setTime(currentHour, minute(), second(), day(), month(), year());
}

void displayTime() {
  // Get the current hour in 12-hour format
  int h = hour();
  int displayHour = h % 12;
  if (displayHour == 0) {
    displayHour = 12; // Adjust for 12 AM and 12 PM
  }
  
  // Determine AM or PM
  char* ampm = (h >= 12) ? "PM" : "AM";

  // Format the time as HH:MM AM/PM
  char timeBuffer[12];
  snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d %s", displayHour, minute(), ampm);

  // Display the time on the LCD
  lcd.setCursor(0, 0);
  lcd.print(timeBuffer);

  // Format the date as MM-DD-YYYY
  char dateBuffer[12];
  snprintf(dateBuffer, sizeof(dateBuffer), "%02d-%02d-%04d", month(), day(), year());

  // Display the date on the LCD
  lcd.setCursor(0, 1);
  lcd.print(dateBuffer);
}

void handleSerialInput() {
  // Read serial input
  String input = Serial.readStringUntil('\n');
  input.trim(); // Remove any extra newline or spaces
  
  int inputHour, inputMinute, inputDay, inputMonth, inputYear;
  
  // Parse the input string in the format HH:MM MM/DD/YYYY
  if (sscanf(input.c_str(), "%d:%d %d/%d/%d", &inputHour, &inputMinute, &inputMonth, &inputDay, &inputYear) == 5) {
    if (inputHour >= 0 && inputHour < 24 && inputMinute >= 0 && inputMinute < 60) {
      // Set the new time and date
      setTime(inputHour, inputMinute, 0, inputDay, inputMonth, inputYear);
      Serial.println("Time updated successfully!");
    } else {
      Serial.println("Invalid time format. Please enter the time in the format HH:MM MM/DD/YYYY");
    }
  } else {
    Serial.println("Invalid input. Please enter the time in the format HH:MM MM/DD/YYYY");
  }
}
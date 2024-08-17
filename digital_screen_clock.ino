#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TimeLib.h>

LiquidCrystal_I2C lcd(0x27, 20, 2);

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  // Set an initial time
  setTime(12, 0, 0, 1, 1, 2024);
}

void loop() {
  // Check for serial input
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    handleCommand(command);
  }

  // Display the current time and date
  displayTime();
  delay(100);
}

void handleCommand(String command) {
  // Command format: SET HH MM SS DD MM YYYY
  if (command.startsWith("SET ")) {
    int hour, minute, second, day, month, year;
    sscanf(command.c_str(), "SET %d %d %d %d %d %d", &hour, &minute, &second, &day, &month, &year);
    
    // Ensure valid time and date
    if (hour >= 0 && hour < 24 && minute >= 0 && minute < 60 && second >= 0 && second < 60 &&
        day >= 1 && day <= 31 && month >= 1 && month <= 12 && year >= 2000) {
      setTime(hour, minute, second, day, month, year);
      Serial.println("Time updated successfully.");
    } else {
      Serial.println("Invalid time or date values.");
    }
  } else {
    Serial.println("Unknown command. Use SET HH MM SS DD MM YYYY.");
  }
}

void displayTime() {
  int h = hour();
  int displayHour = h % 12;
  if (displayHour == 0) {
    displayHour = 12;
  }

  char* ampm = (h >= 12) ? "PM" : "AM";

  char timeBuffer[12];
  snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d %s", displayHour, minute(), ampm);

  lcd.setCursor(0, 0);
  lcd.print(timeBuffer);

  char dateBuffer[12];
  snprintf(dateBuffer, sizeof(dateBuffer), "%02d/%02d/%04d", month(), day(), year());

  lcd.setCursor(0, 1);
  lcd.print(dateBuffer);
}

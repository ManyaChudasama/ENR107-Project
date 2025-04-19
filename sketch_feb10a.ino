#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int soundPin = A0;
const int ledPins[] = {3, 4, 5, 6, 7, 8, 10, 11};
const int numLeds = 8;

int activityLevel = 0;
unsigned long lastDecayTime = 0;
float smoothedValue = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Address might be 0x3F for some displays

void setup() {
  pinMode(soundPin, INPUT);

  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("VU Meter Ready");
  delay(1000);
  lcd.clear();
}

void loop() {
  int micValue = analogRead(soundPin);
  smoothedValue = smoothedValue * 0.8 + micValue * 0.2;

  int newLevel = map(smoothedValue, 0, 1023, 0, numLeds);
  newLevel = constrain(newLevel, 0, numLeds);

  if (newLevel > activityLevel) {
    activityLevel = newLevel;
  } else if (millis() - lastDecayTime > 200) {
    activityLevel--;
    if (activityLevel < 0) activityLevel = 0;
    lastDecayTime = millis();
  }

  for (int i = 0; i < numLeds; i++) {
    digitalWrite(ledPins[i], i < activityLevel ? HIGH : LOW);
  }

  // Estimate Decibel (not accurate, just for representation)
  float voltage = (smoothedValue / 1023.0) * 5.0;
  float dB = 20 * log10(voltage);

  lcd.setCursor(0, 0);
  lcd.print("Mic Value: ");
  lcd.print((int)smoothedValue);

  lcd.setCursor(0, 1);
  lcd.print("Est. dB: ");
  lcd.print(dB, 1);
  Serial.println(voltage);
  Serial.println("Est. dB: ");
  Serial.println(dB, 1);

  delay(100);
}

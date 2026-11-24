// Smart Drawer Guard
// NodeMCU ESP8266 + Reed Switch + IR Sensor + Servo + Buzzer + LEDs + Blynk
//
// Before uploading, replace the 3 placeholders below with your own details.

#define BLYNK_TEMPLATE_ID "TMPL3C_oUSYh4"
#define BLYNK_TEMPLATE_NAME "Smart Drawer Guard"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

Servo myServo;

int reedPin = D5;
int irPin = D6;
int buzzerPin = D7;
int greenLED = D1;
int redLED = D2;
int servoPin = D4;

int lockedAngle = 0;
int unlockedAngle = 90;
bool isLocked = true;
bool alertSent = false;

void setup() {
  Serial.begin(115200);

  pinMode(reedPin, INPUT_PULLUP);
  pinMode(irPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  myServo.attach(servoPin);
  myServo.write(lockedAngle);
  isLocked = true;

  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, LOW);
  digitalWrite(buzzerPin, LOW);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Smart Drawer Guard Ready!");
}

void loop() {
  Blynk.run();

  int irValue = digitalRead(irPin);
  int reedValue = digitalRead(reedPin);

  // Hand near IR sensor: unlock for 5 seconds, then lock again
  if (irValue == LOW && isLocked) {
    Serial.println("Hand detected! Unlocking...");
    myServo.write(unlockedAngle);
    isLocked = false;
    delay(5000);

    myServo.write(lockedAngle);
    isLocked = true;
    alertSent = false;
    Serial.println("Auto-locked again.");
  }

  // Drawer opened while locked: alarm + phone notification
  if (isLocked && reedValue == HIGH) {
    Serial.println("ALERT! Unauthorized opening!");
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    digitalWrite(buzzerPin, HIGH);
    delay(300);
    digitalWrite(buzzerPin, LOW);
    delay(300);

    if (!alertSent) {
      Blynk.virtualWrite(V0, "Intruder Alert! Drawer opened without permission!");
      Blynk.logEvent("intruder_alert", "Unauthorized drawer opening detected!");
      alertSent = true;
    }
  } else {
    // Safe state
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
    digitalWrite(buzzerPin, LOW);
    alertSent = false;
  }

  delay(200);
}

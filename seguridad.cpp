/* 
  Sketch generated by the Arduino IoT Cloud Thing "Untitled"
  https://create.arduino.cc/cloud/things/d9e76d66-1b84-4bb4-b9c0-1cd53bd617b5 

  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  bool fire;
  bool gas;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>  // Updated library for ESP32

#include "thingProperties.h"

#define SS_PIN 15  // RFID SDA
#define RST_PIN 5  // RFID RST

MFRC522 mfrc522(SS_PIN, RST_PIN);

#define LED_PIN 2
#define BUZZER_PIN 2

#define FIRE_PIN 13
#define GAS_PIN 14

Servo servo1;
Servo servo2;

void quicklyRotateServo(int servo);

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information youâll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // NEW ------------------------------------------------------------------
  pinMode(FIRE_PIN, INPUT);
  pinMode(GAS_PIN, INPUT);

  SPI.begin(4, 17, 16, 15);
  mfrc522.PCD_Init();

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  servo1.attach(25, 500, 2400);
  servo2.attach(27, 500, 2400);
}

void loop() {
  ArduinoCloud.update();
  // Your code here
  static unsigned long lastCardReadTime = 0;  // Keep track of the last card read time
  unsigned long currentMillis = millis();

  fire = digitalRead(FIRE_PIN) == LOW;
  gas = digitalRead(GAS_PIN) == LOW;
  delay(200);

  if (currentMillis - lastCardReadTime > 2000) {  // Only attempt to read a new card after 2 seconds have passed
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      Serial.println("Card Detected!");

      // Extract UID of the detected card
      tarjeta_NFC = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        tarjeta_NFC += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
        tarjeta_NFC += String(mfrc522.uid.uidByte[i], HEX);
      }
      tarjeta_NFC.toUpperCase();  // Convert to uppercase

      // Print the UID to the Serial Monitor
      Serial.print("Card UID: ");
      Serial.println(tarjeta_NFC);

      if (tarjeta_NFC == "D7A50E1B") {
        quicklyRotateServo(1);  // Move servo if UID matches
      } else {
        for (int i = 0; i < 4; i++) {
          digitalWrite(LED_PIN, (i % 2 == 0) ? HIGH : LOW);
          delay(200);
        }
      }

      lastCardReadTime = currentMillis;  // Update the last card read time
      delay(200);
    }
  }

  if (fire || gas) {
    quicklyRotateServo(1);
    quicklyRotateServo(2);
  }
}

void quicklyRotateServo(int servo) {

  // Play a different tone when moving the servo
  tone(BUZZER_PIN, 2000, 500);  // 2000 Hz for 500 ms

  if (servo == 1) {
    Serial.printf("Servo in motion: %d\n", servo);
    for (int pos = 180; pos >= 0; pos -= 1) {  // sweep from 180 degrees to 0 degrees
      servo2.write(pos);
      delay(20);
    }
  } else if (servo == 2) {
    Serial.printf("Servo in motion: %d\n", servo);
    for (int pos = 0; pos <= 180; pos += 1) {  // sweep from 0 degrees to 180 degrees
      // in steps of 1 degree
      servo1.write(pos);
      delay(20);  // waits 20ms for the servo to reach the position
    }
  }

  // Optional: Play a tone to indicate the servo has returned to its original position
  tone(BUZZER_PIN, 1000, 300);  // 1000 Hz for 300 ms
}

/*
  Since Puerta is READ_WRITE variable, onPuertaChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onPuertaChange() {
  // Add your code here to act upon Puerta change
  if (puerta)
    quicklyRotateServo(1);
}

/*
  Since Pluma is READ_WRITE variable, onPlumaChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onPlumaChange() {
  // Add your code here to act upon Pluma change
  if (pluma)
    quicklyRotateServo(2);
}

void onKeyChange() {
  String fullString = "2024-2025-2122";  // Cadena completa en la que deseas buscar
  if (key.length() == 4 && fullString.indexOf(key) != -1)
    quicklyRotateServo(1);
}

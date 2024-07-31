#include <SoftwareSerial.h>

// Define pin connections for LEDs and relay
const int greenLEDPin = 10; // Pin for green LED
const int redLEDPin = 11;   // Pin for red LED
const int relayPin = 8;     // Pin for relay

const int ledRxPin = 6;     // RX pin for SoftwareSerial
const int ledTxPin = 7;     // TX pin for SoftwareSerial

SoftwareSerial LEDBoard(ledRxPin, ledTxPin); // Software serial for communication with the main board

void setup() {
  pinMode(greenLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);
  pinMode(relayPin, OUTPUT); // Initialize relay pin as output

  Serial.begin(9600);      // Start serial communication with the monitor
  LEDBoard.begin(9600);   // Start communication with the main board

  // Set initial LED states and relay
  digitalWrite(greenLEDPin, LOW);  // Green LED off
  digitalWrite(redLEDPin, HIGH);   // Red LED on
  digitalWrite(relayPin, LOW);     // Relay off
}

void loop() {
  if (LEDBoard.available()) {
    String data = LEDBoard.readStringUntil('\n');
    Serial.println("Received from main board: " + data); // Display received message on Serial Monitor
    
    if (data.indexOf("HelmetDetected") != -1) {
      digitalWrite(greenLEDPin, HIGH); // Turn on green LED
      digitalWrite(redLEDPin, LOW);    // Turn off red LED
      digitalWrite(relayPin, HIGH);    // Turn ON relay
    } else if (data.indexOf("NoDetection") != -1) {
      digitalWrite(greenLEDPin, LOW);  // Turn off green LED
      digitalWrite(redLEDPin, HIGH);   // Turn on red LED
      digitalWrite(relayPin, LOW);     // Turn OFF relay
    } else if (data.indexOf("DETECTION_DISABLED") != -1) {
      digitalWrite(redLEDPin, HIGH);  // Turn on red LED
      digitalWrite(greenLEDPin, LOW); // Ensure green LED is off
      digitalWrite(relayPin, HIGH);   // Turn ON relay

      unsigned long startMillis = millis();
      while (millis() - startMillis < 60000) { // Keep relay on for 1 minute
        if ((millis() - startMillis) % 500 < 250) {
          digitalWrite(redLEDPin, HIGH);  // Blink red LED
        } else {
          digitalWrite(redLEDPin, LOW);
        }
      }
      // After 1 minute
      digitalWrite(redLEDPin, LOW);  // Ensure red LED is off
      digitalWrite(relayPin, LOW);   // Turn OFF relay after 1 minute
    } else if (data.indexOf("DETECTION_ENABLED") != -1) {
      digitalWrite(redLEDPin, LOW);  // Turn off red LED
      digitalWrite(greenLEDPin, LOW); // Turn off green LED
      digitalWrite(relayPin, LOW);    // Ensure relay is OFF
    }
  }
}

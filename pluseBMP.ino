#include <Servo.h>
#include <SoftwareSerial.h>

// Define pin connections
const int relayPin = 5; // Relay pin
const int signalPin = A0; // Potentiometer pin
const int rxPin = 3; // HC-05 RX pin
const int txPin = 4; // HC-05 TX pin
const int btStatePin = 12; // Bluetooth state pin
const int ledRxPin = 6; // LED board RX pin
const int ledTxPin = 7; // LED board TX pin

Servo myServo; // Servo object
SoftwareSerial BTSerial(rxPin, txPin); // HC-05 Bluetooth module
SoftwareSerial LEDSerial(ledRxPin, ledTxPin); // LED board

bool isConnected = false; // Bluetooth connection status
bool isRelayOn = false; // Relay status

void setup() {
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Start with relay off

  pinMode(signalPin, INPUT);
  pinMode(btStatePin, INPUT);

  Serial.begin(9600); // Start serial communication with monitor
  BTSerial.begin(9600); // Start HC-05 communication
  LEDSerial.begin(9600); // Start LED board communication

  myServo.attach(9); // Attach servo to pin 9

  LEDSerial.println("DISCONNECTED"); // Initial LED state
}

void loop() {
  readPotentiometerAndControlServo();
  checkBluetoothAndControlRelayAndLED();
}

// Function to read potentiometer and control servo
void readPotentiometerAndControlServo() {
  int signalValue = analogRead(signalPin); // Read potentiometer value
  int servoAngle = map(signalValue, 179, 872, 0, 180); // Map value to servo angle

  Serial.print("Signal Value: ");
  Serial.println(signalValue);
  Serial.print("Servo Angle: ");
  Serial.println(servoAngle);

  myServo.write(servoAngle); // Set servo angle
}

// Function to check Bluetooth state and control relay and LED
void checkBluetoothAndControlRelayAndLED() {
  bool currentBtState = digitalRead(btStatePin) == HIGH;

  if (currentBtState && !isConnected) {
    LEDSerial.println("CONNECTED");
    isConnected = true;
    isRelayOn = false; // Reset relay status when connected
  } else if (!currentBtState && isConnected) {
    LEDSerial.println("DISCONNECTED");
    isConnected = false;
    digitalWrite(relayPin, LOW); // Turn off relay
  }

  if (isConnected && BTSerial.available()) {
    String data = BTSerial.readStringUntil('\n'); // Read data from HC-05
    Serial.println("Received from HC-05: " + data);
    if (data.indexOf("mama") != -1 && !isRelayOn) {
      digitalWrite(relayPin, HIGH); // Turn on relay
      LEDSerial.println("RELAY_ON"); // Notify LED board
      isRelayOn = true;
    }
  }
}

#include <Servo.h>

// Define pin connections
const int btStatePin = 12;     // Bluetooth state pin
const int disableButtonPin = 7; // Pin for disabling detection

const int servoPin = 8;        // Servo pin
const int redLEDPin = 13;      // Pin for red LED
const int greenLEDPin = 14;    // Pin for green LED

Servo myServo; // Servo object

bool isConnected = false; // Bluetooth connection status
bool isDetectionDisabled = false; // Status of detection disable
unsigned long disableStartTime = 0; // Start time when detection is disabled
const unsigned long disableDuration = 60000; // 1 minute duration

void setup() {
  // Initialize pins
  pinMode(btStatePin, INPUT);
  pinMode(disableButtonPin, INPUT_PULLUP); // Use pull-up resistor for button
  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);

  // Start serial communications
  Serial.begin(9600);      // Start serial communication with monitor
  Serial3.begin(9600);     // Start HC-06 communication on Serial3
  Serial2.begin(9600);     // Start LED board communication on Serial2

  myServo.attach(servoPin); // Attach servo to the specified pin

  // Initial LED state
  Serial2.println("DISCONNECTED");
  Serial.println("Setup complete. Ready to receive data.");
}

void loop() {
  readPotentiometerAndControlServo();
  checkBluetoothAndControlLED();
  handleSerialCommunication();
  checkDisableButton(); // Check button status

  // Blink LEDs if detection is disabled
  if (isDetectionDisabled) {
    blinkLEDsWhileDisabled();
  }
}

void readPotentiometerAndControlServo() {
  int signalValue = analogRead(A0); // Read potentiometer value
  int servoAngle = map(signalValue, 179, 872, 0, 180); // Map value to servo angle
  servoAngle = constrain(servoAngle, 0, 180);
  myServo.write(servoAngle); // Set servo angle
}

void checkBluetoothAndControlLED() {
  bool currentBtState = digitalRead(btStatePin) == HIGH;

  if (currentBtState && !isConnected) {
    // Bluetooth connected
    Serial2.println("CONNECTED");
    Serial.println("Sent to LED Board: CONNECTED");
    isConnected = true;
  } else if (!currentBtState && isConnected) {
    // Bluetooth disconnected
    Serial2.println("DISCONNECTED");
    Serial.println("Sent to LED Board: DISCONNECTED");
    isConnected = false;
    digitalWrite(greenLEDPin, LOW); // Ensure green LED is off
    digitalWrite(redLEDPin, HIGH);  // Ensure red LED is on
  }

  if (isConnected) {
    while (Serial3.available()) {
      String data = Serial3.readStringUntil('\n'); // Read data from HC-06
      Serial.print("Debug: Received data from HC-06: ");
      Serial.println(data);
      processBluetoothData(data);
    }
  }
}

void processBluetoothData(const String& data) {
  Serial.print("Received from HC-06: ");
  Serial.println(data);

  // Forward data to LED board
  Serial2.print("Received from HC-06: ");
  Serial2.println(data);

  if (data.indexOf("HelmetDetected") != -1) {
    // Helmet detected
    digitalWrite(greenLEDPin, HIGH); // Turn on green LED
    digitalWrite(redLEDPin, LOW);    // Turn off red LED
    Serial2.println("GREEN_ON");
  } else if (data.indexOf("NoDetection") != -1) {
    // No helmet detected
    digitalWrite(greenLEDPin, LOW);  // Turn off green LED
    digitalWrite(redLEDPin, HIGH);   // Turn on red LED
    Serial2.println("RED_ON");
  } else if (data.indexOf("DETECTION_DISABLED") != -1) {
    isDetectionDisabled = true; // Enable detection disable
    disableStartTime = millis(); // Record start time
    Serial.println("Detection Disabled");
    Serial2.println("DETECTION_DISABLED");
    // Send command to board 2 to activate relay for 1 minute
    Serial2.println("ACTIVATE_RELAY");
  } else if (data.indexOf("DETECTION_ENABLED") != -1) {
    isDetectionDisabled = false; // Disable detection disable
    digitalWrite(redLEDPin, LOW);  // Turn off red LED
    digitalWrite(greenLEDPin, LOW); // Turn off green LED
    Serial2.println("BLUE_OFF");
  }
}

void handleSerialCommunication() {
  // Forward data between Serial Monitor, HC-06, and LED board
  if (Serial3.available()) {
    char c = Serial3.read();
    Serial.print(c);
    Serial2.print(c);
  }

  if (Serial.available()) {
    char c = Serial.read();
    Serial3.print(c);
    Serial2.print(c);
  }
}

void checkDisableButton() {
  if (digitalRead(disableButtonPin) == LOW) {
    isDetectionDisabled = !isDetectionDisabled; // Toggle detection disable status
    delay(500); // Debounce delay
    Serial.println(isDetectionDisabled ? "Detection Disabled" : "Detection Enabled");
    Serial2.println(isDetectionDisabled ? "DETECTION_DISABLED" : "DETECTION_ENABLED");
    if (isDetectionDisabled) {
      disableStartTime = millis(); // Record start time
      // Send command to board 2 to activate relay for 1 minute
      Serial2.println("ACTIVATE_RELAY");
    }
  }
}

void blinkLEDsWhileDisabled() {
  unsigned long currentMillis = millis();
  if (currentMillis - disableStartTime < disableDuration) {
    // Blink LEDs
    if ((currentMillis / 500) % 2 == 0) {
      digitalWrite(redLEDPin, HIGH);  // Red LED on
      digitalWrite(greenLEDPin, LOW); // Green LED off
    } else {
      digitalWrite(redLEDPin, LOW);   // Red LED off
      digitalWrite(greenLEDPin, HIGH); // Green LED on
    }
  } else {
    // Turn off LEDs after 1 minute
    digitalWrite(redLEDPin, LOW);
    digitalWrite(greenLEDPin, LOW);
    isDetectionDisabled = false; // Reset disable status
    Serial.println("Detection Enabled");
    Serial2.println("DETECTION_ENABLED");
  }
}

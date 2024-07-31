const int relayPin = 5; // Relay pin

void setup() {
  pinMode(relayPin, OUTPUT);
}

void loop() {
  digitalWrite(relayPin, LOW);  // Turn relay ON
  delay(1000);                  // Wait for 1 second
  digitalWrite(relayPin, HIGH); // Turn relay OFF
  delay(1000);                  // Wait for 1 second
}

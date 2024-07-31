#include <SoftwareSerial.h>

SoftwareSerial BTSerial(10, 11); // RX, TX

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  Serial.println("Slave ready");
}

void loop() {
  if (Serial.available()) {
    BTSerial.write(Serial.read());
  }
  if (BTSerial.available()) {
    Serial.write(BTSerial.read());
  }
}

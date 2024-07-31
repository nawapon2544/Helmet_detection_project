#include <SoftwareSerial.h>

SoftwareSerial BTSerial(10, 11); // RX, TX
const int ledPin = 13;

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  pinMode(ledPin, OUTPUT); // ตั้งค่าขา ledPin เป็น output
  digitalWrite(ledPin, LOW); // ปิด LED ในตอนเริ่มต้น
  Serial.println("BTSerial Started");
}

void loop() {
  if (BTSerial.available()) {
    String command = BTSerial.readStringUntil('\n'); // อ่านคำสั่งจาก Bluetooth

    if (command == "PULSE") {
      digitalWrite(ledPin, HIGH); // เปิด LED
      Serial.println("Received: PULSE"); // แสดงข้อความที่ได้รับบน Serial Monitor
      delay(500); // เปิด LED ค้างไว้สักครู่
      digitalWrite(ledPin, LOW); // ปิด LED
    } else {
      Serial.println("Received unknown command: " + command); // แสดงข้อความที่ได้รับบน Serial Monitor หากไม่ใช่ "PULSE"
    }
  }
}

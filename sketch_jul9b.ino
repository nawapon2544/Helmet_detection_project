#include <SoftwareSerial.h>

// กำหนดพอร์ตอนุกรมสำหรับ HC-05
SoftwareSerial bluetoothHC05(10, 11); // RX, TX

void setup() {
  // เริ่มการสื่อสารกับ Serial Monitor
  Serial.begin(9600);
  Serial.println("Initializing HC-05...");

  // เริ่มการสื่อสารกับ HC-05
  bluetoothHC05.begin(9600);

  Serial.println("HC-05 Ready");
}

void loop() {
  // ส่งข้อความไปยัง HC-06
  bluetoothHC05.write("Hello from HC-05");
  Serial.println("Message sent to HC-06: Hello from HC-05");

  // หน่วงเวลาเพื่อไม่ให้ส่งข้อความถี่เกินไป
  delay(2000);
}

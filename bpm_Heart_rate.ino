#include <SoftwareSerial.h>

const int pulsePin = A0;    // ขาที่เชื่อมต่อ Pulse Sensor
const int ledPin = 8;       // ขาที่เชื่อมต่อ LED (สำหรับแสดงสถานะการทำงาน)
const int bpmThreshold = 60; // ค่า BPM Threshold สำหรับตรวจจับการเต้นของหัวใจ

const int switchPin = A1;   // ขาที่เชื่อมต่อสวิตช์
bool isChecking = false;    // ตัวแปรสถานะเพื่อตรวจสอบว่ากำลังตรวจสอบหรือไม่

SoftwareSerial bluetooth(5, 4);  // ขา RX, TX ของโมดูลบลูทูธ

// ตัวแปรสำหรับติดตามสถานะล่าสุด
bool wasDetecting = false;
bool wasChecking = false;

// ตัวแปรสำหรับคำนวณ BPM
unsigned long lastBeatTime = 0;
unsigned long beatInterval = 0;
int bpm = 0;

void setup() {
  pinMode(ledPin, OUTPUT);        // กำหนดขา LED เป็น OUTPUT
  pinMode(switchPin, INPUT_PULLUP); // กำหนดขาสวิตช์เป็น INPUT พร้อมเปิดใช้งาน pull-up resistor

  Serial.begin(9600);             // เริ่มต้นการสื่อสารผ่าน Serial ที่ความเร็ว 9600 baud
  bluetooth.begin(9600);          // เริ่มต้นการสื่อสารกับโมดูลบลูทูธที่ความเร็ว 9600 baud

  // กำหนดค่าเริ่มต้น
  digitalWrite(ledPin, LOW);    // ปิด LED
}

void loop() {
  // อ่านค่าแอนาล็อกจาก Pulse Sensor
  int sensorValue = analogRead(pulsePin);

  // ตรวจสอบสถานะของสวิตช์
  if (digitalRead(switchPin) == LOW) {
    isChecking = true; // เริ่มตรวจสอบชีพจรเมื่อกดสวิตช์
  } else {
    isChecking = false; // หยุดการตรวจสอบชีพจรเมื่อไม่กดสวิตช์
    digitalWrite(ledPin, LOW); // ปิด LED เมื่อไม่ได้กดสวิตช์เพื่อตรวจสอบ
    if (wasChecking) {
      sendBluetoothSignal("NoDetection"); // ส่งสัญญาณ "NoDetection" เมื่อปล่อยสวิตช์
      wasChecking = false; // อัปเดตสถานะล่าสุด
    }
    wasDetecting = false; // รีเซ็ตสถานะการตรวจจับเมื่อไม่ได้ตรวจสอบ
  }

  // ตรวจสอบชีพจรเมื่อกำลังตรวจสอบ
  if (isChecking) {
    // ตรวจสอบการเต้นของหัวใจ (ใช้การเปลี่ยนแปลงของค่าเซ็นเซอร์เพื่อระบุการเต้น) 
    static int previousSensorValue = 0;
    if (sensorValue > 400 && previousSensorValue <= 400) {
      // ระบุการเต้นของหัวใจ
      unsigned long currentTime = millis();
      beatInterval = currentTime - lastBeatTime;
      lastBeatTime = currentTime;

      // คำนวณ BPM
      bpm = 60000 / beatInterval;

      if (bpm > bpmThreshold) {
        if (!wasDetecting) {
          digitalWrite(ledPin, HIGH);  // เปิด LED เพื่อแสดงสถานะตรวจจับการเต้น
          sendBluetoothSignal("HelmetDetected"); // ส่งสัญญาณผ่านบลูทูธเมื่อตรวจจับการเต้น
          wasDetecting = true; // อัปเดตสถานะล่าสุด
        }
      } else {
        if (wasDetecting) {
          digitalWrite(ledPin, LOW); // ปิด LED เมื่อไม่มีการเต้น
          sendBluetoothSignal("NoDetection"); // ส่งสัญญาณผ่านบลูทูธเมื่อไม่มีการตรวจจับการเต้น
          wasDetecting = false; // อัปเดตสถานะล่าสุด
        }
      }
    }
    previousSensorValue = sensorValue;

    if (!wasChecking) {
      wasChecking = true; // อัปเดตสถานะล่าสุด
    }
  } else {
    blinkRedLED(); // ทำให้ LED กระพริบสีแดงเมื่อไม่ตรวจสอบชีพจร
  }

  delay(10);  // รอเวลาสั้นๆ เพื่อความเสถียร
}

// ฟังก์ชันสำหรับส่งสัญญาณผ่านบลูทูธ
void sendBluetoothSignal(String message) {
  Serial.print("ส่งสัญญาณบลูทูธ: ");
  Serial.println(message);
  bluetooth.println(message);
}

// ฟังก์ชันสำหรับทำให้ LED กระพริบสีแดง
void blinkRedLED() {
  static unsigned long previousMillis = 0;
  const long interval = 500; // Blinking interval in milliseconds

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    int state = digitalRead(ledPin);
    digitalWrite(ledPin, !state); // Toggle LED state
  }
}

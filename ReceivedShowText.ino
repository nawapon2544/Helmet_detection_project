void setup() {
  Serial.begin(9600);      // Serial monitor
  Serial3.begin(9600);     // HC-06 communication
}

void loop() {
  // อ่านข้อมูลจาก HC-06 และส่งไปที่ Serial Monitor
  if (Serial3.available()) {
    char c = Serial3.read();
    Serial.print(c);
  }

  // อ่านข้อมูลจาก Serial Monitor และส่งไปที่ HC-06
  if (Serial.available()) {
    char c = Serial.read();
    Serial3.print(c);
  }
}

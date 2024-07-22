#include <HardwareSerial.h>

const int DE_RE_PIN = 4; // DE and RE pin for RS485

HardwareSerial MySerial(1);

void setup() {
  pinMode(DE_RE_PIN, OUTPUT);
  MySerial.begin(9600, SERIAL_8N1, 16, 17); // RX = 16, TX = 17
  digitalWrite(DE_RE_PIN, LOW); // Enable reception mode
  Serial.begin(115200);
}

void loop() {
  if (MySerial.available()) {
    String receivedData = MySerial.readStringUntil('\n');
    int commaIndex = receivedData.indexOf(',');
    String distanceStr = receivedData.substring(0, commaIndex);
    String waterLevelPerStr = receivedData.substring(commaIndex + 1);

    int distance = distanceStr.toInt();
    int waterLevelPer = waterLevelPerStr.toInt();

    Serial.print("Received Distance = ");
    Serial.print(distance);
    Serial.println(" cm");
    Serial.print("Water Level Percentage = ");
    Serial.println(waterLevelPer);
  }
}

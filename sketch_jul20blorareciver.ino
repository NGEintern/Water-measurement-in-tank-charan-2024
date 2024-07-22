#include <SPI.h>
#include <LoRa.h>

#define BuzzerPin 12  // D12

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  pinMode(BuzzerPin, OUTPUT);
  Serial.println("LoRa Receiver");

  // Setup LoRa receiver
  LoRa.setPins(4, 5, 2);
  while (!LoRa.begin(433E6)) {
    Serial.println(".");
    delay(500);
  }

  LoRa.setSyncWord(0xF1);
  Serial.println("LoRa Initializing Successful!");
}

void loop() {
  // Try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Received a packet
    Serial.print("Received packet: ");

    // Read packet
    String receivedText = "";
    while (LoRa.available()) {
      receivedText += (char)LoRa.read();
    }
    Serial.println(receivedText);

    // Parse the received message
    int commaIndex = receivedText.indexOf(',');
    if (commaIndex != -1) {
      String distanceStr = receivedText.substring(0, commaIndex);
      String waterLevelStr = receivedText.substring(commaIndex + 1);

      int receivedDistance = distanceStr.toInt();
      int receivedWaterLevel = waterLevelStr.toInt();

      Serial.print("Distance: ");
      Serial.print(receivedDistance);
      Serial.println(" cm");

      Serial.print("Water Level: ");
      Serial.print(receivedWaterLevel);
      Serial.println(" %");

      // Example for buzzer control based on received water level
      if (receivedWaterLevel > 80) {
        digitalWrite(BuzzerPin, HIGH);
        Serial.println("Buzzer is turned on");
      } else {
        digitalWrite(BuzzerPin, LOW);
        Serial.println("Buzzer is turned off");
      }
    } else {
      Serial.println("Error: Invalid data received");
    }

     Serial.print(" with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}

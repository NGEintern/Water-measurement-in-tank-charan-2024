#include <Preferences.h>
#include <SPI.h>
#include <LoRa.h>

#define BuzzerPin 12  // D12
#define trigPin 25  //rx of sensor
#define echoPin 26  //tx

int distance;
int emptyTankDistance = 0;
int fullTankDistance = 21; // Assuming 21 is the distance when the tank is full

Preferences preferences;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(BuzzerPin, OUTPUT);

  // Debug console
  Serial.begin(115200);

  // Start the Preferences library
  preferences.begin("waterTank", false);

  // Check if the empty tank distance is already stored in NVS
  if (preferences.isKey("emptyTankDist")) {
    emptyTankDistance = preferences.getInt("emptyTankDist");
    Serial.print("Loaded empty tank distance from NVS: ");
    Serial.println(emptyTankDistance);
  } else {
    // Measure and store the empty tank distance
    emptyTankDistance = measureDistance();
    preferences.putInt("emptyTankDist", emptyTankDistance);
    Serial.print("Measured and stored empty tank distance: ");
    Serial.println(emptyTankDistance);
  }

  // Setup LoRa
  LoRa.setPins(4, 5, 2);
  while (!LoRa.begin(433E6)) {
    Serial.println("LoRa Initializing...");
    delay(500);
  }
  LoRa.setSyncWord(0xF1);
  Serial.println("LoRa Initializing Successful!");
}

void loop() {
  // Clear the trigPin by setting it LOW:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);

  // Trigger the sensor by setting the trigPin high for 10 microseconds:
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
  int duration = pulseIn(echoPin, HIGH);

  // Calculate the distance:
  distance = (duration / 2) / 29.1;

  int waterLevelPer = map(distance, emptyTankDistance, fullTankDistance, 0, 100);

  // Print the distance on the Serial Monitor
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("Water Level = ");
  Serial.print(waterLevelPer);
  Serial.println(" %");

  // Create the message to send
  String message = String(distance) + "," + String(waterLevelPer);

  // Send the distance and water level percentage over LoRa
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
  Serial.println("Data sent via LoRa: " + message);

  // Example for buzzer control based on distance
  if (distance > emptyTankDistance + 45) {
    digitalWrite(BuzzerPin, HIGH);
    Serial.println("Buzzer is turned on");
  } else if (distance < emptyTankDistance + 21) {
    digitalWrite(BuzzerPin, LOW);
  }

  delay(1000);
}

int measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  int duration = pulseIn(echoPin, HIGH);
  return (duration / 2) / 29.1;
}

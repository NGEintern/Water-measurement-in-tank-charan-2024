#include <WiFi.h>
#include <WiFiClient.h>
#include <HardwareSerial.h>
#include <Preferences.h>

const int trigPin = 25;
const int echoPin = 26;
const int BuzzerPin = 12;
const int DE_RE_PIN = 4; // DE and RE pin for RS485

int distance;
int emptyTankDistance = 0;
int fullTankDistance = 21;
Preferences preferences;
HardwareSerial MySerial(1);

char ssid[] = "NGE_Airtel";
char pass[] = "Nge@12345";

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(BuzzerPin, OUTPUT);
  pinMode(DE_RE_PIN, OUTPUT);

  Serial.begin(115200);
  MySerial.begin(9600, SERIAL_8N1, 16, 17); // RX = 16, TX = 17
  preferences.begin("waterTank", false);

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

  // Connect to WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
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
  int distance = (duration / 2) / 29.1;
  int waterLevelPer = map(distance, emptyTankDistance, fullTankDistance, 0, 100);

  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.println(waterLevelPer);


  if (distance > emptyTankDistance + 45) {
    digitalWrite(BuzzerPin, HIGH);
    Serial.println("Buzzer is turned on");
  } else if (distance < emptyTankDistance + 21) {
    digitalWrite(BuzzerPin, LOW);
  }

  // Send data via RS485
  digitalWrite(DE_RE_PIN, HIGH); // Enable transmission mode
  MySerial.print(distance);
  MySerial.print(",");
  MySerial.println(waterLevelPer);
  delay(1000);
  digitalWrite(DE_RE_PIN, LOW); // Enable reception mode

  delay(1000); // Adjust delay as needed
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
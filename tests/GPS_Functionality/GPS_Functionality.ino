#include <TinyGPSPlus.h>

TinyGPSPlus gps;  // GPS object for parsing data

const int ppsPin = 17; // PPS signal pin from GPS module
volatile bool ppsTriggered = false; // To track PPS signal

void ppsISR() {
  // Interrupt Service Routine for PPS signal
  ppsTriggered = true;
}

void setup() {
  Serial.begin(115200);       // Serial monitor for output
  Serial1.begin(9600);        // Initialize GPS communication on UART1 (NEO M9N default is 9600 baud)

  // Configure the PPS pin as input
  pinMode(ppsPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(ppsPin), ppsISR, RISING);  // Attach interrupt to PPS pin

  Serial.println("GPS Functionality Test Started...");
}

void loop() {
  // Check for PPS signal
  if (ppsTriggered) {
    ppsTriggered = false;
    Serial.println("PPS signal received: Accurate second timestamping");
  }

  // Read data from GPS module (UART1)
  while (Serial1.available()) {
    char c = Serial1.read();
    gps.encode(c);  // Feed the GPS parser
  }

  // If valid GPS data has been obtained, print it
  if (gps.location.isValid()) {
    Serial.print("Latitude: "); Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: "); Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude (meters): "); Serial.println(gps.altitude.meters());
    Serial.print("Speed (km/h): "); Serial.println(gps.speed.kmph());
  } else {
    Serial.println("Waiting for valid GPS data...");
  }

  delay(1000);  // Adjust delay as needed
}

// Define LED pins
const int ledLogging = 13;   // LED1: Logging Status
const int ledCANActivity = 14; // LED2: CAN Bus Activity
const int ledError = 15;     // LED3: Error Indicator
const int ledGPSLock = 16;   // LED4: GPS Lock Status

bool isLogging = false;      // Variable to track logging status
bool canBusActive = false;   // Variable to track CAN activity
bool errorOccurred = false;  // Variable to track errors
bool gpsHasLock = false;     // Variable to track GPS lock status

void setup() {
  Serial.begin(115200);

  // Initialize all LEDs as output
  pinMode(ledLogging, OUTPUT);
  pinMode(ledCANActivity, OUTPUT);
  pinMode(ledError, OUTPUT);
  pinMode(ledGPSLock, OUTPUT);

  // Initialize all LEDs to OFF
  digitalWrite(ledLogging, LOW);
  digitalWrite(ledCANActivity, LOW);
  digitalWrite(ledError, LOW);
  digitalWrite(ledGPSLock, LOW);
}

void loop() {
  // Simulated Logging Status
  if (isLogging) {
    digitalWrite(ledLogging, HIGH);  // Turn on logging LED
  } else {
    digitalWrite(ledLogging, LOW);   // Turn off logging LED
  }

  // Simulated CAN Activity Status (BLINK LED2 on activity)
  if (canBusActive) {
    digitalWrite(ledCANActivity, HIGH);
    delay(200);  // Blink interval
    digitalWrite(ledCANActivity, LOW);
    delay(200);
  }

  // Simulated Error Status
  if (errorOccurred) {
    digitalWrite(ledError, HIGH);  // Turn on error LED
  } else {
    digitalWrite(ledError, LOW);   // Turn off error LED
  }

  // Simulated GPS Lock Status
  if (gpsHasLock) {
    digitalWrite(ledGPSLock, HIGH);  // Turn on GPS lock LED
  } else {
    digitalWrite(ledGPSLock, LOW);   // Turn off GPS lock LED
  }

  // Simulate some logic to toggle these variables for testing
  simulateStatusChanges();

  delay(500);  // Adjust delay as needed
}

// Simulate changes in status for testing
void simulateStatusChanges() {
  // Simulate toggling the logging status
  isLogging = !isLogging;
  
  // Simulate CAN bus activity
  canBusActive = !canBusActive;

  // Simulate error occurrence
  errorOccurred = (random(0, 2) == 1);  // Randomly simulate error

  // Simulate GPS lock acquisition
  gpsHasLock = (random(0, 2) == 1);  // Randomly simulate GPS lock
}

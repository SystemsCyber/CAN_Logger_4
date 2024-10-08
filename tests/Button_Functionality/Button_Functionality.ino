// Define pins for buttons and LED
const int startButtonPin = 2;  // Button to start logging
const int stopButtonPin = 3;   // Button to stop logging
const int ledPin = 13;         // LED to indicate logging status

bool isLogging = false;        // Variable to track logging status

void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(115200);

  // Configure button pins as inputs with pull-down resistors
  pinMode(startButtonPin, INPUT_PULLDOWN);
  pinMode(stopButtonPin, INPUT_PULLDOWN);

  // Configure LED pin as output
  pinMode(ledPin, OUTPUT);

  // Start with LED off (not logging)
  digitalWrite(ledPin, LOW);
}

void loop() {
  // Check if the start button is pressed
  if (digitalRead(startButtonPin) == HIGH && !isLogging) {
    // Debounce: Wait for button release
    delay(50);
    if (digitalRead(startButtonPin) == HIGH) {
      // Start logging process
      startLogging();
    }
  }

  // Check if the stop button is pressed
  if (digitalRead(stopButtonPin) == HIGH && isLogging) {
    // Debounce: Wait for button release
    delay(50);
    if (digitalRead(stopButtonPin) == HIGH) {
      // Stop logging process
      stopLogging();
    }
  }

  delay(10);  // Short delay to avoid excessive polling
}

void startLogging() {
  // Indicate logging has started
  isLogging = true;
  digitalWrite(ledPin, HIGH);  // Turn on the LED
  Serial.println("Logging Started!");
  // Add code here to start logging CAN data to SD card
}

void stopLogging() {
  // Indicate logging has stopped
  isLogging = false;
  digitalWrite(ledPin, LOW);  // Turn off the LED
  Serial.println("Logging Stopped!");
  // Add code here to stop logging CAN data to SD card
}

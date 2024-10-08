const int ppsPin = 17;  // Pin connected to PPS signal
volatile bool ppsTriggered = false;
volatile unsigned long ppsCount = 0;  // Counter to track number of PPS signals received

void ppsISR() {
  // Interrupt Service Routine triggered by PPS signal
  ppsTriggered = true;
  ppsCount++;  // Increment PPS count
}

void setup() {
  Serial.begin(115200);
  
  // Configure the PPS pin as an input
  pinMode(ppsPin, INPUT);
  
  // Attach an interrupt to the PPS pin (rising edge of the pulse)
  attachInterrupt(digitalPinToInterrupt(ppsPin), ppsISR, RISING);
  
  Serial.println("PPS Time Synchronization Test Started...");
}

void loop() {
  if (ppsTriggered) {
    ppsTriggered = false;  // Reset flag after handling

    // Print the PPS count (1 pulse per second)
    Serial.print("PPS Signal received! Count: ");
    Serial.println(ppsCount);
    
    // Optionally, you can synchronize this with other processes such as data logging or timestamping.
  }

  delay(100);  // Adjust the delay as needed for testing
}

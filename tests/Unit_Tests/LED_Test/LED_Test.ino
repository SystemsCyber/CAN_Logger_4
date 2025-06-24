void setup() {
  pinMode(9, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {
  digitalWrite(9, HIGH);
  digitalWrite(32, HIGH);
  digitalWrite(14, HIGH);
  digitalWrite(15, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);

  delay(100);

  digitalWrite(9, LOW);
  digitalWrite(32, LOW);
  digitalWrite(14, LOW);
  digitalWrite(15, LOW);
  digitalWrite(LED_BUILTIN, LOW);

  delay(100);

}

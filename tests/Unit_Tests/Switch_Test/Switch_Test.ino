bool switch1;
bool switch2;

void setup() {
  pinMode(36, INPUT_PULLUP);
  pinMode(37, INPUT_PULLUP);

  Serial.begin(9600);
  while(!Serial);

}

void loop() {
  switch1 = digitalRead(36) == LOW;
  switch2 = digitalRead(37) == LOW;

  if(switch1){
    Serial.println("Button 1 is Pressed");
  }

  if(switch2){
    Serial.println("Button 2 is Pressed");
  }

  delay(500);

}

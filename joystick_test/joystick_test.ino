int value = 0;

void setup() {
  // initialize digital pin GPIO18 as an output.
  Serial.begin(115200);  
  pinMode(2, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  value = analogRead(36);
  Serial.print("X: ");
  Serial.print(value, DEC);
  value = analogRead(39);
  if (value >= 2000){
    digitalWrite(2, HIGH);
  } else {
    digitalWrite(2, LOW);
  }
  Serial.print("Y: ");
  Serial.println(value, DEC);
  delay(100);
}

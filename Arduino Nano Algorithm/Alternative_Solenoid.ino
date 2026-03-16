const int solenoid1 = 9;
const int solenoid2 = 10;

void setup() {
  pinMode(solenoid1, OUTPUT);
  pinMode(solenoid2, OUTPUT);
}

void loop() {

  // Solenoid 1 activates
  digitalWrite(solenoid1, HIGH);
  delay(1000);               // push
  digitalWrite(solenoid1, LOW);
  delay(500);                // pull back

  // Solenoid 2 activates
  digitalWrite(solenoid2, HIGH);
  delay(1000);               // push
  digitalWrite(solenoid2, LOW);
  delay(500);                // pull back

}

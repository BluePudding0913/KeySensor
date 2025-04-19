

const int sensorPin     = 35;

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT);
}

void loop() {
  int raw = analogRead(sensorPin);
  delay(1000);
  Serial.print(raw);
  Serial.println(" ");
}
#define LED_PIN                 13

#define GUARD_SENSOR_1_PIN      12

#define CONTROL_LINE_3_PIN      2
#define CONTROL_LINE_4_PIN      3
#define CONTROL_LINE_5_PIN      4

void setup() {
  Serial.begin(9600);  
  pinMode(LED_PIN, OUTPUT);
  pinMode(GUARD_SENSOR_1_PIN, INPUT);
  pinMode(CONTROL_LINE_3_PIN, OUTPUT);
  pinMode(CONTROL_LINE_4_PIN, OUTPUT);
  pinMode(CONTROL_LINE_5_PIN, OUTPUT);
  digitalWrite(CONTROL_LINE_3_PIN, HIGH);
  digitalWrite(CONTROL_LINE_4_PIN, HIGH);
  digitalWrite(CONTROL_LINE_5_PIN, HIGH);
}

void Blink(int msec, int times)
{
  digitalWrite(LED_PIN, LOW);
  for(int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(msec);
    digitalWrite(LED_PIN, LOW);
    delay(msec);
  }
}

void MakeSingleImpulse(int pin)
{
  digitalWrite(pin, LOW);
  delay(500);
  digitalWrite(pin, HIGH);
}

void loop() {
  if (digitalRead(GUARD_SENSOR_1_PIN) == LOW) {
      MakeSingleImpulse(CONTROL_LINE_3_PIN);
      MakeSingleImpulse(CONTROL_LINE_4_PIN);
      MakeSingleImpulse(CONTROL_LINE_5_PIN);
  }
}
































#define LED_PIN    13

#define SENSOR_1_PIN          10
#define SENSOR_2_PIN          11
#define SENSOR_3_PIN          12

#define CONTROL_LINE_1_PIN    2
#define CONTROL_LINE_2_PIN    3
#define CONTROL_LINE_3_PIN    4

void setup() {
  Serial.begin(9600);
  
  pinMode(SENSOR_1_PIN, INPUT);
  pinMode(SENSOR_2_PIN, INPUT);
  pinMode(SENSOR_3_PIN, INPUT);
  
  pinMode(CONTROL_LINE_1_PIN, OUTPUT);
  digitalWrite(CONTROL_LINE_1_PIN, HIGH);
  pinMode(CONTROL_LINE_2_PIN, OUTPUT);
  digitalWrite(CONTROL_LINE_2_PIN, HIGH);
  pinMode(CONTROL_LINE_3_PIN, OUTPUT);
  digitalWrite(CONTROL_LINE_3_PIN, HIGH);
}

void MakeSingleImpulse(int pin)
{
  digitalWrite(pin, LOW);
  delay(1000);
  digitalWrite(pin, HIGH);
  //delay(1000);
}

void loop() {

  if (digitalRead(SENSOR_1_PIN) == HIGH) {
      MakeSingleImpulse(CONTROL_LINE_1_PIN);
  }  
  if (digitalRead(SENSOR_2_PIN) == HIGH) {
      MakeSingleImpulse(CONTROL_LINE_2_PIN);
  }
  if (digitalRead(SENSOR_3_PIN) == HIGH) {
      MakeSingleImpulse(CONTROL_LINE_3_PIN);
  }
  
}
































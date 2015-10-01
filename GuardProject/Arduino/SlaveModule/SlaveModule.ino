#define LED_PIN    13

#define SENSOR_1_PIN    2
//#define SENSOR_2_PIN    11
//#define SENSOR_3_PIN    12

#define TRIGER_1_PIN    12
//#define TRIGER_2_PIN    3
//#define TRIGER_3_PIN    4

void setup() {
  Serial.begin(9600);
  
  pinMode(SENSOR_1_PIN, INPUT);
  
  pinMode(TRIGER_1_PIN, OUTPUT);
  digitalWrite(TRIGER_1_PIN, HIGH);
}

void MakeSingleImpulse(int pin)
{
  digitalWrite(pin, LOW);
  delay(1000);
  digitalWrite(pin, HIGH);
  delay(1000);
}

void loop() {

  if (digitalRead(SENSOR_1_PIN) == HIGH) {
      MakeSingleImpulse(TRIGER_1_PIN);
  }
  /*
  if (digitalRead(SENSOR_2_PIN) == HIGH) {
      MakeSingleImpulse(TRIGER_2_PIN);
  }
  if (digitalRead(SENSOR_3_PIN) == HIGH) {
      MakeSingleImpulse(TRIGER_3_PIN);
  }
  */

}

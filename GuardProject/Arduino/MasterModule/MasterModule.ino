#define LED_PIN                 13

#define GUARD_SENSOR_1_PIN      12
#define KEY_1_PIN               11

#define GSM_LINE_1_PIN          10
#define GSM_LINE_2_PIN          9
#define GSM_LINE_3_PIN          8
#define GSM_LINE_4_PIN          7
#define GSM_LINE_5_PIN          6

#define CONTROL_LINE_1_PIN      2
#define CONTROL_LINE_2_PIN      3

void setup() {
  pinMode(GUARD_SENSOR_1_PIN, INPUT);
  pinMode(KEY_1_PIN, INPUT);
  pinMode(GSM_LINE_1_PIN, INPUT);
  pinMode(GSM_LINE_2_PIN, INPUT);
  pinMode(GSM_LINE_3_PIN, INPUT);
  pinMode(GSM_LINE_4_PIN, INPUT);
  pinMode(GSM_LINE_5_PIN, INPUT);
  pinMode(CONTROL_LINE_1_PIN, OUTPUT);
  pinMode(CONTROL_LINE_2_PIN, OUTPUT);
  digitalWrite(CONTROL_LINE_1_PIN, HIGH);
  digitalWrite(CONTROL_LINE_2_PIN, HIGH);
}

void loop() {


}















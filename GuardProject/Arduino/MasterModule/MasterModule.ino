#include "Messenger.h"

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

Messenger messenger(&Serial);

void setup() {
  Serial.begin(9600);
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

void loop() {
  //Serial.println("blink");
  Message msg;
  messenger.SendMessage(msg);
  Blink(1000, 1);
  messenger.ReciveMessage(msg);
}















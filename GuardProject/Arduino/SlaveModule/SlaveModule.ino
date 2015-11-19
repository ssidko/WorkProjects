//
// SlaveModule
//

#include <MessageTypes.h>
#include <Messenger.h>

#define LED_PIN                       13

#define GUARD_SENSOR_1_PIN            12     // Датчик перемещения блока
#define LINK_TO_MASTER_OUT            9      // Сигнал связи с MasterModule.
#define LINK_TO_MASTER_IN             8      // Сигнал связи от MasterModule 

#define CONTROL_LINE_3_PIN            4       // Линия управления 3.
#define CONTROL_LINE_4_PIN            5       // Линия управления 4.
#define CONTROL_LINE_5_PIN            6       // Линия управления 5.

#define IMPULSE_DURATION  500

Messenger master;

volatile bool guard_enabled = false;

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

void setup() {
  Serial.begin(9600);
  master.Init(Serial);
  pinMode(LED_PIN, OUTPUT);
  pinMode(GUARD_SENSOR_1_PIN, INPUT);
  pinMode(CONTROL_LINE_3_PIN, OUTPUT);
  pinMode(CONTROL_LINE_4_PIN, OUTPUT);
  pinMode(CONTROL_LINE_5_PIN, OUTPUT);
  digitalWrite(CONTROL_LINE_3_PIN, HIGH);
  digitalWrite(CONTROL_LINE_4_PIN, HIGH);
  digitalWrite(CONTROL_LINE_5_PIN, HIGH);
  pinMode(LINK_TO_MASTER_IN, INPUT);
  pinMode(LINK_TO_MASTER_OUT, OUTPUT);
  digitalWrite(LINK_TO_MASTER_OUT, HIGH);
  Blink(200, 5);
}

void MakeLowImpulse(int pin, int duration)
{
  digitalWrite(pin, LOW);
  delay(duration);
  digitalWrite(pin, HIGH);
}

void WaitForEndLowImpulse(int pin)
{
  while (digitalRead(pin) == LOW);
}

void ActivateAllLocalLines(void)
{
  digitalWrite(CONTROL_LINE_3_PIN, LOW);
  digitalWrite(CONTROL_LINE_4_PIN, LOW);
  digitalWrite(CONTROL_LINE_5_PIN, LOW);
  delay(IMPULSE_DURATION);
  digitalWrite(CONTROL_LINE_3_PIN, HIGH);
  digitalWrite(CONTROL_LINE_4_PIN, HIGH);
  digitalWrite(CONTROL_LINE_5_PIN, HIGH);
}

void CheckGuardSensor1(void)
{
  Message msg;
  if (digitalRead(GUARD_SENSOR_1_PIN) == LOW) {
    ActivateAllLocalLines();
    msg.type = MessageType::kNotification;
    msg.code = NotificationType::kSlaveGuardSensor1Activated;
    master.SendMessage(msg);
    WaitForEndLowImpulse(GUARD_SENSOR_1_PIN);
  }
}

void loop() {
  Message msg;
  if (master.ReciveMessage(msg))

  if (guard_enabled) {
    digitalWrite(LED_PIN, HIGH);
    CheckGuardSensor1();   
  } else {
    digitalWrite(LED_PIN, LOW);
  }  
}
































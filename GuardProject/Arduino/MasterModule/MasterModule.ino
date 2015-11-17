//
// MasterModule
//

#include "Messenger.h"

#define LED_PIN                      13

#define GSM_LINE_1_PIN               A0     // Инициация удаления по каналу-1.
#define GSM_LINE_2_PIN               A1     // Инициация удаления по каналу-2.
#define GSM_LINE_3_PIN               A2     // Инициация удаления по каналу-3.
#define GSM_LINE_4_PIN               A3     // Инициация удаления по каналу-4.
#define GSM_LINE_5_PIN               A4     // Инициация удаления по каналу-5.
#define GSM_ERASE_ALL_PIN            A5     // Инициация удаления по всем каналам.

#define GUARD_ENABLED_PIN            12     // Активация режима охраны. 
#define GUARD_SENSOR_1_PIN           11     // Датчик перемещения корпуса блока. Инициация удаления по всем каналам.
#define KEY_ERASE_ALL_PIN            10     // Ключь на блоке (Блок 1). Инициация удаления по всем каналам.
#define LINK_TO_SLAVE_MODULE         9      // Сигнал связи с SlaveModule.   
              
#define CONTROL_LINE_1_PIN           4      // Линия управления 1.
#define CONTROL_LINE_2_PIN           5      // Линия управления 2.

Messenger host;
Messenger slave;

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

void MakeLowImpulse(int pin)
{
  digitalWrite(pin, LOW);
  delay(500);
  digitalWrite(pin, HIGH);
}

void setup() {
  Serial.begin(9600);
  host.Init(Serial);
  Serial1.begin(9600);
  slave.Init(Serial1);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(GUARD_ENABLED_PIN, INPUT);
  pinMode(GUARD_SENSOR_1_PIN, INPUT);
  pinMode(KEY_ERASE_ALL_PIN, INPUT);
  pinMode(GSM_LINE_1_PIN, INPUT);
  pinMode(GSM_LINE_2_PIN, INPUT);
  pinMode(GSM_LINE_3_PIN, INPUT);
  pinMode(GSM_LINE_4_PIN, INPUT);
  pinMode(GSM_LINE_5_PIN, INPUT);
  
  pinMode(CONTROL_LINE_1_PIN, OUTPUT);
  pinMode(CONTROL_LINE_2_PIN, OUTPUT);
  digitalWrite(CONTROL_LINE_1_PIN, HIGH);
  digitalWrite(CONTROL_LINE_2_PIN, HIGH);

  Blink(200, 5);
}

bool IsGuardEnabled(void)
{
  if (digitalRead(GUARD_ENABLED_PIN) == LOW) {
    return true;  
  } else {
    return false;
  }   
}

void CheckGsmLines(void)
{
  if (digitalRead(GSM_LINE_1_PIN) == LOW) {
    MakeLowImpulse(CONTROL_LINE_1_PIN);
  }
  if (digitalRead(GSM_LINE_2_PIN) == LOW) {
    MakeLowImpulse(CONTROL_LINE_2_PIN);
  }
  if (digitalRead(GSM_LINE_3_PIN) == LOW) {
    MakeLowImpulse(CONTROL_LINE_1_PIN);
  }
  if (digitalRead(GSM_LINE_4_PIN) == LOW) {
    MakeLowImpulse(CONTROL_LINE_2_PIN);
  }
  if (digitalRead(GSM_LINE_5_PIN) == LOW) {
    MakeLowImpulse(CONTROL_LINE_1_PIN);
  }
}

void loop() {
  Message msg;
  host.SendMessage(msg);
 
  Blink(100, 2);

  CheckGsmLines();

  host.ReciveMessage(msg);
}















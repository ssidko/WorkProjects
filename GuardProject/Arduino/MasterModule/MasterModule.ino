//
// MasterModule
//

#include "Messenger.h"
#include "MessageTypes.h"

#define LED_PIN                      13

#define GSM_LINE_1_PIN               A0     // Инициация удаления по каналу-1.
#define GSM_LINE_2_PIN               A1     // Инициация удаления по каналу-2.
#define GSM_LINE_3_PIN               A2     // Инициация удаления по каналу-3.
#define GSM_LINE_4_PIN               A3     // Инициация удаления по каналу-4.
#define GSM_LINE_5_PIN               A4     // Инициация удаления по каналу-5.
#define GSM_ERASE_ALL_PIN            A5     // Инициация удаления по всем каналам.
#define GUARD_MODE_ENABLE_PIN        12     // Активация режима охраны. HIGH - активен.
#define GUARD_SENSOR_1_PIN           11     // Датчик перемещения корпуса блока. Инициация удаления по всем каналам.
#define KEY_ERASE_ALL_PIN            10     // Ключь на блоке (Блок 1). Инициация удаления по всем каналам.
#define LINK_TO_SLAVE_OUT            9      // Сигнал связи с SlaveModule.
#define LINK_TO_SLAVE_IN             8      // Сигнал связи от SlaveModule. 
              
#define CONTROL_LINE_1_PIN           4      // Линия управления 1.
#define CONTROL_LINE_2_PIN           5      // Линия управления 2.

#define IMPULSE_DURATION             500

Messenger host;
Messenger slave;

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

void setup() {
  Serial.begin(9600);
  host.Init(Serial);
  Serial1.begin(9600);
  slave.Init(Serial1);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(GUARD_MODE_ENABLE_PIN, INPUT);
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
  pinMode(LINK_TO_SLAVE_IN, INPUT);
  pinMode(LINK_TO_SLAVE_OUT, OUTPUT);
  digitalWrite(LINK_TO_SLAVE_OUT, HIGH);
  Blink(200, 5);
}

bool IsGuardEnabled(void)
{
  return (digitalRead(GUARD_MODE_ENABLE_PIN) == HIGH);
}

void ActivateAllLocalLines(void)
{
  digitalWrite(CONTROL_LINE_1_PIN, LOW);
  digitalWrite(CONTROL_LINE_2_PIN, LOW);
  delay(IMPULSE_DURATION);
  digitalWrite(CONTROL_LINE_1_PIN, HIGH);
  digitalWrite(CONTROL_LINE_2_PIN, HIGH);
}

void ActivateAllLines(void)
{
  digitalWrite(CONTROL_LINE_1_PIN, LOW);
  digitalWrite(CONTROL_LINE_2_PIN, LOW);
  
  Message msg;    
  msg.type = MessageType::kCommand;  
  msg.code = CommandType::kActivateAllLines;
  slave.SendMessage(msg);

  delay(IMPULSE_DURATION);

  digitalWrite(CONTROL_LINE_1_PIN, HIGH);
  digitalWrite(CONTROL_LINE_2_PIN, HIGH);

  msg.type = MessageType::kNotification;  
  msg.code = NotificationType::kAllLinesActivated;
  slave.SendMessage(msg);
}

void CheckGsmLines(void)
{
  Message msg;
  if (digitalRead(GSM_LINE_1_PIN) == LOW) {
    MakeLowImpulse(CONTROL_LINE_1_PIN, IMPULSE_DURATION);
    msg.type = MessageType::kNotification;
    msg.code = NotificationType::kLine1Activated;
    host.SendMessage(msg);
    WaitForEndLowImpulse(GSM_LINE_1_PIN);
  }
  if (digitalRead(GSM_LINE_2_PIN) == LOW) {
    MakeLowImpulse(CONTROL_LINE_2_PIN, IMPULSE_DURATION);
    msg.type = MessageType::kNotification;
    msg.code = NotificationType::kLine2Activated;
    host.SendMessage(msg);
    WaitForEndLowImpulse(GSM_LINE_2_PIN);
  }
  if (digitalRead(GSM_LINE_3_PIN) == LOW) {
    msg.type = MessageType::kCommand;
    msg.code = CommandType::kActivateLine3;
    slave.SendMessage(msg);
    msg.type = MessageType::kNotification;
    msg.code = NotificationType::kLine3Activated;
    host.SendMessage(msg);
    WaitForEndLowImpulse(GSM_LINE_3_PIN);
  }
  if (digitalRead(GSM_LINE_4_PIN) == LOW) {
    msg.type = MessageType::kCommand;
    msg.code = CommandType::kActivateLine4;
    slave.SendMessage(msg);
    msg.type = MessageType::kNotification;
    msg.code = NotificationType::kLine4Activated;
    host.SendMessage(msg);
    WaitForEndLowImpulse(GSM_LINE_4_PIN);
  }
  if (digitalRead(GSM_LINE_5_PIN) == LOW) {    
    msg.type = MessageType::kCommand;
    msg.code = CommandType::kActivateLine5;
    slave.SendMessage(msg);
    msg.type = MessageType::kNotification;
    msg.code = NotificationType::kLine5Activated;
    host.SendMessage(msg);
    WaitForEndLowImpulse(GSM_LINE_5_PIN);
  }
}

void CheckGuardSensor1(void)
{
  Message msg;
  if (digitalRead(GUARD_SENSOR_1_PIN) == LOW) {
    ActivateAllLines();
    msg.type = MessageType::kNotification;
    msg.code = NotificationType::kMasterGuardSensor1Activated;
    host.SendMessage(msg);
    WaitForEndLowImpulse(GUARD_SENSOR_1_PIN);
  }
}

void CheckGsmEraseAll(void)
{
  Message msg;
  if (digitalRead(GSM_ERASE_ALL_PIN) == LOW) {
    ActivateAllLines();
    msg.type = MessageType::kNotification;
    msg.code = NotificationType::kGsmEraseAllActivated;
    host.SendMessage(msg);
    WaitForEndLowImpulse(GSM_ERASE_ALL_PIN);
  }
}

void CheckKeyEraseAll(void)
{
  Message msg;
  if (digitalRead(KEY_ERASE_ALL_PIN) == LOW) {
    ActivateAllLines();
    msg.type = MessageType::kNotification;
    msg.code = NotificationType::kKeyEraseAllActivated;
    host.SendMessage(msg);
    WaitForEndLowImpulse(KEY_ERASE_ALL_PIN);
  }
}

void CheckLinkToSlave()
{
  Message msg;
  if (digitalRead(LINK_TO_SLAVE_IN) == LOW) {
    ActivateAllLines();
    msg.type = MessageType::kNotification;
    msg.code = NotificationType::kLinkToSlaveLosted;
    host.SendMessage(msg);
    WaitForEndLowImpulse(LINK_TO_SLAVE_IN);
  }
}

void loop()
{
  Message msg;  
  if (host.ReciveMessage(msg)) {
    if (msg.type == MessageType::kNotification) {
      msg.code = IdentificationType::kMasterID;
      host.SendMessage(msg);
    }  
  }
  if (slave.ReciveMessage(msg)) {
    if (msg.type == MessageType::kNotification) {
      if (msg.code == NotificationType::kSlaveGuardSensor1Activated) {
        host.SendMessage(msg);
        ActivateAllLines();
        msg.code = NotificationType::kAllLinesActivated;
        host.SendMessage(msg);
      }
    }  
  }
  
  if (IsGuardEnabled()) {
    digitalWrite(LED_PIN, HIGH);
    CheckGsmEraseAll();
    CheckGsmLines();
    CheckKeyEraseAll();
    CheckGuardSensor1();
    CheckLinkToSlave();   
  } else {
    digitalWrite(LED_PIN, LOW); 
  }
}















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
volatile bool line_1_activated = false;
volatile bool line_2_activated = false;
volatile bool line_3_activated = false;
volatile bool line_4_activated = false;
volatile bool line_5_activated = false;

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

void ActivateAllLines(void)
{
  if (!line_1_activated) {
    digitalWrite(CONTROL_LINE_1_PIN, LOW);
    line_1_activated = true;
  }
  if (!line_2_activated) {
    digitalWrite(CONTROL_LINE_2_PIN, LOW);
    line_2_activated = true;
  }
  // НЕ ТРОГАТЬ!!!!! Так все и должно быть.
  Message msg;    
  msg.type = MessageType::kCommand;  
  msg.code = CommandType::kActivateAllLines;
  slave.SendMessage(msg);
  
  delay(IMPULSE_DURATION);

  digitalWrite(CONTROL_LINE_1_PIN, HIGH);
  digitalWrite(CONTROL_LINE_2_PIN, HIGH);
}

void InitGuardMode(void)
{
  line_1_activated = false;
  line_2_activated = false;
  line_3_activated = false;
  line_4_activated = false;
  line_5_activated = false;
}

bool IsGuardEnabled(void)
{
  return (digitalRead(GUARD_MODE_ENABLE_PIN) == HIGH);
}

void CheckGuardEnabled(void)
{
  Message msg(MessageType::kCommand, 0);
  Message notification(MessageType::kNotification, 0);
  msg.type = MessageType::kCommand;
  if (guard_enabled != IsGuardEnabled()) {
    if (guard_enabled) {
      msg.code = CommandType::kDisableGuardMode;
      notification.code = NotificationType::kGuardModeDisabled;
      guard_enabled = false;
      InitGuardMode();
    } else {
      msg.code = CommandType::kEnableGuardMode;
      notification.code = NotificationType::kGuardModeEnabled;
      guard_enabled = true;
      InitGuardMode();
    }
    slave.SendMessage(msg);
    host.SendMessage(notification);
    delay(1);
  }
}

void CheckGsmLines(void)
{
  Message msg;
  if (digitalRead(GSM_LINE_1_PIN) == LOW) {
    if (!line_1_activated) {
      MakeLowImpulse(CONTROL_LINE_1_PIN, IMPULSE_DURATION);
      msg.type = MessageType::kNotification;
      msg.code = NotificationType::kLine1Activated;
      host.SendMessage(msg);
      WaitForEndLowImpulse(GSM_LINE_1_PIN);
      line_1_activated = true;
    }
  }
  if (digitalRead(GSM_LINE_2_PIN) == LOW) {
    if (!line_2_activated) {
      MakeLowImpulse(CONTROL_LINE_2_PIN, IMPULSE_DURATION);
      msg.type = MessageType::kNotification;
      msg.code = NotificationType::kLine2Activated;
      host.SendMessage(msg);
      WaitForEndLowImpulse(GSM_LINE_2_PIN);
      line_2_activated = true;
    }
  }
  if (digitalRead(GSM_LINE_3_PIN) == LOW) {
    if (!line_3_activated) {
      msg.type = MessageType::kCommand;
      msg.code = CommandType::kActivateLine3;
      slave.SendMessage(msg);
      msg.type = MessageType::kNotification;
      msg.code = NotificationType::kLine3Activated;
      host.SendMessage(msg);
      WaitForEndLowImpulse(GSM_LINE_3_PIN);
      line_3_activated = true;
    }
  }
  if (digitalRead(GSM_LINE_4_PIN) == LOW) {
    if (!line_4_activated) {
      msg.type = MessageType::kCommand;
      msg.code = CommandType::kActivateLine4;
      slave.SendMessage(msg);
      msg.type = MessageType::kNotification;
      msg.code = NotificationType::kLine4Activated;
      host.SendMessage(msg);
      WaitForEndLowImpulse(GSM_LINE_4_PIN);
      line_4_activated = true;
    }
  }
  if (digitalRead(GSM_LINE_5_PIN) == LOW) {
    if (!line_5_activated) {    
      msg.type = MessageType::kCommand;
      msg.code = CommandType::kActivateLine5;
      slave.SendMessage(msg);
      msg.type = MessageType::kNotification;
      msg.code = NotificationType::kLine5Activated;
      host.SendMessage(msg);
      WaitForEndLowImpulse(GSM_LINE_5_PIN);
      line_5_activated = true;
    }
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
  }
}

void PrintMessage(Message &msg)
{
    Serial.println("---==== Message info ====---");
    Serial.print("header: "); Serial.println(msg.header);
    Serial.print("type: "); Serial.println(msg.type);
    Serial.print("code: "); Serial.println(msg.code);
    Serial.print("footer: "); Serial.println(msg.footer);
    Serial.print("valid: "); Serial.println(msg.IsValidMessage());
    Serial.println("=============================");
}

void loop()
{
  Message msg;  
  if (host.ReciveMessage(msg)) {
    if (msg.type == MessageType::kIdentification) {
      msg.code = IdentificationType::kMasterID;
      host.SendMessage(msg);
    }  
  }
  if (slave.ReciveMessage(msg)) {
    PrintMessage(msg);
    if (msg.type == MessageType::kNotification) {
      switch (msg.code) {
        case NotificationType::kSlaveGuardSensor1Activated:
          host.SendMessage(msg);
          ActivateAllLines();
          msg.code = NotificationType::kAllLinesActivated;
          host.SendMessage(msg);
          break;
        case NotificationType::kLinkToMasterLosted:
          host.SendMessage(msg);
          ActivateAllLines();
          msg.code = NotificationType::kAllLinesActivated;
          host.SendMessage(msg);
          break;
        default:
          Blink(100,20);
          break;        
      }
    }  
  }

  CheckGuardEnabled();  
  
  if (guard_enabled) {
    digitalWrite(LED_PIN, HIGH);
    CheckGsmEraseAll();
    CheckGsmLines();
    CheckKeyEraseAll();
    CheckGuardSensor1();
    CheckLinkToSlave();   
  } else {
    digitalWrite(LED_PIN, LOW); 
  }

/*
//========================================================================
  Message msg1;
  msg1.Clear();
  if (sizeof(Message) == Serial1.readBytes((char *)&msg1, sizeof(Message))) {
    PrintMessage(msg1);
  }
//========================================================================
*/
}















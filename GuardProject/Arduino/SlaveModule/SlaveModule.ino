//
// SlaveModule
//

#include <MessageTypes.h>
#include <Messenger.h>

#define LED_PIN                       13

#define GUARD_SENSOR_1_PIN            12      // Датчик перемещения блока
#define LINK_TO_MASTER_OUT            9       // Сигнал связи с MasterModule.
#define LINK_TO_MASTER_IN             8       // Сигнал связи от MasterModule 

#define CONTROL_LINE_3_PIN            4       // Линия управления 3.
#define CONTROL_LINE_4_PIN            5       // Линия управления 4.
#define CONTROL_LINE_5_PIN            6       // Линия управления 5.

#define OUT_IMPULSE_DURATION          500
#define IN_IMPULSE_DURATION           50      // Минимальная длительность входных импусьсов.
                                              // Импульсы меньшей длительности не воспринимаются.

Messenger master;

volatile bool guard_enabled = false;
volatile bool line_3_activated = false;
volatile bool line_4_activated = false;
volatile bool line_5_activated = false;
volatile bool guard_sensor_1_activated = false;
volatile bool link_to_master_losted = false;

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

void setup() {
  Serial1.begin(9600);
  master.Init(Serial1);
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

void InitGuardMode(void)
{
  line_3_activated = false;
  line_4_activated = false;
  line_5_activated = false;
  guard_sensor_1_activated = false;
  link_to_master_losted = false;
}

void ActivateAllLocalLines(void)
{
  if (!line_3_activated) {
    digitalWrite(CONTROL_LINE_3_PIN, LOW);  
  }
  if (!line_4_activated) {
    digitalWrite(CONTROL_LINE_4_PIN, LOW);
  }
  if (!line_5_activated) {
    digitalWrite(CONTROL_LINE_5_PIN, LOW);
  }
  delay(OUT_IMPULSE_DURATION);
  digitalWrite(CONTROL_LINE_3_PIN, HIGH);
  digitalWrite(CONTROL_LINE_4_PIN, HIGH);
  digitalWrite(CONTROL_LINE_5_PIN, HIGH);

  line_3_activated = true;
  line_4_activated = true;
  line_5_activated = true;
}

inline bool GuardSensor1Activated(void)
{
  if (digitalRead(GUARD_SENSOR_1_PIN) == HIGH) {
    return true;    
  }
  return false;  
}

void CheckGuardSensor1(void)
{
  Message msg;
  if (GuardSensor1Activated()) {
    if (guard_sensor_1_activated == false) {
      guard_sensor_1_activated = true;
      msg.type = MessageType::kNotification;
      msg.code = NotificationType::kSlaveGuardSensor1Activated;
      master.SendMessage(msg);
      ActivateAllLocalLines();      
    }
  }
}

inline bool LinkToMasterLosted(void)
{
  return (digitalRead(LINK_TO_MASTER_IN) == LOW);
}

void CheckLinkToMaster()
{
  Message msg;
  if (LinkToMasterLosted()) {
    if (link_to_master_losted == false) {
      link_to_master_losted = true;
      msg.type = MessageType::kNotification;
      msg.code = NotificationType::kLinkToMasterLosted;
      master.SendMessage(msg);
      ActivateAllLocalLines();      
    }
  }
}

void MasterMessageHandling(void)
{
  Message msg;
  if (master.ReciveMessage(msg)) {

    switch (msg.type) {      
      case MessageType::kIdentification:
        msg.code = IdentificationType::kSlaveID;
        master.SendMessage(msg);
        break;        
      case MessageType::kCommand :      
        switch (msg.code) {
          case CommandType::kActivateLine3:
            if (!line_3_activated) {
              MakeLowImpulse(CONTROL_LINE_3_PIN, OUT_IMPULSE_DURATION);
              line_3_activated = true;
            }
            break; 
          case CommandType::kActivateLine4:
            if (!line_4_activated) {
              MakeLowImpulse(CONTROL_LINE_4_PIN, OUT_IMPULSE_DURATION);
              line_4_activated = true;
            }
            break;
          case CommandType::kActivateLine5:
            if (!line_5_activated) {
              MakeLowImpulse(CONTROL_LINE_5_PIN, OUT_IMPULSE_DURATION);
              line_5_activated = true;
            }
            break;
          case CommandType::kActivateAllLines:
            ActivateAllLocalLines();
            break;
          case CommandType::kEnableGuardMode:
            InitGuardMode();
            guard_enabled = true;  
            break;
          case CommandType::kDisableGuardMode:
            InitGuardMode();
            guard_enabled = false;  
            break;            
          default:
            break;
        }
      default:
        break;  
    }
  }    
}

void loop()
{
  MasterMessageHandling();
  /*
  guard_enabled = true;
  line_3_activated = false;
  line_4_activated = false;
  line_5_activated = false;
  */  
  if (guard_enabled) {
    digitalWrite(LED_PIN, HIGH);
    CheckGuardSensor1();
    CheckLinkToMaster();  
  } else {
    digitalWrite(LED_PIN, LOW);
  }

/*
//========================================================================
  Message msg;
  msg.type = MessageType::kNotification;
  msg.code = NotificationType::kSlaveGuardSensor1Activated;
  master.SendMessage(msg);
  delay(1000);
//========================================================================
*/
}
































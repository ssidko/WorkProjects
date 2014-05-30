#define MAX_PIN_NUMBER          12
#define LED_PIN                 13
#define IMPULSE_WIDTH           50

#define ID_CMD		        "GetID"
#define VERSION_CMD		"GetVer"

#define ID                      "00014001"
#define VERSION                 "00000001"

String command = "";
boolean cmd_complete = false;

void setup ()
{
  Serial.begin(9600);
  command.reserve(16);
  pinMode(LED_PIN, OUTPUT);
  for (int i = 2; i <= MAX_PIN_NUMBER; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
}

void Blink()
{
  digitalWrite(LED_PIN, LOW);
  for (int x = 0; x < 3; x++) {
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(50);
  }
  digitalWrite(LED_PIN, LOW);
}

void MakeImpulse(int pin)
{
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(pin, HIGH);
  delay(IMPULSE_WIDTH);
  digitalWrite(pin, LOW);
  digitalWrite(LED_PIN, LOW);
}

void loop ()
{
  if (cmd_complete) {
    Blink();
    command.trim();
    if (command == "Pin2") {
      MakeImpulse(2);
    } else if (command == "Pin3") {
      MakeImpulse(3);
    } else if (command == "Pin4") {
      MakeImpulse(4);
    } else if (command == "Pin5") {
      MakeImpulse(5);
    } else if (command == "Pin6") {
      MakeImpulse(6);
    } else if (command == "Pin7") {
      MakeImpulse(7);
    } else if (command == "Pin8") {
      MakeImpulse(8);
    } else if (command == "Pin9") {
      MakeImpulse(9);
    } else if (command == "Pin10") {
      MakeImpulse(10);
    } else if (command == "Pin11") {
      MakeImpulse(11);
    } else if (command == "Pin12") {
      MakeImpulse(12);
    } if (command == "Hello") {
      Serial.write("Hi");
      Serial.flush();
    } else if (command == GET_ID_CMD) {
      Serial.write(ID);
      Serial.flush();
    } else if (command == GET_VERSION_CMD) {
      Serial.write(VERSION);
      Serial.flush();
    }
    command = "";
    cmd_complete = false;
  }
}

void serialEvent ()
{
  while(Serial.available()) {
    char ch = (char)Serial.read();
    command += ch;
    if (ch == '\n') {
      cmd_complete = true;
    }
  }
}

#include <SoftwareSerial.h>

#define BTXD_PIN 10
#define BRXD_PIN 11
#define SPARK_PLUG_PIN 7
#define LED_PIN 4

SoftwareSerial bluetooth(BRXD_PIN, BTXD_PIN);

String rawCommand = "";
boolean shouldStartSparkPlugTest = false;
boolean shouldStartMagnetoTest = false;

void setup() {
  // Serial.begin(9600);
  bluetooth.begin(9600);

  pinMode(SPARK_PLUG_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  delay(100);

  receiveBtData();
  startSparkPlugTest();
}

void receiveBtData() {
  if (!bluetooth.available()) return;

  char c = bluetooth.read();
  
  if (c != '\n') {
    rawCommand += c;
    return;
  } else {
    // Serial.println(rawCommand);
    runCommand();
    rawCommand = "";
  }
}

void runCommand() {
  String command = rawCommand.substring(2);
  char target = rawCommand.charAt(0);

  switch (target) {
    case 'L':
      // LED test
      if (command == "ON") {
        digitalWrite(LED_PIN, HIGH);
      }

      if (command == "OFF") {
        digitalWrite(LED_PIN, LOW);
      }
      break;
    case 'S':
      // Spark plug test
      if (command == "START") {
        shouldStartSparkPlugTest = true;
      }

      if (command == "STOP") {
        shouldStartSparkPlugTest = false;
      }
      break;
    case 'M':
      // Magneto test
      if (command == "START") {
        shouldStartMagnetoTest = true;
      }

      if (command == "STOP") {
        shouldStartMagnetoTest = false;
      }
      break;
    default:
      break;
  }
}

void sendBtData(String data) {
  if (!bluetooth.available()) return;

  // this sends string data as bytes
  bluetooth.write(data.c_str());
}

void startSparkPlugTest() {
  if (!shouldStartSparkPlugTest) return;

  // read from the connected spark plug pin
  int inputState = digitalRead(SPARK_PLUG_PIN);

  if (inputState == LOW) {
    // Serial.println("PASS");
    sendBtData("PASS");
    digitalWrite(LED_PIN, HIGH);
  } else {
    // Serial.println("FAIL");
    sendBtData("FAIL");
    digitalWrite(LED_PIN, LOW);
  }

  delay(2000);
}

void startMagnetoTest() {
  if (!shouldStartMagnetoTest) return;
  
  // TODO add magneto test code here
}

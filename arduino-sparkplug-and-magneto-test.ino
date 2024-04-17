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
  receiveBtData();
  startSparkPlugTest();

  delay(100);
}

void startSparkPlugTest() {
  if (!shouldStartSparkPlugTest) return;

  // read from the connected spark plug pin
  int inputState = digitalRead(SPARK_PLUG_PIN);

  if (inputState == LOW) {
    String data = "P";
    digitalWrite(LED_PIN, HIGH);
    sendBtData(data);
    // Serial.println(data);
  } else {
    String data = "F";
    digitalWrite(LED_PIN, LOW);
    sendBtData(data);
    // Serial.println(data);
  }

  delay(2000);
}

void startMagnetoTest() {
  if (!shouldStartMagnetoTest) return;
  
  // TODO add magneto test code here
}

//////////////////////
// HELPER FUNCTIONS //
//////////////////////

void sendBtData(String data) {
  bluetooth.write(data.c_str());
}

void receiveBtData() {
  if (!bluetooth.available()) return;

  char c = bluetooth.read();
  
  if (c != '\n') {
    rawCommand += c;
    return;
  } else {
    // Serial.println(rawCommand);

    String command = rawCommand.substring(2);
    char module = rawCommand.charAt(0);
    runCommand(command, module);

    // clear
    rawCommand = "";
  }
}

void runCommand(String command, char module) {
  switch (module) {
    case 'S':
      // Spark plug test
      if (command == "START") {
        shouldStartSparkPlugTest = true;
      }

      if (command == "STOP") {
        shouldStartSparkPlugTest = false;
        digitalWrite(LED_PIN, LOW);
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
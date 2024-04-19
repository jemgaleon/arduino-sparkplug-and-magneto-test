#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

#define BTXD_PIN 12 // rxd in the board
#define BRXD_PIN 13 // txd in the board
#define SPARK_PLUG_PIN 7
int IR_PIN = 3;

SoftwareSerial bluetooth(BRXD_PIN, BTXD_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

String rawCommand = "";
boolean shouldStartSparkPlugTest = false;
boolean shouldStartMagnetoTest = false;

// RPM
float REV = 0;
int RPM_VALUE;
int PREVIOUS = 0;
int TIME;

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);

  pinMode(SPARK_PLUG_PIN, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  attachInterrupt(digitalPinToInterrupt(IR_PIN), interrupt, RISING);
}

void loop() {
  receiveBtData();
  startSparkPlugTest();
  startMagnetoTest();

  delay(100);
}

void startSparkPlugTest() {
  if (!shouldStartSparkPlugTest) return;

  // read from the connected spark plug pin
  int inputState = digitalRead(SPARK_PLUG_PIN);

  if (inputState == LOW) {
    String data = "P";
    sendBtData(data);

    lcd.setCursor(0,1);
    lcd.print("PASS");
  } else {
    String data = "F";
    sendBtData(data);

    lcd.setCursor(0,1);
    lcd.print("FAIL");
  }

  delay(2000);
}

void startMagnetoTest() {
  if (!shouldStartMagnetoTest) return;

  detachInterrupt(digitalPinToInterrupt(IR_PIN));
  TIME = millis() - PREVIOUS;
  RPM_VALUE = (REV/TIME) * 60000;
  PREVIOUS = millis();
  REV = 0;
  
  lcd.setCursor(0,1);
  lcd.print("RPM");
  lcd.setCursor(5,1);
  lcd.print(RPM_VALUE);
  lcd.print("     ");

  sendBtData(String(RPM_VALUE));

  attachInterrupt(digitalPinToInterrupt(IR_PIN), interrupt, RISING);

  delay(1000);
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
    case 'S': // Spark plug 

      if (command == "START") {
        shouldStartSparkPlugTest = true;

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("  Spark Plug");
      }

      if (command == "STOP") {
        shouldStartSparkPlugTest = false;

        lcd.clear();
      }
      break;
    case 'M': // Magneto
  
      if (command == "START") {
        shouldStartMagnetoTest = true;

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("  Magneto");
      }

      if (command == "STOP") {
        shouldStartMagnetoTest = false;

        lcd.clear();
      }
      break;
    default:
      break;
  }
}

void interrupt() {
  REV++;
}


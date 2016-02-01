#include "Adafruit_GFX.h"
#include "Adafruit_LEDBackpack.h"

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();

int led = D7;
int switchPinBrightness = D5;
int switchPinToggle = D4;
unsigned int ledState = 0;
int currentBrightness = 0; // max 15, min 0
bool brighter = false;
int currentDisplayState = 1;
int maxDisplayState = 2;
bool togglingDisplayState = false;
int sleepInterval = 30000;
int lastAction;
int messageLoops = 3;
int previousDisplayState;
bool noSleep = false;

String currentTemperature;
String lastTime;
String currentTime;
String currentMessage;

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

void setup() {
  Serial.begin(9600);
  Serial.println("Clockwork started...");

  Time.zone(11);
	Particle.syncTime();
  lastSync = millis();

  Particle.variable("CurrentTime", Time.timeStr());
  Particle.function("currentTemp", setTemperature);
  Particle.function("currentMsg", setMessage);

  RGB.control(true);
  pinMode(led, OUTPUT);
  //pinMode(switchPinBrightness, INPUT);
  pinMode(switchPinBrightness, INPUT);
  pinMode(switchPinToggle, INPUT_PULLUP);

  alpha4.begin(0x70);  // pass in the address

  alpha4.clear();
  alpha4.writeDisplay();
  delay(250);
  alpha4.writeDigitAscii(0, 'C');
  alpha4.writeDigitAscii(1, 'L');
  alpha4.writeDigitAscii(2, 'C');
  alpha4.writeDigitAscii(3, 'K');
  alpha4.writeDisplay();
  delay(250);
  alpha4.writeDigitAscii(0, 'W');
  alpha4.writeDigitAscii(1, 'O');
  alpha4.writeDigitAscii(2, 'R');
  alpha4.writeDigitAscii(3, 'K');
  alpha4.writeDisplay();
  delay(500);
  alpha4.clear();
  alpha4.writeDisplay();
  alpha4.setBrightness(currentBrightness);
}

void loop() {
  RGB.color(0,0,0);

  if (millis() - lastSync > ONE_DAY_MILLIS) {
    Particle.syncTime();
    lastSync = millis();
  }

  if (digitalRead(switchPinToggle) == LOW) {
    RGB.color(255,0,0);
    if (! togglingDisplayState) {
      togglingDisplayState = true;
      if (currentDisplayState < maxDisplayState) {
        currentDisplayState++;
      } else {
        currentDisplayState = 0;
      }
      char cw_state[8];
      sprintf(cw_state, "%d", currentDisplayState);
      Particle.publish("clockwork_state", cw_state);
    }
    lastAction = millis();
  } else {
    togglingDisplayState = false;
  }

  if (digitalRead(switchPinBrightness) == HIGH) {
    RGB.color(0,255,0);
    if (! brighter) {
      brighter = true;
      if (currentBrightness >= 14) {
        currentBrightness = 0;
      } else {
        currentBrightness = currentBrightness + 5;
      }
      alpha4.setBrightness(currentBrightness);
      if (currentBrightness > 0 && currentDisplayState == 0) {
        currentDisplayState = 1;
      }
      char cw_brightness[8];
      sprintf(cw_brightness, "%d", currentBrightness);
      Particle.publish("clockwork_brightness", cw_brightness);
    }
    lastAction = millis();
  } else {
    brighter = false;
  }

  if (millis() > (lastAction + sleepInterval) && ! noSleep) {
    currentDisplayState = 0;
  }

  switch(currentDisplayState) {
    case 1:
      displayTime();
      break;
    case 2:
      displayTemperature();
      break;
    case 99:
      displayMessage();
      break;
    default:
      displayBlank();
      break;
  }
}

void displayTime() {
  currentTime = Time.timeStr();

  if (lastTime != currentTime) {
    Particle.variable("CurrentTime", currentTime);
    if ( ledState == 0 ) {
      ledState = 1;
      digitalWrite(led, HIGH);
    } else {
      ledState = 0;
      digitalWrite(led, LOW);
    }

    alpha4.writeDigitAscii(0, currentTime[11]);
    alpha4.writeDigitAscii(1, currentTime[12]);
    alpha4.writeDigitAscii(2, currentTime[14]);
    alpha4.writeDigitAscii(3, currentTime[15]);
    alpha4.writeDisplay();

    lastTime = currentTime;
  }
}

void displayBlank() {
  currentTime = Time.timeStr();

  if (lastTime != currentTime) {
    ledState = 0;
    digitalWrite(led, LOW);
    alpha4.writeDigitAscii(0, ' ');
    alpha4.writeDigitAscii(1, ' ');
    alpha4.writeDigitAscii(2, ' ');
    alpha4.writeDigitAscii(3, ' ');
    alpha4.writeDisplay();

    lastTime = currentTime;
  }
}

void displayTemperature() {
  currentTime = Time.timeStr();

  if (lastTime != currentTime) {
    ledState = 0;
    digitalWrite(led, LOW);
    alpha4.writeDigitAscii(0, currentTemperature[0]);
    alpha4.writeDigitAscii(1, currentTemperature[1]);
    alpha4.writeDigitAscii(2, currentTemperature[2]);
    alpha4.writeDigitAscii(3, currentTemperature[3]);
    alpha4.writeDisplay();

    lastTime = currentTime;
  }
}

int setTemperature(String currentTemp) {
  currentTemperature = currentTemp;
  Particle.publish("clockwork_temperature", currentTemperature);
}

int setMessage(String message) {
  currentMessage = message;
  previousDisplayState = currentDisplayState;
  currentDisplayState = 99;
  noSleep = true;
  Particle.publish("clockwork_message", currentMessage);
}

void displayMessage() {
  if (currentMessage == "") {
    currentDisplayState = (previousDisplayState ? previousDisplayState : 1);
    return;
  }

  currentTime = Time.timeStr();

  if (lastTime != currentTime) {
    ledState = 0;
    digitalWrite(led, LOW);

    String preparedMessage;
    preparedMessage = "    " + currentMessage + "    ";
    int l = strlen(preparedMessage);
    for (int j = 0; j < messageLoops; j++) {
      for (int i = 0; i < (l - 3); i++) {
        alpha4.writeDigitAscii(0, preparedMessage[i]);
        alpha4.writeDigitAscii(1, preparedMessage[i+1]);
        alpha4.writeDigitAscii(2, preparedMessage[i+2]);
        alpha4.writeDigitAscii(3, preparedMessage[i+3]);
        alpha4.writeDisplay();
        delay(200);
      }
    }

    currentMessage = "";
    noSleep = false;
    currentDisplayState = (previousDisplayState ? previousDisplayState : 1);
    lastTime = currentTime;
  }
}

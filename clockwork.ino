#include "Adafruit_GFX.h"
#include "Adafruit_LEDBackpack.h"

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();

int led = D7;
int switchPinBrightness = D5;
int switchPinToggle = D4;
unsigned int ledState = 0;
int brtns = 0; // max 15, min 0
bool brighter = false;
int currentDisplayState = 0;
int maxDisplayState = 2;
bool togglingDisplayState = false;

String currentTemperature;
String lastTime;
String currentTime;

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

void setup() {
  Serial.begin(9600);
  Serial.println("Clockwork started...");

  Time.zone(11);
	Particle.syncTime();
  lastSync = millis();

  Particle.variable("CurrentTime", Time.timeStr());
  Particle.function("currentTemp", setTemperature);

  RGB.control(true);
  pinMode(led, OUTPUT);
  //pinMode(switchPinBrightness, INPUT);
  pinMode(switchPinBrightness, INPUT_PULLUP);
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
  alpha4.setBrightness(brtns);
}

void loop() {
  if (millis() - lastSync > ONE_DAY_MILLIS) {
    Particle.syncTime();
    lastSync = millis();
  }

  if (digitalRead(switchPinBrightness) == HIGH) {
    RGB.color(255,0,0);
    if (! brighter) {
      if (brtns >= 14) {
        brtns = 0;
      } else {
        brtns = brtns + 5;
      }
      alpha4.setBrightness(brtns);
      brighter = true;
      char cw_brightness[8];
      sprintf(cw_brightness, "%d", brtns);
      Particle.publish("clockwork_brightness", cw_brightness);
    }
  } else {
    RGB.color(0,255,0);
    brighter = false;
  }

  if (digitalRead(switchPinToggle) == HIGH) {
    if (! togglingDisplayState) {
      togglingDisplayState = true;
      if (currentDisplayState < maxDisplayState) {
        currentDisplayState++;
      } else {
        currentDisplayState = 1;
      }
      char cw_state[8];
      sprintf(cw_state, "%d", currentDisplayState);
      Particle.publish("clockwork_state", cw_state);
    }
  } else {
    togglingDisplayState = false;
  }

  switch(currentDisplayState) {
    case 1 :
      displayTemperature();
    case 2 :
      displayTime();
    default :
      displayBlank();
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
    RGB.color(255,255,0);
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

#include "Adafruit_GFX.h"
#include "Adafruit_LEDBackpack.h"

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();

int led = D7;
unsigned int ledState = 0;
int brtns = 0; // max 15, min 0

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

  pinMode(led, OUTPUT);

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

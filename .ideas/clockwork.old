#include "Adafruit_GFX.h"
#include "Adafruit_LEDBackpack.h"

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();

int led = D7;
unsigned int ledState = 0;
unsigned int outputTimeRate = 10000;
unsigned int nextOutputTime;
char currentTime[128];

String lastTime;
String currentTime;

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

void setup() {
  Serial.begin(9600);
  Serial.println("Clockwork started.");

  Time.zone(11);
	Particle.syncTime();
  lastSync = millis();

  Particle.variable("CurrentTime", currentTime);

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
}

char displaybuffer[4] = {' ', ' ', ' ', ' '};

void loop() {
  if (millis() - lastSync > ONE_DAY_MILLIS) {
    Particle.syncTime();
    lastSync = millis();
  }

  currentTime = Time.timeStr();

  if (lastTime != currentTime) {
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

  //while (! Serial.available()) return;

  /*
  char c = Serial.read();
  if (! isprint(c)) return; // only printable!

  // scroll down display
  displaybuffer[0] = displaybuffer[1];
  displaybuffer[1] = displaybuffer[2];
  displaybuffer[2] = displaybuffer[3];
  displaybuffer[3] = c;

  // set every digit to the buffer
  alpha4.writeDigitAscii(0, displaybuffer[0]);
  alpha4.writeDigitAscii(1, displaybuffer[1]);
  alpha4.writeDigitAscii(2, displaybuffer[2]);
  alpha4.writeDigitAscii(3, displaybuffer[3]);

  // write it out!
  alpha4.writeDisplay();
  delay(200);
  */

  if ( lastTimeUpdate != currentTimeUpdate ) {
    if ( ledState == 0 ) {
      ledState = 1;
      digitalWrite(led, HIGH);
    } else {
      ledState = 0;
      digitalWrite(led, LOW);
    }

    String alphaTime;
    alphaTime = Time.timeStr(); // Wed May 21 01:08:47 2014
    Serial.println(alphaTime);

    Serial.println("Time:");
    Serial.println(alphaTime[11]);
    Serial.println(alphaTime[12]);
    Serial.println(alphaTime[14]);
    Serial.println(alphaTime[15]);
    Serial.println();

    //Serial.println(Time.format(Time.second() % 2 == 0 ? "%l:%M" : "%l %M"));
    Serial.println(Time.format("%p"));
    Serial.println(Time.format("%F"));
    /*
    time_t rawtime;
    struct tm *info;
    */
    char alphaHour[3];
    char alphaMinute[3];
    /*
    time(&rawtime);
    info = localtime(&rawtime);

    int Hour = info->tm_hour;
    int Min = info->tm_min;
    */
    sprintf(alphaHour, "%s", Time.hour());
    sprintf(alphaMinute, "%s", Time.minute());
    Serial.print(alphaHour == 0 ? "00" : alphaHour);
    Serial.print(':');
    Serial.print(alphaMinute);
    Serial.println();
    //strftime(alphaHour, 4, '%H', info);
    //strftime(alphaMinute, 4, '%M', info);

    alpha4.writeDigitAscii(0, alphaTime[11]);
    alpha4.writeDigitAscii(1, alphaTime[12]);
    alpha4.writeDigitAscii(2, alphaTime[14]);
    alpha4.writeDigitAscii(3, alphaTime[15]);
    alpha4.writeDisplay();

    /*
    char cTime[8];
    char cHour[4];
    char cMinute[4];
    sprintf(cHour, "%d", Time.hour());
    strcat(cTime, cHour);
    sprintf(cMinute, "%d", Time.minute());
    strcat(cTime, cMinute);
    sprintf(currentTime, "%s", cTime);
    */
    nextOutputTime = millis() + outputTimeRate;

    //Serial.println(currentTime);
  }
}

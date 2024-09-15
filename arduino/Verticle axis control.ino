#include <Servo.h>
#include <Wire.h>
#include <RTClib.h>
#include <SolarCalculator.h>
#include <TimeLib.h>

Servo servoV; // the vertical servo on top (move with el angle)
RTC_DS3231 rtc; // the rtc object to read time
const int buttonPin = 2; // button input connect to pin 2
int mode = 0; // 0 for RTC mode, 1 for LDR mode

// the latitude and longitude at Geisel library (La Jolla)
double latitude = 32.88143039433458;
double longitude = -117.23784695667904;
int utc_offset = -7; // Pacific time
int angle = 0;
int interval = 600; // update angle every 10 minutes (600 seconds)
const int threshold = 30; // Sensitivity threshold
const int stepSize = 10; // Step size for motor movement


void setup() {
  servoV.attach(3); // connect top servo to pin 3
  servoV.write(angle); // preset initial angle of top servo to 10 degree
  
  Wire.begin();
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set RTC to date and time this sketch was compiled
  //setTime(0, 0, 0, 1, 1, 2022);
}




void loop() {
  if (digitalRead(buttonPin) == LOW) {
    mode = !mode;
    delay(50); // button debounce delay
  }

  if (mode == 0) {
    Serial.println("RTC mode");
    trackSunWithRTC();
  } else {
    Serial.println("LDR mode");
    trackSunWithLDR();
  }
}

void trackSunWithRTC() {
  DateTime now = rtc.now(); // get current time
  static unsigned long next_millis = 0;

  // At every interval
  if (millis() > next_millis){
    time_t utc = toUtc(now.unixtime());
    double az, el;

    // Calculate the solar position, in degrees
    calcHorizontalCoordinates(utc, latitude, longitude, az, el);

    // adjust vertical motor with elevation angle (-90 to 90), but we only take the positive angle
    if (el >= 0) {
      // assumes motor go from flat, else change value to el + 90
      servoV.write(90 - el);
    } else {
      // assumes motor go from flat, else change the value to 180
      servoV.write(0);
    } 
    next_millis = millis() + interval * 1000L;
  }
  delay(1000);
}

void trackSunWithLDR() {
  
  int ldrTop = analogRead(A0); // connect top to A2
  int ldrBottom = analogRead(A1); // connect bottom to A3


  // Calculate differences in light intensity
  int verticalDifference = ldrBottom - ldrTop;
  Serial.print("Top: ");
  Serial.println(ldrTop);
  Serial.print("Bottom: ");
  Serial.println(ldrBottom);

  // Determine motor movements based on differences
  if (abs(verticalDifference) > threshold) {
    if (verticalDifference > 0) {
      servoV.write(servoV.read() + stepSize);
    } else {
      servoV.write(servoV.read() - stepSize);
    }
  }

  delay(100); // Small delay to allow for smooth movement
}

time_t toUtc(time_t local) {
  return local - utc_offset * 3600L;
}

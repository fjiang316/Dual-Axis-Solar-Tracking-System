#include <Servo.h>
#include <Wire.h>
#include <RTClib.h>
#include <SolarCalculator.h>
#include <TimeLib.h>

Servo servoH; // the horizontal servo on bottom (move with az angle)
RTC_DS3231 rtc; // the rtc object to read time
const int buttonPin = 2; // button input connect to pin 2
int mode = 0; // 0 for RTC mode, 1 for LDR mode

// the latitude and longitude at Geisel library (La Jolla)
double latitude = 32.88143039433458;
double longitude = -117.23784695667904;
int utc_offset = -7; // Pacific time
int angle = 10;
int interval = 600; // update angle every 10 minutes (600 seconds)
const int threshold = 30; // Sensitivity threshold
const int stepSize = 10; // Step size for motor movement


void setup() {
  servoH.attach(5); // connect bottom servo to pin 5
  servoH.write(angle); // preset initial angle of bottom servo to 10 degree
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
  if (millis() > next_millis)
  {
    time_t utc = toUtc(now.unixtime());
    double az, el;

    // Calculate the solar position, in degrees
    calcHorizontalCoordinates(utc, latitude, longitude, az, el);

    // adjust vertical motor with elevation angle (-90 to 90), but we only take the positive angle
    if (el >= 0) {
      // assumes motor's 0 angle is set to east (facing east initially)
      if (az < 180) {
        servoH.write(max(az - 90, 0));
      } else {
        servoH.write(min(az - 90, 180));
      }
    }
    else {
      servoH.write(0);
    }

    // adjust horizontal motor with azimuth angle (0 to 360), 
    next_millis = millis() + interval * 1000L;
  }

  delay(1000);
}

void trackSunWithLDR() {
  
  int ldrLeft = analogRead(A2); // connect bottomleft to A2
  int ldrRight = analogRead(A3); // connect bottomright to A3


  // Calculate differences in light intensity
  int horizontalDifference = ldrRight - ldrLeft;
  Serial.print("Left: ");
  Serial.println(ldrLeft);
  Serial.print("Right: ");
  Serial.println(ldrRight);

  // Determine motor movements based on differences
  if (abs(horizontalDifference) >= threshold) {
    if (horizontalDifference > 0) {
      servoH.write(servoH.read() + stepSize);
    } else {
      servoH.write(servoH.read() - stepSize);
    }
  }

  delay(100); // Small delay to allow for smooth movement
}

time_t toUtc(time_t local) {
  return local - utc_offset * 3600L;
}

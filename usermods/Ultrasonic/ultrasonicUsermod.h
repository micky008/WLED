#pragma once

#include <NewPing.h>
#include "wled.h"

#define MAX_DISTANCE 300
#define THRESHOLD_MAX 255
#define NB_SENSOR 4

/**
 * SPEC
 * I think the sensors will be installed at height of 275cm.
 *
 *
 * My thoughts are:
Greater than 255 = off
255-195 = RGB LEDs 0-59
195-135 = UV LEDs 60-120
135-0 = spot LED
*/



class UltraSonicUsermod : public Usermod {

private:

  class Pair {
  public:
    int min;
    int max;
    Pair() {}
    Pair(int max, int min) {
      this->min = min;
      this->max = max;
    }
  };

  class Sonar {
  public:
    uint8_t echo;
    uint8_t triger;
    int max1;
    int min1;
    int max2;
    int min2;
    int max3;
    int min3;
    Sonar() {}
    Sonar(int e, int t, Pair t1, Pair t2, Pair t3) {
      this->echo = (uint8_t)e;
      this->triger = (uint8_t)t;
      this->max1 = t1.max;
      this->min1 = t1.min;
      this->max2 = t2.max;
      this->min2 = t2.min;
      this->max3 = t3.max;
      this->min3 = t3.min;
    }
  };

  const int th1[4] = { 0,3,6,9 };
  const int th2[4] = { 1,4,7,10 };
  const int th3[4] = { 2,5,8,11 };

  // string that are used multiple time (this will save some flash memory)
  static const char _name[];
  static const char _enabled[];
  bool enabled = false;
  long lastTime = 0;
  NewPing* sensors[NB_SENSOR];
  Sonar sonars[NB_SENSOR] = {
    //Sonar(17, 5, Pair(120, 100), Pair(99, 40), Pair(39, 0)), //int e, int t
    Sonar(32, 33, Pair(THRESHOLD_MAX, 195), Pair(194,135), Pair(134,0)),
    Sonar(25, 26, Pair(THRESHOLD_MAX, 195), Pair(194,135), Pair(134,0)),
    Sonar(27, 14, Pair(THRESHOLD_MAX, 195), Pair(194,135), Pair(134,0)),
    Sonar(12, 13, Pair(THRESHOLD_MAX, 195), Pair(194,135), Pair(134,0)),
  };
  int captorFirst = 0;

public:

  inline void enable(bool enable) {
    enabled = enable;
  }

  inline bool isEnabled() {
    return enabled;
  }

  void setup() {
    for (int i = 0; i < NB_SENSOR; i++) {
      sensors[i] = new NewPing(sonars[i].triger, sonars[i].echo, MAX_DISTANCE);
      ledsoff(i);
    }
    strip.setBrightness(255, false);
  }

  void loop() {
    if (!enabled || strip.isUpdating()) {
      return;
    }

    if (millis() - lastTime > 500) {
      for (uint8_t i = 0; i < NB_SENSOR; i++) {
        long dist = sensors[i]->ping_cm();
        Serial.printf("sensor=%d dist=%ld\n", i, dist);
        if (dist >= sonars[i].min1 && dist <= sonars[i].max1) {
          Serial.printf("sensor=%d th1\n", i);
          strip.getSegment(th1[i]).setOpacity(254);
          strip.getSegment(th2[i]).setOpacity(0);
          strip.getSegment(th3[i]).setOpacity(0);
          colorUpdated(CALL_MODE_NOTIFICATION);
        }
        else if (dist >= sonars[i].min2 && dist <= sonars[i].max2) {
          Serial.printf("sensor=%d th2\n", i);
          strip.getSegment(th1[i]).setOpacity(0);
          strip.getSegment(th2[i]).setOpacity(254);
          strip.getSegment(th3[i]).setOpacity(0);
          colorUpdated(CALL_MODE_NOTIFICATION);
        }
        else if (dist >= sonars[i].min3 && dist <= sonars[i].max3) {
          Serial.printf("sensor=%d th3\n", i);
          strip.getSegment(th1[i]).setOpacity(0);
          strip.getSegment(th2[i]).setOpacity(0);
          strip.getSegment(th3[i]).setOpacity(254);
          colorUpdated(CALL_MODE_NOTIFICATION);
        }
        lastTime = millis();

      }
    }
  }

  void ledsoff(int i) {
    strip.getSegment(th1[i]).setOpacity(0);
    strip.getSegment(th2[i]).setOpacity(0);
    strip.getSegment(th3[i]).setOpacity(0);
  }

  void addToConfig(JsonObject& root) {
    if (root.containsKey(FPSTR(_name))) {
      return;
    }
    JsonObject top = root.createNestedObject(FPSTR(_name));
    JsonObject sensor1 = top.createNestedObject("sensor1");
    JsonObject sensor2 = top.createNestedObject("sensor2");
    JsonObject sensor3 = top.createNestedObject("sensor3");
    JsonObject sensor4 = top.createNestedObject("sensor4");

    top[FPSTR(_enabled)] = enabled;
    fillSensor(sensor1, sonars[0]);
    fillSensor(sensor2, sonars[1]);
    fillSensor(sensor3, sonars[2]);
    fillSensor(sensor4, sonars[3]);
  }

  void fillSensor(JsonObject& sensor, Sonar& so) {
    sensor["echo"] = so.echo;
    sensor["trigger"] = so.triger;
    sensor["max1"] = so.max1;
    sensor["min1"] = so.min1;
    sensor["max2"] = so.max2;
    sensor["min2"] = so.min2;
    sensor["max3"] = so.max3;
    sensor["min3"] = so.min3;
  }

  bool readFromConfig(JsonObject& root) {

    JsonObject top = root[FPSTR(_name)];

    if (top["enabled"].isNull()) {
      this->enabled = false;
      return false;
    }
    this->enabled = top["enabled"];
    JsonObject s1 = top["sensor1"];
    JsonObject s2 = top["sensor2"];
    JsonObject s3 = top["sensor3"];
    JsonObject s4 = top["sensor4"];
    peupleSonar(s1, 0);
    peupleSonar(s2, 1);
    peupleSonar(s3, 2);
    peupleSonar(s4, 3);
    return true;
  }

  void peupleSonar(JsonObject& sonar, uint8_t i) {
    sonars[i].echo = sonar["echo"];
    sonars[i].triger = sonar["trigger"];
    sonars[i].max1 = sonar["max1"];
    sonars[i].min1 = sonar["min1"];
    sonars[i].max2 = sonar["max2"];
    sonars[i].min2 = sonar["min2"];
    sonars[i].max3 = sonar["max3"];
    sonars[i].min3 = sonar["min3"];
  }

  void appendConfigData() {
    oappend(SET_F("addInfo('UltrasonicUsermod:sensor1:min1',1,'in cm');"));           // 0 is field type, 1 is actual field
    oappend(SET_F("addInfo('UltrasonicUsermod:sensor1:max1',1,'in cm');"));           // 0 is field type, 1 is actual field
  }

  uint16_t getId() {
    return USERMOD_ID_ULTRASONIC;
  }

};

// add more strings here to reduce flash memory usage
const char UltraSonicUsermod::_name[] PROGMEM = "UltrasonicUsermod";
const char UltraSonicUsermod::_enabled[] PROGMEM = "enabled";




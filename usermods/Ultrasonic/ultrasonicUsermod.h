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

  class Tuple {
  public:
    int min;
    int max;
    uint8_t idSegment;
    int maxLed;
    Tuple() {}
    Tuple(int min, int max, int idSeg, int maxLed) {
      this->min = min;
      this->max = max;
      this->idSegment = (uint8_t)idSeg;
      this->maxLed = maxLed;
    }
  };

  class Sonar {
  public:
    uint8_t echo;
    uint8_t triger;
    int min1;
    int max1;
    uint8_t idSegment1;
    int maxLed1;
    int min2;
    int max2;
    uint8_t idSegment2;
    int maxLed2;
    int min3;
    int max3;
    uint8_t idSegment3;
    int maxLed3;
    Sonar() {}
    Sonar(int e, int t, Tuple t1, Tuple t2, Tuple t3) {
      this->echo = (uint8_t)e;
      this->idSegment1 = t1.idSegment;
      this->max1 = t1.max;
      this->min1 = t1.min;
      this->idSegment2 = t2.idSegment;
      this->max2 = t2.max;
      this->min2 = t2.min;
      this->idSegment3 = t3.idSegment;
      this->max3 = t3.max;
      this->min3 = t3.min;
      this->triger = (uint8_t)t;
      this->maxLed1 = t1.maxLed;
      this->maxLed2 = t2.maxLed;
      this->maxLed3 = t3.maxLed;
    }
  };

  // string that are used multiple time (this will save some flash memory)
  static const char _name[];
  static const char _enabled[];
  bool enabled = false;
  long lastTime = 0;
  NewPing* sensors[NB_SENSOR];
  Sonar sonars[NB_SENSOR] = {
    Sonar(32, 33, Tuple(195, THRESHOLD_MAX, 1, 59), Tuple(135, 194, 2, 119), Tuple(0, 134, 3, 120)),
    Sonar(25, 26, Tuple(195, THRESHOLD_MAX, 4, 59), Tuple(135, 194, 5, 119), Tuple(0, 134, 6,120)),
    Sonar(27, 14, Tuple(195, THRESHOLD_MAX, 7, 59), Tuple(135, 194, 8, 119), Tuple(0, 134, 9,120)),
    Sonar(12, 13, Tuple(195, THRESHOLD_MAX, 10, 59), Tuple(135, 194, 11, 119), Tuple(0, 134, 12,120)),
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
    }
  }

  void loop() {
    if (!enabled || strip.isUpdating()) {
      return;
    }

    // do your magic here
    if (millis() - lastTime > 1000) {
      // for (uint8_t i = 0; i < NB_SENSOR; i++) {
      for (uint8_t i = 0; i < 1; i++) {
        long dist = sensors[i]->ping_cm();
        if (sonars[i].min1 >= dist && dist <= sonars[i].max1) {
          //maxLed = sonars[i].max1 - sonars[i].min1
          // X led = dist => (dist * sonars[i].maxLed1) / (sonars[i].max1 - sonars[i].min1)
          uint16_t stop = (dist * sonars[i].maxLed1) / (sonars[i].max1 - sonars[i].min1);
          strip.setSegment(sonars[i].idSegment1, 0, stop); //id start stop
        }
      }
      colorUpdated(CALL_MODE_DIRECT_CHANGE);
      lastTime = millis();
    }
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

    sensor["idSeg1"] = so.idSegment1;
    sensor["min1"] = so.min1;
    sensor["max1"] = so.max1;
    sensor["nbLeds1"] = so.maxLed1;

    sensor["idSeg2"] = so.idSegment2;
    sensor["min2"] = so.min2;
    sensor["max2"] = so.max2;
    sensor["nbLeds2"] = so.maxLed2;

    sensor["idSeg3"] = so.idSegment3;
    sensor["min3"] = so.min3;
    sensor["max3"] = so.max3;
    sensor["nbLeds3"] = so.maxLed3;
  }

  bool readFromConfig(JsonObject& root) {

    JsonObject top = root[FPSTR(_name)];

    if (top["sensor1"].isNull() || top["sensor2"].isNull() || top["sensor3"].isNull() || top["sensor4"].isNull()) {
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
    sonars[i].idSegment1 = sonar["idSeg1"];
    sonars[i].min1 = sonar["min1"];
    sonars[i].max1 = sonar["max1"];
    sonars[i].idSegment2 = sonar["idSeg2"];
    sonars[i].min2 = sonar["min2"];
    sonars[i].max2 = sonar["max2"];
    sonars[i].idSegment3 = sonar["idSeg3"];
    sonars[i].min3 = sonar["min3"];
    sonars[i].max3 = sonar["max3"];
  }

  void appendConfigData() {
    oappend(SET_F("addInfo('UltrasonicUsermod:min1',1,'in cm');"));           // 0 is field type, 1 is actual field
    oappend(SET_F("addInfo('UltrasonicUsermod:max1',1,'in cm');"));           // 0 is field type, 1 is actual field
  }

  uint16_t getId() {
    return USERMOD_ID_ULTRASONIC;
  }

};

// add more strings here to reduce flash memory usage
const char UltraSonicUsermod::_name[] PROGMEM = "UltrasonicUsermod";
const char UltraSonicUsermod::_enabled[] PROGMEM = "enabled";




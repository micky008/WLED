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

  class MySegment {
  public:
    int minCm;
    int maxCm;
    uint8_t idSegment;
    int maxLed;
    int minLed;
    MySegment(int _minCm, int _maxCm, int _minLed, int _maxLed, int _idSeg) {
      this->minCm = _minCm;
      this->maxCm = _maxCm;
      this->idSegment = (uint8_t)_idSeg;
      this->maxLed = _maxLed;
      this->minLed = _minLed;
    }
  };

  class MyStrip {
  public:
    MySegment* seg1 = nullptr;
    MySegment* seg2 = nullptr;
    MySegment* seg3 = nullptr;
    MyStrip(MySegment* _seg1, MySegment* _seg2, MySegment* _seg3) {
      this->seg1 = _seg1;
      this->seg2 = _seg2;
      this->seg3 = _seg3;
    }
  };


  class Sonar {
  public:
    uint8_t echo;
    uint8_t triger;
    MyStrip* strip = nullptr;

    Sonar(int e, int t, MyStrip* _strip) {
      this->echo = (uint8_t)e;
      this->triger = (uint8_t)t;
      this->strip = _strip;
    }
  };

  // string that are used multiple time (this will save some flash memory)
  static const char _name[];
  static const char _enabled[];
  bool enabled = false;
  long lastTime = 0;
  NewPing* sensors[NB_SENSOR];
  Sonar sonars[NB_SENSOR] = {
     Sonar(32, 33, new MyStrip(new MySegment(195, 255, 0, 59, 0), new MySegment(135, 194, 59, 119, 1), new MySegment(0, 134, 119, 120, 2))),
     Sonar(25, 26, new MyStrip(new MySegment(195, 255, 0, 59, 3), new MySegment(135, 194, 59, 119, 4), new MySegment(0, 134, 119, 120, 5))),
     Sonar(27, 14, new MyStrip(new MySegment(195, 255, 0, 59, 6), new MySegment(135, 194, 59, 119, 7), new MySegment(0, 134, 119, 120, 8))),
     Sonar(12, 13, new MyStrip(new MySegment(195, 255, 0, 59, 9), new MySegment(135, 194, 59, 119, 10), new MySegment(0, 134, 119, 120, 11)))
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
    Serial.begin(115200);
    for (int i = 0; i < NB_SENSOR; i++) {
      sensors[i] = new NewPing(sonars[i].triger, sonars[i].echo, MAX_DISTANCE);
    }
  }

  void loop() {
    if (!enabled || strip.isUpdating()) {
      return;
    }

    // do your magic here
    if (millis() - lastTime > 500) {
      // for (uint8_t i = 0; i < NB_SENSOR; i++) {
      for (uint8_t i = 0; i < 1; i++) {
        long dist = sensors[i]->ping_cm();
        Serial.printf("dist=%d\n", dist);
        // if (sonars[i].min1 >= dist && dist <= sonars[i].max1) {
        //   //maxLed = sonars[i].max1 - sonars[i].min1
        //   // X led = dist => (dist * sonars[i].maxLed1) / (sonars[i].max1 - sonars[i].min1)
        //   uint16_t stop = (dist * sonars[i].maxLed1) / (sonars[i].max1 - sonars[i].min1);
        //   Serial.printf("stop1=%d\n", stop);
        //   strip.setSegment(sonars[i].idSegment1, 0, stop); //id start stop
        //   colorUpdated(CALL_MODE_DIRECT_CHANGE);
        // }
        // else if (sonars[i].min2 >= dist && dist <= sonars[i].max2) {
        //   uint16_t stop = (dist * sonars[i].maxLed2) / (sonars[i].max2 - sonars[i].min2);
        //   strip.setSegment(sonars[i].idSegment2, 0, stop);
        //   colorUpdated(CALL_MODE_DIRECT_CHANGE);
        // }
        // else if (sonars[i].min3 >= dist && dist <= sonars[i].max3) {
        //   uint16_t stop = (dist * sonars[i].maxLed3) / (sonars[i].max3 - sonars[i].min3);
        //   strip.setSegment(sonars[i].idSegment3, 0, stop);
        //   colorUpdated(CALL_MODE_DIRECT_CHANGE);
        // }
      }

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

    JsonObject mystrip = sensor.createNestedObject("myStrip");
    JsonObject mySegment1 = mystrip.createNestedObject("mySegment1");
    JsonObject mySegment2 = mystrip.createNestedObject("mySegment2");
    JsonObject mySegment3 = mystrip.createNestedObject("mySegment3");

    mySegment1["maxCm"] = so.strip->seg1->maxCm;
    mySegment1["minCm"] = so.strip->seg1->minCm;
    mySegment1["idSeg"] = so.strip->seg1->idSegment;
    mySegment1["maxLed"] = so.strip->seg1->maxLed;
    mySegment1["minLed"] = so.strip->seg1->minLed;

    mySegment2["maxCm"] = so.strip->seg2->maxCm;
    mySegment2["minCm"] = so.strip->seg2->minCm;
    mySegment2["idSeg"] = so.strip->seg2->idSegment;
    mySegment2["maxLed"] = so.strip->seg2->maxLed;
    mySegment2["minLed"] = so.strip->seg2->minLed;

    mySegment3["maxCm"] = so.strip->seg3->maxCm;
    mySegment3["minCm"] = so.strip->seg3->minCm;
    mySegment3["idSeg"] = so.strip->seg3->idSegment;
    mySegment3["maxLed"] = so.strip->seg3->maxLed;
    mySegment3["minLed"] = so.strip->seg3->minLed;

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
    JsonObject mystrip = sonar["myStrip"];
    JsonObject mySegment1 = mystrip["mySegment1"];
    JsonObject mySegment2 = mystrip["mySegment2"];
    JsonObject mySegment3 = mystrip["mySegment3"];

    Serial.printf("sonars[i].strip->seg1->idSegment %s\n", mySegment1["idSeg"]);

    // if (sonars[i].strip == nullptr) {
    //   sonars[i].strip = new MyStrip(nullptr, nullptr, nullptr);
    //   sonars[i].strip->seg1 = new MySegment(0,0,0,0,0);
    //   sonars[i].strip->seg2 = new MySegment(0,0,0,0,0);
    //   sonars[i].strip->seg3 = new MySegment(0,0,0,0,0);
    // }

    // sonars[i].strip->seg1->idSegment = mySegment1["idSeg"];
    // sonars[i].strip->seg1->maxCm = mySegment1["maxCm"];
    // sonars[i].strip->seg1->maxLed = mySegment1["maxLed"];
    // sonars[i].strip->seg1->minCm = mySegment1["minCm"];
    // sonars[i].strip->seg1->minLed = mySegment1["minLed"];

    // sonars[i].strip->seg2->idSegment = mySegment2["idSeg"];
    // sonars[i].strip->seg2->maxCm = mySegment2["maxCm"];
    // sonars[i].strip->seg2->maxLed = mySegment2["maxLed"];
    // sonars[i].strip->seg2->minCm = mySegment2["minCm"];
    // sonars[i].strip->seg2->minLed = mySegment2["minLed"];

    // sonars[i].strip->seg3->idSegment = mySegment3["idSeg"];
    // sonars[i].strip->seg3->maxCm = mySegment3["maxCm"];
    // sonars[i].strip->seg3->maxLed = mySegment3["maxLed"];
    // sonars[i].strip->seg3->minCm = mySegment3["minCm"];
    // sonars[i].strip->seg3->minLed = mySegment3["minLed"];
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



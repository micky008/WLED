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
    Tuple() {}
    Tuple(int min, int max, int idSeg) {
      this->min = min;
      this->max = max;
      this->idSegment = (uint8_t)idSeg;
    }
  };

  class Range {
  public:
    Tuple* t1 = nullptr;
    Tuple* t2 = nullptr;
    Tuple* t3 = nullptr;
    Range() {
    }
    Range(Tuple* t1, Tuple* t2, Tuple* t3) {
      this->t1 = t1;
      this->t2 = t2;
      this->t3 = t3;
    }
    ~Range() {
      delete t1;
      delete t2;
      delete t3;
    }
  };

  class Sonar {
  public:
    uint8_t echo;
    uint8_t triger;
    Range* range;
    Sonar() {}
    Sonar(int e, int t, Range* r) {
      this->echo = (uint8_t)e;
      this->range = r;
      this->triger = (uint8_t)t;
    }
    ~Sonar() {
      delete range;
    }
  };

  // string that are used multiple time (this will save some flash memory)
  static const char _name[];
  static const char _enabled[];
  bool enabled = false;
  long lastTime = 0;
  NewPing* sensors[NB_SENSOR];
  Sonar* sonars[NB_SENSOR];
  int captorFirst = 0;

public:
  // non WLED related methods, may be used for data exchange between usermods (non-inline methods should be defined out of class)

  inline void enable(bool enable) {
    enabled = enable;
  }

  inline bool isEnabled() {
    return enabled;
  }


  void setup() {
    //uint8_t maxSeg = strip.getMaxSegments();
    for (int i = 0; i < NB_SENSOR; i++) {
      sensors[i] = new NewPing(sonars[i]->triger, sonars[i]->echo, MAX_DISTANCE);
    }
  }

  void loop() {
    if (!enabled || strip.isUpdating()) {
      return;
    }

    // do your magic here
    if (millis() - lastTime > 1000) {
      // for (uint8_t i = 0; i < NB_SENSOR; i++) {
      //   long dist = sensors[i]->ping_cm();
      //   if (dist <= sonars[i]->threshold) {
      //     Segment& seg = strip.getSegment(0);
      //     //seg.
      //     // strip.
      //   }
      // }
      lastTime = millis();
    }
  }

  void addToConfig(JsonObject& root) {
    JsonObject top = root.createNestedObject(FPSTR(_name));
    JsonObject sensor1 = top.createNestedObject("sensor1");
    JsonObject sensor2 = top.createNestedObject("sensor2");
    JsonObject sensor3 = top.createNestedObject("sensor3");
    JsonObject sensor4 = top.createNestedObject("sensor4");

    Sonar s1(32, 33, getRange(1));
    Sonar s2(25, 26, getRange(4));
    Sonar s3(27, 14, getRange(7));
    Sonar s4(12, 13, getRange(10));

    top[FPSTR(_enabled)] = enabled;
    fillSensor(sensor1, s1);
    fillSensor(sensor2, s2);
    fillSensor(sensor3, s3);
    fillSensor(sensor4, s4);

  }

  Range* getRange(int idFirstSeg) {
    Tuple* t1 = new Tuple(195, THRESHOLD_MAX, idFirstSeg);
    Tuple* t2 = new Tuple(135, 194, idFirstSeg + 1);
    Tuple* t3 = new Tuple(0, 134, idFirstSeg + 2);
    return new Range(t1, t2, t3);
  }

  void fillSensor(JsonObject& sensor, Sonar& so) {
    sensor["echo"] = so.echo;
    sensor["trigger"] = so.triger;
    JsonObject range1 = sensor.createNestedObject("range1");
    range1["idSeg"] = so.range->t1->idSegment;
    range1["min"] = so.range->t1->min;
    range1["max"] = so.range->t1->max;
    JsonObject range2 = sensor.createNestedObject("range2");
    range2["idSeg"] = so.range->t2->idSegment;
    range2["min"] = so.range->t2->min;
    range2["max"] = so.range->t2->max;
    JsonObject range3 = sensor.createNestedObject("range3");
    range3["idSeg"] = so.range->t3->idSegment;
    range3["min"] = so.range->t3->min;
    range3["max"] = so.range->t3->max;
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
    if (sonars[0] == nullptr) {
      sonars[0] = new Sonar();
      sonars[1] = new Sonar();
      sonars[2] = new Sonar();
      sonars[3] = new Sonar();
    }
    peupleSonar(s1, 0);
    peupleSonar(s2, 1);
    peupleSonar(s3, 2);
    peupleSonar(s4, 3);
    return true;
  }

  void peupleSonar(JsonObject& sonar, uint8_t i) {
    sonars[i]->echo = sonar["echo"];
    sonars[i]->triger = sonar["trigger"];
    JsonObject range1 = sonar["range1"];
    JsonObject range2 = sonar["range2"];
    JsonObject range3 = sonar["range3"];
    sonars[i]->range = new Range();
    sonars[i]->range->t1 = new Tuple();
    sonars[i]->range->t1->idSegment = range1["idSeg"];
    sonars[i]->range->t1->min = range1["min"];
    sonars[i]->range->t1->max = range1["max"];
    sonars[i]->range->t2 = new Tuple();
    sonars[i]->range->t2->idSegment = range2["idSeg"];
    sonars[i]->range->t2->min = range2["min"];
    sonars[i]->range->t2->max = range2["max"];
    sonars[i]->range->t3 = new Tuple();
    sonars[i]->range->t3->idSegment = range3["idSeg"];
    sonars[i]->range->t3->min = range3["min"];
    sonars[i]->range->t3->max = range3["max"];

  }

  void appendConfigData() {
    oappend(SET_F("addInfo('UltrasonicUsermod:min',1,'in cm');"));           // 0 is field type, 1 is actual field
    oappend(SET_F("addInfo('UltrasonicUsermod:max',1,'in cm');"));           // 0 is field type, 1 is actual field

  }

  uint16_t getId() {
    return USERMOD_ID_ULTRASONIC;
  }

};

// add more strings here to reduce flash memory usage
const char UltraSonicUsermod::_name[] PROGMEM = "UltrasonicUsermod";
const char UltraSonicUsermod::_enabled[] PROGMEM = "enabled";




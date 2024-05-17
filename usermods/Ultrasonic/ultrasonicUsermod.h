#pragma once

#include <NewPing.h>
#include "wled.h"

#define MAX_DISTANCE 200


class UltraSonicUsermod : public Usermod {

private:
  struct Sonar {

  public:
    uint8_t echo;
    uint8_t triger;
    uint8_t idSegment1;
    uint8_t idSegment2;
  };
  // string that are used multiple time (this will save some flash memory)
  static const char _name[];
  static const char _enabled[];
  bool enabled = false;
  long lastTime = 0;
  NewPing* sensors[4];
  Sonar* sonars[4];

public:
  // non WLED related methods, may be used for data exchange between usermods (non-inline methods should be defined out of class)

  inline void enable(bool enable) {
    enabled = enable;
  }

  inline bool isEnabled() {
    return enabled;
  }


  void setup() {
    sensors[0] = new NewPing(sonars[0]->triger, sonars[0]->echo, MAX_DISTANCE);
    sensors[1] = new NewPing(sonars[1]->triger, sonars[1]->echo, MAX_DISTANCE);
    sensors[2] = new NewPing(sonars[2]->triger, sonars[2]->echo, MAX_DISTANCE);
    sensors[3] = new NewPing(sonars[3]->triger, sonars[3]->echo, MAX_DISTANCE);
  }

  void loop() {
    if (!enabled || strip.isUpdating()) {
      return;
    }

    // do your magic here
    if (millis() - lastTime > 1000) {
      // Serial.println("I'm alive!");
      lastTime = millis();
    }
  }

  void addToConfig(JsonObject& root) {
    JsonObject top = root.createNestedObject(FPSTR(_name));
    JsonObject sensor1 = root.createNestedObject("sensor1");
    JsonObject sensor2 = root.createNestedObject("sensor2");
    JsonObject sensor3 = root.createNestedObject("sensor3");
    JsonObject sensor4 = root.createNestedObject("sensor4");

    sensor1["echo"] = 32;
    sensor1["trigger"] = 33;
    sensor1["segId1"] = 1;
    sensor1["segId2"] = 2;

    sensor2["echo"] = 25;
    sensor2["trigger"] = 26;
    sensor2["segId1"] = 3;
    sensor2["segId2"] = 4;

    sensor3["echo"] = 27;
    sensor3["trigger"] = 14;
    sensor3["segId1"] = 5;
    sensor3["segId2"] = 6;

    sensor4["echo"] = 12;
    sensor4["trigger"] = 13;
    sensor4["segId1"] = 7;
    sensor4["segId2"] = 8;


    top[FPSTR(_enabled)] = enabled;

  }

  bool readFromConfig(JsonObject& root) {

    JsonObject top = root[FPSTR(_name)];

    if (top["sensor1"].isNull() || top["sensor2"].isNull() || top["sensor3"].isNull() || top["sensor4"].isNull()) {
      enabled = false;
      return false;
    }
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

  void peupleSonar(JsonObject& sonar, byte i) {
    sonars[i]->echo = sonar["echo"];
    sonars[i]->triger = sonar["trigger"];
    sonars[i]->idSegment1 = sonar["segId1"];
    sonars[i]->idSegment2 = sonar["segId2"];
  }

  void appendConfigData() {
    //   oappend(SET_F("addInfo('WireGuard:host',1,'Server Hostname');"));           // 0 is field type, 1 is actual field

  }

  uint16_t getId() {
    return USERMOD_ID_ULTRASONIC;
  }



};

// add more strings here to reduce flash memory usage
const char UltraSonicUsermod::_name[] PROGMEM = "UltrasonicUsermod";
const char UltraSonicUsermod::_enabled[] PROGMEM = "enabled";




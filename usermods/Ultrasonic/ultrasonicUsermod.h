#pragma once

#include <NewPing.h>
#include "wled.h"

#define MAX_DISTANCE 200

class Sonar {

public:
  byte echo;
  byte triger;
  byte idSegment1;
  byte idSegment2;
  bool enabled = false;
  Sonar(byte e, byte t, byte id1, byte id2, bool en) {
    this->echo = e;
    this->triger = t;
    this->idSegment1 = id1;
    this->idSegment2 = id2;
    this->enabled = en;
  }
};

class UltraSonicUsermod : public Usermod {

private:
  // string that are used multiple time (this will save some flash memory)
  static const char _name[];
  static const char _enabled[];
  bool enabled = false;
  long lastTime = 0;
  NewPing sensors[4];
  Sonar sonars[4];

public:
  // non WLED related methods, may be used for data exchange between usermods (non-inline methods should be defined out of class)

  inline void enable(bool enable) {
    enabled = enable;
  }

  inline bool isEnabled() {
    return enabled;
  }


  void setup() {
    sensors[0] = NewPing(sonars[0].triger, sonars[0].echo, MAX_DISTANCE);
    if (sonars[1].enabled) {
      sensors[1] = NewPing(sonars[1].triger, sonars[1].echo, MAX_DISTANCE);
    }
    if (sonars[2].enabled) {
      sensors[2] = NewPing(sonars[2].triger, sonars[2].echo, MAX_DISTANCE);
    }
    if (sonars[3].enabled) {
      sensors[3] = NewPing(sonars[3].triger, sonars[3].echo, MAX_DISTANCE);
    }
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
    sonars[0] = { s1["echo"], s1["trigger"],s1["segId1"],s1["segId2"],true };
    sonars[1] = { s2["echo"], s2["trigger"],s2["segId1"],s2["segId2"],true };
    sonars[2] = { s3["echo"], s3["trigger"],s3["segId1"],s3["segId2"],true };
    sonars[3] = { s4["echo"], s4["trigger"],s4["segId1"],s4["segId2"],true };
    return true;
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



#include "SmartLamp.h"

SmartLamp smartLamp;

void setup() {
  smartLamp.setup();
}

void loop() {
  smartLamp.detectLight();
}
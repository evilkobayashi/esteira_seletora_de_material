#include "sensor.h"
#include "config.h"

void sensorInit() {
    pinMode(PIN_SENSOR_DIGITAL, INPUT);
}

bool sensorDigital() {
    return digitalRead(PIN_SENSOR_DIGITAL) == HIGH;
}

int sensorAnalog() {
    return analogRead(PIN_SENSOR_ANALOG);
}

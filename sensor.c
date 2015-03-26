// Utils about sensor
#include "sensor.h"

void add_to_sensors(SENSOR_INFO* snr) {
    int i = 0;
    for (; i < MAX_SENSORS_COUNT && 
            (sensors[i].sensor_id == snr->sensor_id ||
            sensors[i].sensor_id == 0);
        i++) {} // An empty "for". So cute :-)

    // no sensor with specified id found. add new.
    strcpy(sensors[i].sensor_name, snr->sensor_name);
    strcpy(sensors[i].sensor_trigger, snr->sensor_trigger);
    sensors[i].sensor_value = snr->sensor_value;
    sensors[i].sensor_type = snr->sensor_type;
}

void add_to_sensors_with_attr(int id, char* name, char* trigger, int type, int value) {
    SENSOR_INFO snr;

    if (id == 0) return;
    snr.sensor_id = id;
    strcpy(snr.sensor_name, name);
    strcpy(snr.sensor_trigger, trigger);
    snr.sensor_type = type;
    snr.sensor_value = value;

    add_to_sensors(&snr);
}
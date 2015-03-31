// Utils about sensor
#include "sensor.h"
#include "trigger.h"
#include "sensor_private.h"

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

SENSOR_INFO* get_sensor_at(int i) {
	return &sensors[i];
}

void sensor_array_init() {
    // initialize sensors array
    int i = 0;
    for (; i <= MAX_SENSORS_COUNT; i++) {
        sensors[i].sensor_id = i + 1;
        sensors[i].sensor_name[0] = 0;
        sensors[i].sensor_trigger[0] = 0;
        sensors[i].sensor_type = SENSOR_TYPE_LIGHT;
        sensors[i].sensor_value = 0;
    }
    // TODO we should not initialize trigger array here
    // but I can't change STM32 code
#ifdef USE_TRIGGER_ARRAY
    trigger_array_init();
#endif
}

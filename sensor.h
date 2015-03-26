// Definitons about sensor

#ifndef SENSOR_H
#define SENSOR_H

#define SENSOR_TYPE_LIGHT 0
#define SENSOR_TYPE_ELECTRICITY 1
#define SENSOR_TYPE_MOTION 2
#define SENSOR_TYPE_SWITCH 3
#define SENSOR_TYPE_IR 4

#define SENSOR_NAME_BUFFER_SIZE 100
#define SENSOR_TRIGGER_BUFFER_SIZE 500
#define MAX_SENSORS_COUNT 50

typedef struct {
    int sensor_id;
    int sensor_type;
    char sensor_name[SENSOR_NAME_BUFFER_SIZE];
    int sensor_value;
    char sensor_trigger[SENSOR_TRIGGER_BUFFER_SIZE];
} SENSOR_INFO;

// Is this memory costly?
// If sensor_id == 0, means this array's content has come to an end.
// Like char name[100]="ImThisShortComeToBeatMe";
SENSOR_INFO sensors[MAX_SENSORS_COUNT];

// Add a new sensor to sensors array
// Auto determines replace or add new
void add_to_sensors(SENSOR_INFO* snr);
// Same as add_to_sensors
// except this has different parameters
void add_to_sensors_with_attr(int id, char* name, char* trigger, int type, int value);
#endif SENSOR_H
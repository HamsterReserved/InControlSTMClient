#include "stub_stm32.h"
#include <stdio.h>

void delay(long mills) {

}

void read_device_id(char *buf) {
    strcpy(buf, "1");
}
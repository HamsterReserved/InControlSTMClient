#include "stub_stm32.h"
#include <stdio.h>
#include <string.h>

void delay(long mills) {

}

void read_device_id(char *buf) {
    strcpy(buf, "1");
}

void log(const char* logstr) {
    printf("LOG: %s\n", logstr);
}
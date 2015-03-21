/*
 Stub file for STM32. Used for debugging the logic in VS or anything else.
 Contains specific function on STM32 platform.
*/

#ifndef STUB_STM32_H
#define STUB_STM32_H

#define DEVICE_ID_LENGTH 20

void delay(long mills);
// Read device ID into buf
void read_device_id(char* buf);
void log(const char* logstr);
void update_display();

long get_current_time();
#endif
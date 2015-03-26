/*
 * sms.h
 *
 *  Created on: 2015-2-27
 *      Author: Hamster Tian
 *
 * Used for SMS related functions
 */

#ifndef SMS_H_
#define SMS_H_

#define NUMBER_BUFFER_SIZE 20
#define PDU_HEAD_BUFFER_SIZE 300
#define MESSAGE_BUFFER_SIZE 200

#include "global_config.h"

typedef struct {
    char* dest_number; // Destination phone number (with +86) in TEXT. Currently supports China operators only. (8 bytes length)
    char* service_center; // SMS service center. Will set 00 for SCA if null in TEXT. Chinese only for now.
    char* content; // Original text
} SMS_SUBMIT_PARAM;

void send_sms(char* buffer, SMS_SUBMIT_PARAM* sms);
void invert_number_string(char* dest, const char* src);
void send_command(const char* buf);
int check_if_chinese_character(const char* in_str);
#endif /* SMS_H_ */

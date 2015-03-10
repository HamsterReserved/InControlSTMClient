/*
 * sms.c
 *
 *  Created on: 2015-2-27
 *      Author: Hamster
 *  有一件事很重要所有要说三次：
 *  注意短信是以十六进制字符串发送的，而不是十六进制对应的ASCII字符！
 *  所以在处理的时候要特别注意自己现在在处理的是【字面的】十六进制字符串（"0F"）还是【真正的】单个字符（15）！
 *  所以在处理的时候要特别注意自己现在在处理的是【字面的】十六进制字符串（"0F"）还是【真正的】单个字符（15）！
 *  所以在处理的时候要特别注意自己现在在处理的是【字面的】十六进制字符串（"0F"）还是【真正的】单个字符（15）！
 */
#include "sms.h"
#include "stub_stm32.h"
#include "modem_common.h"
#include <string.h>
#include <stdio.h>

#define DEBUG

/************************************************************************/
/* Invert a byte array ((+) 86 15 52 72 70 00 0F -> 68 51 25 27 07 00 F0)
/* NOTE! 15527270000 is TEXT!
/* And this function does STRCPY as well
/************************************************************************/
void invert_number_string(char* dest, const char* src) {
	int s_len = strlen(src);
	int cur_pos = 0;

	while (cur_pos <= s_len - 1) {
		if (s_len - cur_pos == 1) {
			dest[cur_pos] = 'F';
		} else {
			dest[cur_pos] = src[cur_pos + 1];
		}
		dest[cur_pos + 1] = src[cur_pos];
		cur_pos += 2;
	}
}

/************************************************************************/
/* Build a PDU into (param)buffer    
/* First paramtere is not used... this function sends it.
/************************************************************************/
void send_sms(char* buffer, SMS_SUBMIT_PARAM* sms) {
	char pdu_head[PDU_HEAD_BUFFER_SIZE];
	char msg_buffer[MESSAGE_BUFFER_SIZE];
	char service_number_buffer[NUMBER_BUFFER_SIZE];
	char target_phone_num_buffer[NUMBER_BUFFER_SIZE];

    int is_chinese = check_if_chinese_character(sms->content);
	int content_length = strlen(sms->content);
	char content_length_buffer[3]; // With 0x00 end. Max. 70 in decimal
	int head_length = 0;
	char head_length_buffer[3]; // With 0x00 end

	char end_of_message[] = {0x0A, 0x00}; // 0x00 is string end

    
    if (is_chinese) {
        memset(service_number_buffer, 0, NUMBER_BUFFER_SIZE); // Clear things or strcat will fail
        memset(target_phone_num_buffer, 0, NUMBER_BUFFER_SIZE);
        memset(pdu_head, 0, PDU_HEAD_BUFFER_SIZE);
        memset(msg_buffer, 0, MESSAGE_BUFFER_SIZE);
        memset(content_length_buffer, 0, 3);
        memset(head_length_buffer, 0, 3);

        if (sms->service_center != NULL) { // Specificed message center
            strcat(service_number_buffer, "0891"); // Center number length(8) and "+"
            invert_number_string(service_number_buffer + 4, sms->service_center + 1); // Skip the first 0891 and +
        }
        else {
            strcpy(service_number_buffer, "00"); // Not 00 in hex. Empty SCA.
        }

        strcpy(target_phone_num_buffer, "0D91"); // Target phone number length(different from SCA, 13 here with +86) and +
        invert_number_string(target_phone_num_buffer + 4, sms->dest_number + 1); // Skip +

        strcat(pdu_head, service_number_buffer);
        strcat(pdu_head, "1100"); // Parameter Type + MR
        strcat(pdu_head, target_phone_num_buffer);

        strcat(pdu_head, "000800"); // Protocol and data coding scheme (UCS2 here) and vaildity period

        sprintf(content_length_buffer, "%02X", content_length / 2);

        strcat(pdu_head, content_length_buffer);
        // PDU Head ends here

        strcpy(msg_buffer, sms->content);
        strcat(msg_buffer, end_of_message); // famous 0x0A
        // PDU content ends here

        head_length = strlen(pdu_head) - strlen(service_number_buffer);
        sprintf(head_length_buffer, "%d", head_length);

        send_command("AT+CMGF=0\r"); // Set PDU mode
        delay(50);
        // TODO Check for OK
        
        send_text("AT+CMGS=");
        send_text(head_length_buffer);
        send_text("\r");
        set_command(COMMAND_CMGS);
        // AT+CMGS=40 etc

        delay(50); // Wait for >
        send_text(pdu_head);
        send_text(msg_buffer);
        // No need to \r. msg_buffer contains it.
        // Wait for OK in calling functions
    } else {
        // English message. Using text mode is enough.
        head_length = strlen(sms->dest_number);
        sprintf(head_length_buffer, "%d", head_length);

        send_command("AT+CMGF=1\r"); // Set text mode
        delay(50);
        send_text("AT+CMGS=");
        send_text(sms->dest_number);
        send_text("\r");
        set_command(COMMAND_CMGS);
        // AT+CMGS=+8615527270000
        
        delay(50); // Wait for >
        send_text(sms->content);
        send_text(end_of_message);
        // Wait for OK in calling functions
    }
}

/*
 * Should be normal string. Not HEX.
 */
int check_if_chinese_character(const char* in_str) {
    int slen = strlen(in_str);
    int cur_pos = 0;
    for (; cur_pos < slen - 1; cur_pos += 2) {
        if (in_str[cur_pos] < 0) // Normally only Chinese characters exceed this (0x80, become negative when signed)
            return 1;
    }
    return 0;
}

/*
 * TODO Not implemented. Should convert ASCII Chinese to UCS2
 */
void ucs2_encode_string(char* out, const char* in) {
    out = in;
}

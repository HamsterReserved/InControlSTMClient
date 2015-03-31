/*
 Internal trigger array. ONLY INCLUDE IN trigger.c!
 */

#ifndef TRIGGER_PRIVATE_H
#define TRIGGER_PRIVATE_H

#include "trigger.h"

// We plan to split trigger info and sensor info
TRIGGER_INFO triggers[MAX_TRIGGER_COUNT];
#endif
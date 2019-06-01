#ifndef DEF_H
#define DEF_H

/* Gen */
#include "utils.h"
#define BYTE_INV 0.003921568627

/* Gen - SW Debounce */
#define DEBOUNCE_TIME 50
#define WARM_UP_TIME  500
#define STORE_TIME    2500

/* Serial */
#define SERIAL_BAUD 115200

/* EEPROM */
#include <EEPROM.h>
#define EEPROM_ADDRESS      0
#define EEPROM_MEM_SIZE     4096    // using EEPROM.length() instead
#define EEPROM_USED_MEM     

/* Project */
#define NUM_LIGHTS 10
#define NUM_FADERS 11
#define NUM_CUES   10

#endif
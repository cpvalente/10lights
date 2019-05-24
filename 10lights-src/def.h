#ifndef DEF_H
#define DEF_H

/* Gen */

/* Gen - SW Debounce */
#define DEBOUNCE_TIME 200
#define WARM_UP_TIME 500

/* Serial */
#define SERIAL_BAUD 115200

/* EEPROM */
#include <Wire.h>
#define disk1 0x50          // !! check for mega
#define EEPROM_ADDRESS 0

/* Project */
#define NUM_LIGHTS 10
#define NUM_FADERS 11

#endif
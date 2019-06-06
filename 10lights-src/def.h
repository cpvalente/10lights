#ifndef DEF_H
#define DEF_H

/* Gen */
#include "libs/digitalfast.h"
#include "libs/analogfast.h"
#include "utils.h"
#define BYTE_COEF       0.003921568627

#define LOW_PASS        0.65
#define HIGH_PASS       0.3

/* Serial */
#define SERIAL_BAUD 115200
#ifdef DEBUG
    #define DEBUG_PRINTLN(x)  Serial.println (x)
    #define DEBUG_PRINT(x)    Serial.print (x)
#else
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINT(x)
#endif

/* Gen - SW Debounce */
#define DEBOUNCE_TIME 50
#define WARM_UP_TIME  500
#define ACTION_TIME   2500

/* Gen - IO */
#define STORE_PIN   0b10000000
#define BACK_PIN    0b00000010
#define GO_PIN      0b00000001

/* EEPROM */
#include <EEPROM.h>
#define EEPROM_ADDRESS      0
#define EEPROM_MEM_SIZE     4096    // using EEPROM.length() instead

/* Project */
#define NUM_LIGHTS 10
#define NUM_FADERS 11
#define NUM_CUES   10
#define NUM_MODES  3
#define MAX_FADE   10000

/* Pin Assignment */
const uint8_t analogInputs[]      {A10, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9}; // Faders, first is master
const uint8_t digitalOutputsPWM[] {2,   12, 11, 10,  9,  8,  7 , 6,  5,  4,  3}; // channel indicator LED, first is master
const uint8_t digitalOutputs[]    {22, 27, 23, 28, 24, 29, 25, 30, 26, 31};      // cue indicator LED
const uint8_t digitalInputs[]     {13, 52, 53};                                  // store, back, go
const uint8_t mode_select[]       {19, 20, 21};                                  // mode indicator (1-3)
const uint8_t time_indicator[]    {41, 40, 39, 38, 37, 36, 35, 34, 33, 32};      // time indicator LED-bargraph, first is segment 1
const uint8_t hlt_button[]        {43, 45, 47, 49, 51, 50, 48, 46, 44, 42};      // buttons for channel highlighting, first is CH1


#endif

/*
 *  10 lights
 *  -------------
 *  Summer project with Stu, lighting console for 10 LEDs
 *
 *  Code by Carlos Valente
 *  
 *  TODO:
 *  Gen
 *  - Three button surprise
 *  - debug modes: verbose
 *  Functionality
 *  - overflow on fade?
 *  - Some sort of erase memory confirmation, like some LEDs flash or someting
 *  - the LED bargraph thingy indicating time
 *  - the highlight button
 *  INPUT
 *  - ADC prescaler
 *  LEDs
 *  - indicator LEDs to flash according to mode select 
 */
 
#include "def.h"

/* Gen - State Machine */
enum states {MODE_1, MODE_2, MODE_3};
uint8_t state = MODE_1;
uint32_t timeNow, lastBtnRead;

/* Gen - Lighting States */
uint8_t lightingData[NUM_CUES][NUM_FADERS];
uint8_t selectedCue;
uint32_t fadeTime;
bool bFading;               // initiate fade
bool bRecordCue;            // record fader data in cue
bool bClearEEPROM;          // reset EEPROM data
bool bClearRunningData;     // clear running data
bool bClearIndicators;      // clear indicator LEDs

/* Gen - Pin Values Buttons */
bool store, back, go;
bool prevStore, prevBack, prevGo;
uint32_t lastStore, lastBack, lastGo;

/* Gen - Pin Values Faders */
uint8_t faderValues[NUM_FADERS];     // values from faders at each iteration
uint8_t values[NUM_FADERS];          // values for output
uint8_t leds[NUM_CUES];              // values for indicator leds

/* Include code files */
#include "io.h"
#include "eeprom.h"


void setup(){
    /* Pin Assignments */
    init_io();
 
    /* Serial */
    Serial.begin(115200);

    /* initialize data from EEPROM memory */
    init_from_eeprom(EEPROM_ADDRESS);

    /* Initialize aux */
    selectedCue = 0;
    bRecordCue = bClearEEPROM = bClearRunningData = bClearIndicators = false;
    lastBtnRead = 0;
}

void loop(){
    timeNow = millis();             // get iteration time
    read_inputs();                  // this should only happen if needed
    loop_execute( check_mode() );   // call state machine
    called_actions();               // run user actions
    refresh_outputs();              // refresh LEDs

    /* iterate flags */
    prevGo      = go;
    prevBack    = back;
    prevStore   = store;

    /* reset flags */
    bClearIndicators    = false;
    bClearEEPROM        = false;
    bClearRunningData   = false;
    bRecordCue          = false;

    /* aux */
    DEBUG_PLOT("\n");
}

uint8_t check_mode(){
    // reads mode selection input
    uint8_t called_mode = state;
    bool bStartFade = false;

    // store changes between modes, check everytime
    if (store) {
        // Store button is pressed
        if (prevStore) {
            // Store button was pressed in previous iteration
            if (timeNow - lastStore > ACTION_TIME) {
                /* Store button has been pressed long enough to call action,
                 * Enter cycle Mode */
                if (go) called_mode++;
                if (back) called_mode--;
                if (called_mode >= NUM_MODES) called_mode = 0;
            }
        } else {
            // start counter
            lastStore = timeNow;
        }
    } else {
        if (prevStore && (called_mode == MODE_2)) {
            bRecordCue = true;
        }
    } 

    if (called_mode == MODE_1) {
        // delete only in mode 1
        if (back && go) { 
            // both buttons are pressed
            if (!prevGo && !prevBack) {
                // were not pressed before, start counting
                lastGo = lastBack = timeNow;
            } else {
                // pressed in previous iteration, check time
                if ((timeNow - lastGo > ACTION_TIME) && (timeNow - lastBack > ACTION_TIME)) {
                    // if both buttons where pressed for ACTION TIME we call clear
                    bClearEEPROM = true;
                    bClearRunningData = true;
                    // give visual feedback
                    flash_indicators();
                    // and reset flags
                    back = go = false;
                }
            }
        }
    } else {
        // go and go backwards only work on mode 2 and 3

        // go backwards
        if (back && !prevBack) { 
            selectedCue -= 1;
            if (selectedCue >= NUM_CUES) {
                selectedCue = NUM_CUES - 1;
            }
            bStartFade = true;
        }

        // go forwards
        if (go && !prevGo) {
            selectedCue = (selectedCue + 1) % NUM_CUES;
            bStartFade = true;
        }

        if (bStartFade) {
            lastGo = timeNow;
            bFading = true;
        }
    }

    DEBUG_PRINT("Selected Cue: ");
    DEBUG_PRINTLN(selectedCue);

    return called_mode;
}

void loop_execute(uint8_t called_mode){

    if (called_mode < 3) state = called_mode;

    DEBUG_PRINT("Mode: ");
    DEBUG_PRINTLN(state + 1);

    switch (state) {

        case MODE_1:        // Fader is value
            // calculate values to pass
            for (int i = 1; i < NUM_FADERS; i++) {
                values[i] = cap(faderValues[i], faderValues[0]);
            }
            values[0] = faderValues[0]; // master not affected

        break;

        case MODE_2:        // Record
            // calculate values for leds
            for (int i = 1; i < NUM_FADERS; i++) {
                values[i] = faderValues[i];
            }
            values[0] = faderValues[0]; // master not affected

            // calculate values for indicator LEDs
            led_from_selected_cue();
        break;

        case MODE_3:        // Playback
            uint32_t fadeTimeElapsed;
            float step = 1.0f;

            fadeTime = lightingData[selectedCue][0];
            DEBUG_PRINT("Cue fade time ");
            DEBUG_PRINT(fadeTime);

            fadeTime = map(fadeTime, 0, 255, 0, MAX_FADE);
            DEBUG_PRINT(" to ");
            DEBUG_PRINTLN(fadeTime);

            // calculate cue transition position
            if (bFading) {      // this should be replaced with proper maths
                fadeTimeElapsed = timeNow - lastGo;
                if (fadeTimeElapsed < fadeTime) {
                    step = ((float)fadeTimeElapsed / fadeTime);
                    DEBUG_PRINT("Fade  ");
                    DEBUG_PRINT(step);
                    DEBUG_PRINTLN(" completed ");
                } else {
                    bFading = false;
                }
            }

            // calculate values to pass
            for (int i = 1; i < NUM_FADERS; i++) {
                uint8_t target = cap(lightingData[selectedCue][i], faderValues[0]);
                uint8_t v = target;
                if (bFading) {
                    v = values[i] + ((target - values[i]) * step);
                    DEBUG_PRINT("Channel ");
                    DEBUG_PRINT(i);
                    DEBUG_PRINT(" Currently at ");
                    DEBUG_PRINT(lightingData[selectedCue][i]);
                    DEBUG_PRINT(" fading step: ");
                    DEBUG_PRINT(v);
                    DEBUG_PRINT(" of target ");
                    DEBUG_PRINTLN(target);
                }
                values[i] = largest(v , cap(faderValues[i], faderValues[0] ) );
            } 
            values[0] = faderValues[0]; // master not affected

            // calculate values for indicator LEDs
            led_from_selected_cue();
        break;
    }
}

void called_actions() {

    // record cue called
    if (bRecordCue) {
        DEBUG_PRINTLN("Recording cue..");

        uint16_t address;
        for (int i = 0; i < NUM_FADERS; ++i) {
            // write to running memory
            lightingData[selectedCue][i] = values[i];
            // write data to EEPROM
            address = (selectedCue * NUM_FADERS) + i;
            write_to_eeprom(values[i], address + EEPROM_ADDRESS);
        }
    }

    // reset EEPROM
    if (bClearEEPROM) { 
        DEBUG_PRINTLN("Calling EEPROM clear..");
        clear_eeprom();
    }

    // clear running data called
    if (bClearRunningData) {
        DEBUG_PRINTLN("Clearing running data..");
        memset(lightingData, 0, sizeof(lightingData));  // clear running data
    }

    // clear indicators called
    if (bClearIndicators) {
        // clear indicators from previous mode
        memset(leds, 0, sizeof(leds));                 // all indicator LEDs off
    }
}

void refresh_outputs() {
    /* Refresh LED outptus */
    write_to_leds();                // write outputs
    write_to_indicators();     // write to indicator LEDs
}
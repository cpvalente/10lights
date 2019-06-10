/*
 *  10 lights
 *  -------------
 *  Summer project with Stu, lighting console for 10 LEDs
 *
 *  Code by Carlos Valente
 *  
 *  TODO:
 *  Gen
 *  - debug modes: verbose
 *  Functionality
 *  - the highlight button
 *  INPUT
 *  - ADC prescaler
 */

#define DEBUG
#include "def.h"

/* Gen - State Machine */
enum states {MODE_1, MODE_2, MODE_3};
uint8_t state = MODE_1;
uint32_t timeNow, lastBtnRead;
bool bInitSequence;

/* Gen - Lighting States */
uint8_t lightingData[NUM_CUES][NUM_FADERS];
uint8_t selectedCue, runningCue;
uint32_t fadeTime;
bool bFading;               // initiate fade
bool bRecordCue;            // record fader data in cue
bool bClearEEPROM;          // reset EEPROM data
bool bClearRunningData;     // clear running data
bool bClearIndicators;      // clear indicator LEDs
bool bClearTimer;           // clear timing indicator LEDs
bool bModeChanged;          // mode change flag
bool bCueChanged;           // cue change flag
bool bModeSelect;           // mode select flag
bool bStandby;              // standby mode, no cue

/* Gen - Pin Values Buttons */
bool store, back, go;
bool prevStore, prevBack, prevGo;
uint32_t lastStore, lastBack, lastGo;

/* Gen - Pin Values Faders */
uint8_t faderValues[NUM_FADERS];     // values from faders at each iteration
uint8_t values[NUM_FADERS];          // values for output
UILed   cueLeds[NUM_CUES];           // values for indicator leds
uint8_t timerLeds[NUM_TIMER];        // values for timing indicators

/* Include code files */
#include "io.h"
#include "eeprom.h"


void setup(){
    /* Pin Assignments */
    init_io();
 
     /* LED initialize */
    init_UILed();

    /* Serial */
    Serial.begin(SERIAL_BAUD);

    /* initialize data from EEPROM memory */
    init_from_eeprom(EEPROM_ADDRESS);

    /* Initialize aux */
    selectedCue = runningCue = 0;    // runningCue used in mode 3

    bRecordCue = bClearEEPROM = bClearRunningData = false;
    bClearIndicators = bClearTimer = bModeChanged = bModeSelect = false;
    bInitSequence = bCueChanged = true;

    lastBtnRead = 0;                // button sw debounce
}

void loop(){
    if (bInitSequence) 
        init_sequence(UI_SLOW);     // init sequence on startup

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
    bRecordCue          = false;
    bClearEEPROM        = false;
    bClearRunningData   = false;
    bClearIndicators    = false;
    bClearTimer         = false;
    bModeChanged        = false;
    bCueChanged         = false;
    bInitSequence       = false;
    bModeSelect         = false;

    /* aux */
    DEBUG_PLOT("\n");
}

void init_sequence(int time){
    /* on init all leds flash in order 
     * all pins by hand, set visually  */

    // cue indicators
    uint8_t pinOrder1[NUM_CUES] = {22,27,23,28,24,31,26,30,25,29};
    for (int i = 0; i < NUM_CUES; ++i) {
        blink(pinOrder1[i], time);
    }

    // PWM lights
    uint8_t pinOrder2[NUM_FADERS] = {2,3,4,5,6,7,8,9,10,11,12};
    for (int i = 0; i < NUM_FADERS; ++i) {
        fade(pinOrder2[i], time * 2);
    }

    // mode and timing indicators
    uint8_t pinOrder3[NUM_MODES + NUM_TIMER] = {19,20,21,41,40,39,38,37,36,35,34,33,32};
    for (int i = 0; i < NUM_MODES + NUM_TIMER; ++i) {
        blink(pinOrder3[i], time);
    }
}

uint8_t check_mode(){
    /* reads mode selection input */
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
                bModeSelect = true;

                if (go & !prevGo) {
                    called_mode = (called_mode + 1) % NUM_MODES;
                    bModeChanged = bCueChanged = true;
                }
                if (back & !prevBack) { 
                    called_mode --;
                    bModeChanged = bCueChanged = true;
                }
                if (called_mode >= NUM_MODES)   called_mode = NUM_MODES - 1;
                
                return called_mode;
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
                    // and reset flags
                    back = go = false;
                }
            }
        }
    } else {
        // go and go backwards only work on mode 2 and 3

        // go backwards
        if (back && !prevBack) { 
            if (!bStandby) {
                selectedCue -= 1;
                if (selectedCue >= NUM_CUES) {
                    selectedCue = NUM_CUES - 1;
                }
            }
            if (called_mode == MODE_3) bStartFade = true;
            bCueChanged = true;
        }

        // go forwards
        if (go && !prevGo) {
            if (!bStandby) {
                selectedCue = (selectedCue + 1) % NUM_CUES;
            }
            if (called_mode == MODE_3) bStartFade = true;
            bCueChanged = true;
        }

        if (bStartFade) {
            lastGo = timeNow;
            bFading = true;
            bStandby = false;
        }
    }
    return called_mode;
}

void loop_execute(uint8_t called_mode){

    if (called_mode < 3) state = called_mode;

    DEBUG_PRINT("Mode: ");
    DEBUG_PRINTLN(state + 1);

    switch (state) {

        case MODE_1:        // Fader is value
            // call clear indicator LEDs
            if (bModeChanged) bClearIndicators = true;

            // calculate values to pass
            for (int i = 1; i < NUM_FADERS; ++i) {
                values[i] = cap(faderValues[i], faderValues[0]);
            }
            values[0] = faderValues[0]; // master not affected

            // calculate values for time indicator LEDs
            leds_from_value(values[0]);

        break;

        case MODE_2:        // Record
            // mode change resets cue select
            if (bModeChanged) selectedCue = 0;
            runningCue = selectedCue;       // no fading in mode 2

            // calculate values for leds
            for (int i = 1; i < NUM_FADERS; ++i) {
                values[i] = faderValues[i];
            }
            values[0] = faderValues[0]; // master not affected

            // calculate values for time indicator LEDs
            leds_from_value(values[0]);

        break;

        case MODE_3:        // Playback

            float step = 1.0f;

            // mode initializes from empty
            if (bModeChanged) {
                // need to have an extra uint for running cue
                memset(values, 0, sizeof(values));  // clear running values at start
                bClearIndicators = bCueChanged = true;
                selectedCue = runningCue = 0;
                bFading  = false;
                bStandby = true;
            }

            // calculate cue transition position
            if (bFading) {      // this should be replaced with proper maths
                uint32_t fadeTimeElapsed;

                fadeTime = lightingData[selectedCue][0];
                DEBUG_PRINT("Cue fade time ");
                DEBUG_PRINT(fadeTime);

                fadeTime = map(fadeTime, 0, 255, 0, MAX_FADE);
                DEBUG_PRINT(" to ");
                DEBUG_PRINTLN(fadeTime);

                fadeTimeElapsed = timeNow - lastGo;

                if (fadeTimeElapsed < fadeTime) {
                    leds_from_time_delta(fadeTimeElapsed, fadeTime);
                    step = ((float)fadeTimeElapsed / fadeTime);
                    DEBUG_PRINT("Fade ");
                    DEBUG_PRINT(step);
                    DEBUG_PRINTLN(" completed ");
                } else {
                    bFading = false;
                    bCueChanged = true;
                    runningCue = selectedCue;
                }
            }

            // calculate values to pass
            for (int i = 1; i < NUM_FADERS; ++i) {
                if (bStandby) {
                    values[i] = cap(faderValues[i], faderValues[0]);
                } else {
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
            } 
            values[0] = faderValues[0]; // master not affected
        break;
    }

    if (bStandby) {
        DEBUG_PRINTLN("Standing by for cue");
    } else {
        DEBUG_PRINT("Selected Cue: ");
        DEBUG_PRINTLN(selectedCue + 1);

        DEBUG_PRINT("Running Cue: ");
        DEBUG_PRINTLN(runningCue + 1);
    }
}

void called_actions() {

    /* record cue called */
    if (bRecordCue) {
        DEBUG_PRINTLN("Recording cue..");

        uint16_t address = EEPROM_ADDRESS + (selectedCue * NUM_FADERS) ;
        for (int i = 0; i < NUM_FADERS; ++i) {
            // write to running memory
            lightingData[selectedCue][i] = values[i];
            // write data to EEPROM
            write_to_eeprom(values[i], address);
            address++;
        }
        bCueChanged = true;
    }

    /* reset EEPROM */
    if (bClearEEPROM) { 
        DEBUG_PRINTLN("Calling EEPROM clear..");
        clear_eeprom();
        // give visual feedback
        init_sequence(UI_BLINK);
    }

    /* clear running data called */
    if (bClearRunningData) {
        DEBUG_PRINTLN("Clearing running data..");
        memset(lightingData, 0, sizeof(lightingData));  // clear running data
    }

    /* clear indicators called */
    if (bClearIndicators) {
        // clear indicators from previous mode
        for (int i = 0; i < NUM_CUES; ++i) {
            cueLeds[i].setMode(0);                      // all cue LEDs off
        }
    }
    if (bClearTimer) {
        // clear indicators from previous mode
        memset(timerLeds, 0, sizeof(timerLeds));        // all timer LEDS off
    }
    /* UI management */
    if (bCueChanged) {
        // calculate values for indicator LEDs
        if (state == MODE_2) {
            bool bEmpty = true;
            for (int i = 0; i < NUM_FADERS; ++i) {
                if (lightingData[selectedCue][i] != 0) bEmpty = false;
            }
            DEBUG_PRINT("Mode 2: Cue ");
            DEBUG_PRINT(selectedCue);
            if (bEmpty) DEBUG_PRINT(" Empty");
            led_from_selected_cue(selectedCue, bEmpty);
        } else if (state == MODE_3) {
            if (bStandby) {
                led_running(runningCue);
            } else {
                led_up_to_cue(selectedCue);
                if (bFading) led_running(selectedCue);
            }
        }
    }
}

void refresh_outputs() {
    /* Refresh LED outputs */
    write_to_leds();                        // write outputs
    write_to_indicators();                  // write to indicator LEDs
}

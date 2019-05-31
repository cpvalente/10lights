/*
 *  10 lights
 *  -------------
 *  Summer project with Stu, lighting console for 10 LEDs
 *
 *  Code by Carlos Valente
 *  
 *  TODO:
 *  Gen
 *  - Modes pass values as reference
 *  - Toggle verbose
 *  - Three button surprise
 *  Functionality
 *  - Fade time from cue
 *  - overflow on fade?
 *  - read / write to EEPROM
 *  Pin definitions
 *  - pins as port 
 *  LEDs
 *  - duty cycle for cue leds 
 *  -- https://www.reddit.com/r/arduino/comments/5pycxp/adjusting_frequency_and_duty_cycle_of_led_blink/
 *  - indicator LEDs to flash according to mode select 
 */

#include "def.h"

/* Gen - State Machine */
enum states {MODE_1, MODE_2, MODE_3};
uint8_t state = MODE_1;

/* Gen - Lighting States */
uint8_t lightingData[NUM_CUES][NUM_LIGHTS + 1] = {0};   // numLights + cue fade time
uint8_t selectedCue;
uint32_t fadeTime = 3000;                               // temporary
bool bFading = false;

/* Gen - Pin Assignments */
const uint8_t analogInputs[]      {A10, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9}; // Faders, first is master
const uint8_t digitalOutputsPWM[] {2,   12, 11, 10,  9,  8,  7 , 6,  5,  4,  3}; // channel indicator LED, first is master
const uint8_t digitalOutputs[]    {22, 27, 23, 28, 24, 29, 25, 30, 26, 31};      // cue indicator LED
const uint8_t digitalInputs[]     {13, 52, 53};                                  // store, back, go

/* Gen - Pin Values Buttons */
bool store, back, go;
bool prevStore, prevBack, prevGo;
uint32_t lastStore, lastBack, lastGo, timeNow;

/* Gen - Pin Values Faders */
uint8_t faderValues[NUM_FADERS];     // values from faders at each iteration
uint8_t values[NUM_FADERS];          // values for output
uint8_t leds[NUM_CUES];              // values for indicator leds

void setup(){

    /* Pin Assignments
     * Pin Name is MEGA name, the long way
     */
    
    int arraySize;

    // Pin Assignments - Analog Pins
    arraySize = sizeof(analogInputs) / sizeof(analogInputs[0]);
    for (int i = 0; i < arraySize; i++) {
        pinMode(analogInputs[i], INPUT);
    }

    // Pin Assignments - Digital Pins INPUT
    arraySize = sizeof(digitalInputs) / sizeof(digitalInputs[0]);
    for (int i = 0; i < arraySize; i++) {
        pinMode(digitalInputs[i], INPUT_PULLUP);
    }

    // Pin Assignments - Digital Pins PWM OUTPUT
    arraySize = sizeof(digitalOutputsPWM) / sizeof(digitalOutputsPWM[0]);
    for (int i = 0; i < arraySize; i++) {
        pinMode(digitalOutputsPWM[i], OUTPUT);
    }

    // Pin Assignments - Digital Pins OUTPUT
    arraySize = sizeof(digitalOutputs) / sizeof(digitalOutputs[0]);
    for (int i = 0; i < arraySize; i++) {
        pinMode(digitalOutputs[i], OUTPUT);
    }

    // Serial setup
    Serial.begin(115200);

    Serial.print("Initializing array... ");
    // will be replaced with EEPROM read
    Serial.println("finished");

    /* Initialize aux */
    selectedCue = 0;
}

void loop(){
    timeNow = millis();             // get iteration time
    read_inputs();                  // this should only happen if needed
    loop_execute( check_mode() );   // call state machine
    write_to_leds();                // write outputs
    write_to_indicators();          // write to UI LEDs
    delay(20);                      // !! replace with refresh rate
}

void read_inputs(){
    // reads declared inputs

    Serial.println("Reading analog inputs...");

    // Read Input - Analog Pins
    for (int i = 0; i < NUM_FADERS; i++) {
        faderValues[i] = analogRead8(analogInputs[i]);
        Serial.print(i);
        Serial.print(": ");
        Serial.println(faderValues[i]);
    }

    Serial.println("Reading digital inputs...");

    // Read Input - Digital Pins INPUT
    store = digitalRead(digitalInputs[0]);
    back  = digitalRead(digitalInputs[1]);
    go    = digitalRead(digitalInputs[2]);

    Serial.print("Store: "); Serial.println(store);
    Serial.print("Back: ");  Serial.println(back);
    Serial.print("Go: ");    Serial.println(go);
}

void write_to_leds(){
    // writes values to PWM digital
    Serial.println("Writing to PWM outputs inputs...");
    for (int i = 0; i < NUM_FADERS; i++) {
        analogWrite(digitalOutputsPWM[i], values[i]);
        Serial.print(i);
        Serial.print(": ");
        Serial.println(values[i]);
    }
}

void write_to_indicators(){
    // write to lighting cue indicator LEDs
    for (int i = 0; i < NUM_CUES; i++) {
        digitalWrite(digitalOutputs[i], leds[i]);
    }
}

uint8_t check_mode(){
    // reads mode selection input
    uint8_t called_mode = state;
    bool bStartFade = false;

    // store changes between modes, check everytime
    if (store) {
        if (prevStore) {
            if (timeNow - lastStore > STORE_TIME) {
                if (state == MODE_1) called_mode = MODE_2;
                else if (state == MODE_2) called_mode = MODE_3;
                else {
                    called_mode = MODE_1;
                    leds[NUM_CUES] = {0};   // reset cue LEDs
                }
                prevStore = false;
            }
        } else {
            prevStore = true;
            lastStore = timeNow;
        }
    } else {
        if (prevStore) {
            for (int i = 1; i < NUM_LIGHTS + 1; i++) {
                lightingData[selectedCue][i] = values[i];
            }
        }
        prevStore = false;
    } 

    // go and go backwards only work on mode 2 and 3
    if (called_mode != MODE_1) {
        // go backwards
        if (back && !prevBack) { 
            selectedCue -= 1;
            if (selectedCue >= NUM_CUES) {
                selectedCue = NUM_CUES - 1;
            }
            prevBack = true;
            bStartFade = true;
        }
        if (!back) prevBack = false;

        // go forwards
        if (go && !prevGo) {
            selectedCue = (selectedCue + 1) % NUM_CUES;
            prevGo = true;
            bStartFade = true;
        }
        if (!go) prevGo = false;

        if (bStartFade) {
            lastGo = timeNow;
            bFading = true;
        }
    }

    Serial.print("Selected Cue: ");
    Serial.println(selectedCue);

    return called_mode;
}

void loop_execute(uint8_t called_mode){

    if (called_mode < 3) state = called_mode;

    switch (state) {

        case MODE_1:
            // Fader is value
            Serial.println("Mode 1");

            // calculate values to pass
            for (int i = 1; i < NUM_FADERS; i++) {
                values[i] = cap(faderValues[i], faderValues[0]);
            }
            values[0] = faderValues[0]; // master not affected
        break;

        case MODE_2:
            // Record
            Serial.println("Mode 2");

            // calculate values for leds
            for (int i = 1; i < NUM_FADERS; i++) {
                values[i] = cap(faderValues[i], faderValues[0]);
            }
            values[0] = faderValues[0]; // master not affected

            // calculate values for indicator LEDs
            for (int i = 0; i < NUM_CUES; i++) {
                leds[i] = (selectedCue == i);
             }
        break;

        case MODE_3:
            // Playback
            Serial.println("Mode 3");
            uint32_t fadeTimeElapsed;
            float step = 1.0f;

            // calculate cue transition position
            if (bFading) {
                fadeTimeElapsed = timeNow - lastGo;
                if (fadeTimeElapsed < fadeTime) {
                    step = ((float)fadeTimeElapsed / fadeTime);
                    Serial.print("Fade time ");
                    Serial.print(fadeTimeElapsed);
                    Serial.print(" completed ");
                    Serial.println(step);
                } else {
                    bFading = false;
                }
            }

            // calculate values to pass
            for (int i = 1; i < NUM_FADERS; i++) {
                uint8_t v = 0;
                if (bFading) {
                    Serial.print("Channel ");
                    Serial.print(i);
                    Serial.print(" fading, ");
                    Serial.print(v);
                    Serial.print(" of ");
                    Serial.println((lightingData[selectedCue][i]));   
                    v = values[i] + ((lightingData[selectedCue][i] - values[i]) * step);
                } else {
                    v = lightingData[selectedCue][i];
                }
                values[i] = cap( largest(v, faderValues[i]) , faderValues[0] );
            }

            values[0] = faderValues[0]; // master not affected

            // calculate values for indicator LEDs
            for (int i = 0; i < NUM_CUES; i++) {
                leds[i] = (selectedCue == i);
             }
        break;
    }
}
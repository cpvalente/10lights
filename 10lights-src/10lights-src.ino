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
 *  Pin definitions
 *  - pins as port 
 *  LEDs
 *  - duty cycle for cue leds 
 *  -- https://www.reddit.com/r/arduino/comments/5pycxp/adjusting_frequency_and_duty_cycle_of_led_blink/
 */
#include "def.h"

/* Gen - State Machine */
enum states {MODE_1, MODE_2, MODE_3};
uint8_t state = MODE_1;

/* Gen - Lighting States */
uint8_t currentCue[NUM_FADERS];   // numLights + cue fade time
uint8_t previousCue[NUM_FADERS];
uint8_t nextCue[NUM_FADERS];
uint8_t currentLighting[NUM_LIGHTS];
uint8_t selectedCue;

/* Gen - Pin Assignments */
const uint8_t analogInputs[]      {A10, A8, A6, A5, A4, A3, A2, A1, A0, A7, A9}; // Faders, first is master
const uint8_t digitalOutputsPWM[] {2,   12, 11, 10,  9,  8,  7 , 6,  5,  4,  3}; // channel indicator LED, first is master
const uint8_t digitalOutputs[]    {22, 27, 23, 28, 24, 29, 25, 30, 26, 31};      // cue indicator LED last
const uint8_t digitalInputs[]     {40, 52, 53};                                  // store, back, go

/* Gen - Pin Values Buttons */
byte store, back, go;
byte prevStore, prevBack, prevGo;
long lastStore, lastBack, lastGo, timeNow;

/* Gen - Pin Values Faders */
uint8_t faderValues[NUM_FADERS];
uint8_t values[NUM_FADERS];
byte    leds[NUM_CUES];


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

}

void loop(){
    timeNow = millis();             // get iteration time
    read_inputs();                  // this should only happen if needed
    loop_execute( check_mode() );   // call state machine
    write_to_leds();                // write outputs
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
    for (int i = 0; i < NUM_LIGHTS; i++) {
        analogWrite(digitalOutputsPWM[i], values[i]);
        Serial.print(i);
        Serial.print(": ");
        Serial.println(values[i]);
    }

    for (int i = 0; i < NUM_CUES; i++) {
        // replace by duty cycle 
        digitalWrite(digitalOutputs[i], leds[i]);
    }
}

uint8_t check_mode(){
    // reads mode selection input
    uint8_t called_mode = 11;

    if (store) {
        if (prevStore) {
            if (timeNow - lastStore > STORE_TIME) {
                if (state == MODE_1) called_mode = MODE_2;
                else called_mode = MODE_3;
            }
        } else {
            prevStore = true;
            lastStore = timeNow;
        }
    } else  prevStore = false;

    // go backwards
    if (back && !prevBack) { 
        selectedCue -= 1;
        if (selectedCue >= 10) {
            selectedCue = 9;
        }
        prevBack = true;
    }
    if (!back) prevBack = false;

    // go forwards
    if (go && !prevGo) {
        selectedCue = (selectedCue + 1) % NUM_CUES;
        prevGo = true;
    }
    if (!go) prevGo = false;

    Serial.print("Selected Cue: ");
    Serial.println(selectedCue);
    Serial.print("Selected Mode: ");
    Serial.println(called_mode);

    return called_mode;
}

void loop_execute(uint8_t called_mode){

    if (called_mode < 3) state = called_mode;

    switch (state) {

        case MODE_1:
            // Fader is value
            Serial.println("Mode 1");

            // calculate values to pass
            for (int i = 1; i < NUM_LIGHTS; i++) {
                values[i] = cap(faderValues[i], faderValues[0]);
            }
            values[0] = faderValues[0]; // master not affected

            for (int i = 0; i < NUM_CUES; i++) {
                leds[i] = (selectedCue == i);
            }

        break;

        case MODE_2:
            // Record
            Serial.println("Mode 2");
        break;

        case MODE_3:
            // Playback
            Serial.println("Mode 3");
        break;
    }
}

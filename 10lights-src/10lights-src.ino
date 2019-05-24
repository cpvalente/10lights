/*
 *  10 lights
 *  -------------
 *  Summer project with Stu, lighting console for 10 LEDs
 *
 *  Code by Carlos Valente
 *  
 *  TODO:
 *  Pin definitions
 *  - define pin names from table 
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

/* Gen - Pin Assignments */
uint8_t analogInputs[]      {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10}; // Faders, last is master
uint8_t digitalInputs[]     {40, 52, 53};                                  // store, back, go
uint8_t digitalOutputsPWM[] {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};          // channel indicator LED, first is master
uint8_t digitalOutputs[]    {22, 23, 24, 25, 26, 27, 28, 29, 30, 31};      // cue indicator LED, 

/* Gen - Pin Values */
uint8_t store, back, go;
uint8_t faderValues[NUM_FADERS];

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
        pinMode(digitalInputs[i], INPUT);
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
    read_inputs();                  // this should only happen if needed
    loop_execute( check_mode() );

}

void read_inputs(){
    // reads declared inputs

    // Read Input - Analog Pins
    for (int i = 0; i < NUM_FADERS; i++) {
        faderValues[i] = max(1, analogRead(i) >> 2);
    }

    // Read Input - Digital Pins INPUT
    store = digitalRead(digitalInputs[0]);
    back  = digitalRead(digitalInputs[1]);
    go    = digitalRead(digitalInputs[2]);

}
uint8_t check_mode(){
    // reads mode selection input
    return 0;
}

void loop_execute(uint8_t called_mode){
    switch (state) {

        case MODE_1:
            // Fader is value
            Serial.println("Mode 1");
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

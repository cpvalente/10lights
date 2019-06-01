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
 *  - overflow on fade?
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
uint8_t lightingData[NUM_CUES][NUM_FADERS];
uint8_t selectedCue;
uint32_t fadeTime;
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
     * by PORT
     */
    
    // Pin Assignments - Analog Pins
    DDRF = DDRF | 0b00000000;
    DDRK = DDRK | 0b00000000;

    // Pin Assignments - Digital Pins INPUT
    DDRB = DDRB | 0b10000011;

    // Pin Assignments - Digital Pins PWM OUTPUT
    DDRB = DDRB | 0b10000011; 
    DRDE = DDRE | 0b00011100;
    DRDG = DDRG | 0b10010000;
    DRDH = DDRH | 0b00111100;

    // Pin Assignments - Digital Pins OUTPUT
    DDRA = DDRA | 0b11111111;   // CUE LEDs 1, 3, 5, 7, 9, 2, 4, 6
    DDRC = DDRA | 0b11111111;   // LED BAR 5,6,7,8,9,10 - CUE LEDs 10, 8
    DDRD = DDRD | 0b01000000;   // LED BAR 4
    DDRG = DDRG | 0b00000111;   // LED BAR 1,2,3

    // Serial setup
    Serial.begin(115200);

    // initialize data from EEPROM memory
    Serial.print("Initializing array... ");

    uint16_t address = 0;
    for (uint8_t i = 0; i < NUM_CUES; ++i) {
        for (uint8_t j = 0; j < NUM_FADERS; ++j) {
            lightingData[i][j] = read_from_EEPROM(address + EEPROM_ADDRESS);
            address++;
        }
    }
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
    store = PINB & STORE_PIN;
    back  = PINB & BACK_PIN;
    go    = PINB & GO_PIN;

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
        digitalWriteFast(digitalOutputs[i], leds[i]);
    }
}

inline uint8_t read_from_EEPROM(uint16_t address){
    // returns byte from EEPROM at address
    return EEPROM.read(address);
}

inline void write_to_EEPROM(uint8_t b, uint16_t address){
    // write single byte to EEMPROM at address
    EEPROM.update(address, b);  
}

bool clear_EPPROM(){
    for (int i = 0 ; i < EEPROM.length() ; ++i) {
        EEPROM.update(i, 0);
    }
    Serial.println("EEPROM erased");
    return true;
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
                    memset(leds, 0, sizeof(NUM_CUES));      // all indicator LEDs off
                    // clear ram
                    for (uint8_t i = 0; i < NUM_CUES; ++i) {
                        for (uint8_t j = 0; j < NUM_FADERS; ++j) {
                            lightingData[i][j] = 0;
                        }
                    }
                }
                prevStore = false;
            }
        } else {
            prevStore = true;
            lastStore = timeNow;
        }
    } else {
        if (prevStore) {
            // record cue
            uint16_t address;
            for (int i = 0; i < NUM_LIGHTS + 1; i++) {
                address = (selectedCue * NUM_FADERS) + i;
                lightingData[selectedCue][i] = values[i];
                // write data to EEPROM
                write_to_EEPROM(values[i], address + EEPROM_ADDRESS);
            }
        }
        prevStore = false;
    } 

    if (called_mode == MODE_1) {
        // delete only in mode 1
        if (back && go) { 
            if (!prevGo && !prevBack) {
                lastGo = lastBack = timeNow;
            } else {
                if ((timeNow - lastGo > STORE_TIME) && (timeNow - lastBack > STORE_TIME)) {
                    clear_EPPROM();
                    // memset(lightingData, 0, (NUM_LIGHTS * NUM_CUES) * (sizeof *lightingData));
                    for (uint8_t i = 0; i < NUM_CUES; ++i) {
                        for (uint8_t j = 0; j < NUM_FADERS; ++j) {
                            lightingData[i][j] = 0;
                        }
                    }
                    prevBack = prevGo = false;
                }
                Serial.println("Calling EEPROM clear");
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
                values[i] = faderValues[i];
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
            fadeTime = lightingData[selectedCue][0];
            fadeTime = map(fadeTime, 0, 255, 0, 10000);
            Serial.print("cue fade time ");
            Serial.print(lightingData[selectedCue][0]);
            Serial.print(" to ");
            Serial.println(fadeTime);

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
                uint8_t target = cap(lightingData[selectedCue][i], faderValues[0]);
                if (bFading) {
                    v = values[i] + ((target - values[i]) * step);
                    Serial.print("Channel ");
                    Serial.print(i);
                    Serial.print(" Currently at ");
                    Serial.print(lightingData[selectedCue][i]);
                    Serial.print(" fading step: ");
                    Serial.print(v);
                    Serial.print(" of target ");
                    Serial.println((lightingData[selectedCue][i]));
                } else {
                    v = target;
                }
                values[i] = largest(v , cap(faderValues[i], faderValues[0] ) );
            } 
            values[0] = faderValues[0]; // master not affected

            // calculate values for indicator LEDs
            for (int i = 0; i < NUM_CUES; i++) {
                leds[i] = (selectedCue == i);
             }
        break;
    }
}

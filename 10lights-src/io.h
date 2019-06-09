void init_io() {
    // Pin Assignments - Analog Pins
    DDRF = DDRF | 0b00000000;   // Fader CH1->CH8
    DDRK = DDRK | 0b00000000;   // Fader CH9, CH10, MASTER

    // Pin Assignments - Digital Pins INPUT
    DDRB = DDRB | 0b10001111;   // STORE button, Highlight buttons 6,5, BACK button, GO button
    DDRL = DDRL | 0b11111111;   // Highlight buttons 10,1,9,2,8,3,7,4

    // Pin Assignments - Digital Pins PWM OUTPUT
    DDRB = DDRB | 0b01110000;   // PWM Out CH 3, 2, 1 
    DDRE = DDRE | 0b00111000;   // PWM Out CH 8, MASTER, 10
    DDRG = DDRG | 0b00100000;   // PWM Out CH 9
    DDRH = DDRH | 0b01111000;   // PWM Out CH 4, 5, 6, 7

    // Pin Assignments - Digital Pins OUTPUT
    DDRA = DDRA | 0b11111111;   // CUE LEDs 1, 3, 5, 7, 9, 2, 4, 6
    DDRC = DDRA | 0b11111111;   // LED BAR 5,6,7,8,9,10, CUE LEDs 10, 8
    DDRD = DDRD | 0b10000111;   // LED BAR 4 , MODE 1 LED, MODE 2 LED, MODE 3 LED
    DDRG = DDRG | 0b00000111;   // LED BAR 1,2,3

}

void read_inputs(){
    // reads declared inputs

    DEBUG_PRINTLN("Reading analog inputs...");

    /* Read Input - Analog Pins */
    for (int i = 0; i < NUM_FADERS; ++i) {
        faderValues[i] = ema(values[i], analogRead8(analogInputs[i]), VERY_HIGH_PASS);
        DEBUG_PRINT(i);
        DEBUG_PRINT(": ");
        DEBUG_PRINTLN(faderValues[i]);

        DEBUG_PLOT(faderValues[i]);
        DEBUG_PLOT("\t");
    }  

    DEBUG_PRINTLN("Reading digital inputs...");

    if (timeNow - lastBtnRead > DEBOUNCE_TIME) {
        /* Read Input - Digital Pins INPUT */
        store = PINB & STORE_PIN;
        back  = PINB & BACK_PIN;
        go    = PINB & GO_PIN;

        DEBUG_PRINT("Store: "); DEBUG_PRINTLN(store);
        DEBUG_PRINT("Back: ");  DEBUG_PRINTLN(back);
        DEBUG_PRINT("Go: ");    DEBUG_PRINTLN(go);
        
        lastBtnRead = timeNow;  // reset timer
    }
}

void write_to_leds(){
    // writes values to PWM digital
    DEBUG_PRINTLN("Writing to PWM outputs inputs...");
    for (int i = 0; i < NUM_FADERS; ++i) {
        analogWrite(digitalOutputsPWM[i], values[i]);
        DEBUG_PRINT(i);
        DEBUG_PRINT(": ");
        DEBUG_PRINTLN(values[i]);

        DEBUG_PLOT(values[i]);
        DEBUG_PLOT("\t");
    }
}

inline void write_to_indicators(){
    // write to lighting cue indicator LEDs
    for (int i = 0; i < NUM_CUES; ++i) {
        digitalWriteFast(digitalOutputs[i], leds[i]);
    }
    // write to mode indicator LEDs
    for (int i = 0; i < NUM_MODES; ++i) {
        digitalWriteFast(mode_select[i], state == i);
    }
    // write to time indicator LEDs
    for (int i = 0; i < NUM_TIMER; ++i) {
        digitalWriteFast(time_indicator[i], timerLeds[i]);
    }
}

inline void flash_indicators() {
    // flash indicator LEDs
    memset(leds, 255, sizeof(leds));
}

inline void blink(uint8_t led, int time) {
    // flash led for time
    digitalWriteFast(led, HIGH);
    delay(time);
    digitalWriteFast(led, LOW);
}

inline void fade(uint8_t led, int time) {
    // fade led in and out in time
    for (int i = 0; i < 255; ++i) {
        analogWrite(led, i);
        delay (time / 255);
    }
    for (int i = 255; i > 0; --i) {
        analogWrite(led, i);
        delay (time / 255);
    }
}

inline void led_from_selected_cue(uint8_t selected){
    for (int i = 0; i < NUM_CUES; ++i) {
        (i <= selected) ? (leds[i] = 255) : (timerLeds[i] = 0);   
    }
}

inline void leds_from_time_delta(uint32_t time, uint32_t totalTime){
    uint8_t led_on = map(time, 0, totalTime, 0, 10);
    for (int i = 0; i < NUM_TIMER; ++i) {
        (i <= led_on) ? (timerLeds[i] = 255) : (timerLeds[i] = 0);   
    }
}

inline void leds_from_time(uint32_t time){
    uint8_t led_on = map(time, 0, MAX_FADE, 0, 10);
    for (int i = 0; i < NUM_TIMER; ++i) {
        (i <= led_on) ? (timerLeds[i] = 255) : (timerLeds[i] = 0);   
    }
}

inline void leds_from_value(uint8_t time){
    uint8_t led_on = map(time, 0, 255, 0, 10);
    for (int i = 0; i < NUM_TIMER; ++i) {
        (i <= led_on) ? (timerLeds[i] = 255) : (timerLeds[i] = 0);   
    }
}
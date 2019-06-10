class UILed {

    /* Operation */
    enum modes {CONSTANT_OFF, CONSTANT_ON, BLINK};
    uint8_t mode, state;

    /* IO */
    uint8_t pin;

    /* Timings */
    uint32_t BLINK_TIME = 160;
    uint32_t prevTime;

    public:
    UILed(){
        pin = 0;
        state = LOW;
    }

    void init(uint8_t pin_, uint8_t mode_ = CONSTANT_OFF){
        pin = pin_;
        setMode(mode_);
        prevTime = 0;
    }

    void update(uint32_t *timeNow) {
        if (mode == BLINK ) {
            if (*timeNow - prevTime > BLINK_TIME) {
                state = !state;
                prevTime = *timeNow;
            }
        }
        digitalWriteFast(pin, state)
    }

    void setMode(uint8_t mode_) {
        // 0 = constant off - 1 = constant on - 2 = blink
        mode = mode_;
        if (mode_ == 0) { state = LOW; }
        else            { state = HIGH; }
    }
};
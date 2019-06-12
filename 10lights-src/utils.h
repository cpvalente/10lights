inline uint8_t analogRead8(uint8_t pin){
    return (analogReadFast(pin) >> 2);
}

inline uint8_t cap(uint8_t value, uint8_t master){
    return (value / 255.0 * master);
}

inline uint8_t largest(uint8_t v1, uint8_t v2){
    if (v1 >= v2) return v1;
    else return v2;
}

inline uint8_t ema(uint8_t value, uint8_t new_value, float filter){
    uint8_t v = (value * (1.0 - filter)) + (new_value * filter);
    return v;
}
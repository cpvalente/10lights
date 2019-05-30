inline uint8_t analogRead8(uint8_t pin){
    return (analogRead(pin) >> 2);
}

inline uint8_t cap(uint8_t value, uint8_t master){
    return (value / 255.0 * master);
}

inline uint8_t largest(uint8_t v1, uint8_t v2){
    if (v1 > v2) return v1;
    else return v2;
}
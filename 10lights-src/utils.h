inline uint8_t analogRead8(uint8_t pin){
    return max(1, analogRead(pin) >> 2);
}

inline uint8_t cap(uint8_t value, uint8_t master){
    return (value * 255 / master);
}
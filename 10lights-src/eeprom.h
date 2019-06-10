inline uint8_t read_from_eeprom(uint16_t address){
    /* returns byte from EEPROM at address */
    return EEPROM.read(address);
}

void init_from_eeprom(uint16_t *address) {
    /* initializes lighting data with data from eeprom */
    DEBUG_PRINT("Initializing array... ");
    for (uint8_t i = 0; i < NUM_CUES; ++i) {
        for (uint8_t j = 0; j < NUM_FADERS; ++j) {
            lightingData[i][j] = read_from_eeprom(*address + EEPROM_ADDRESS);
            *address++;
        }
    }
    DEBUG_PRINTLN("finished");
}

inline void write_to_eeprom(uint8_t b, uint16_t address){
    /* write single byte to EEPROM at address */
    EEPROM.update(address, b);  
}

void clear_eeprom(){
    /* clear entire eeprom */
    for (int i = 0 ; i < EEPROM.length(); ++i) {
        write_to_eeprom(i, 0);
    }
    DEBUG_PRINTLN("EEPROM erased");
}

void clear_eeprom_addresses(uint16_t *startAdd, uint16_t *endAdd){
    /* clear eeprom from given addresses */
    for (int i = *startAdd ; i < *endAdd; ++i) {
        write_to_eeprom(i, 0);
    }
    DEBUG_PRINT("EEPROM erased in given address");
}

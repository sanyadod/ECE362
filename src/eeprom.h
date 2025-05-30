#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>
void init_eeprom();
void write_high_score(int score);


void enable_ports_eeprom(void);
void eeprom_write(uint16_t loc, const char* data, uint8_t len);
void eeprom_read(uint16_t loc, char data[], uint8_t len);
void init_i2c_eeprom(void);
void i2c_start_prom(uint8_t targadr, uint8_t size, uint8_t dir);
void i2c_stop_prom(void);
void i2c_waitidle_prom(void);
int i2c_senddata_prom(uint8_t addr, uint8_t *data, uint8_t size);
int i2c_recvdata_prom(uint8_t addr, uint8_t *data, uint8_t size);
void i2c_clearnack_prom(void);
int i2c_checknack_prom(void);
void save_high_score(uint8_t score);
uint8_t read_high_score(void);

#endif

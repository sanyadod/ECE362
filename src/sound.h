#ifndef SOUND_H
#define SOUND_H
#include <stdint.h>
#define SOUND_HIT 1
#define SOUND_GAME_OVER 2

void dac_init(void);
void init_buzzer();
void dac_output(uint16_t val);
void delay_ms(uint32_t ms);
void buzz_for_sound(int delay);
void buzz_with_dac(int step_delay, int buzz_time_ms);
#endif

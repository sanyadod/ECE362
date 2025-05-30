#include "gesture.h"
#include "display_setup.h"
#include "display_game_function.h"
#include "sound.h"
#include "eeprom.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <apds9960.h>
void internal_clock();



int main(void) {
    printf("left");
    internal_clock();
    display_main();
    play_game();

    /*enable_ports_gesture();
    init_i2c_gesture();
    delay_ms(700);
    apds9960init();           // Init the sensor
    enableGestureSensor(0);   // Enable gesture mode
    printf("left");
    while (1) {
        char gesture = detect_left_or_right_gesture();
        if (gesture == DIR_LEFT) {
            printf("left");
        } else if (gesture == DIR_RIGHT) {
            printf("right");
        }
    }*/
    delay_ms(200);
    return 0;
}

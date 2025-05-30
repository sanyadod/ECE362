#include "display_setup.h"
#include "display_game_function.h"
#include "lcd.h"
#include "stm32f0xx.h"
#include <string.h>
#include <apds9960.h>
#include <stdio.h>
#include "sound.h"
#include "eeprom.h"
#include "gesture.h"

int high_score = 0;  // EEPROM-loaded high score

int time_left = 60;  // 60 seconds for 1 minute

int score = 0;  // Global score variable

// Paddle dimensions and starting position
#define PADDLE_WIDTH   40
#define PADDLE_HEIGHT  6
int paddle_x = 100;
int paddle_y = 290;

// Ball dimensions
#define BALL_RADIUS 4
int ball_x = 120;
int ball_y = 280;
int ball_moving = 0; // 0 = resting on paddle, 1 = moving up

// Brick setup
#define BRICK_ROWS 3
#define BRICK_COLS 6
#define BRICK_WIDTH 30
#define BRICK_HEIGHT 10
#define BRICK_START_X 20
#define BRICK_START_Y 20
#define BRICK_GAP 5

int brick_exists[BRICK_ROWS][BRICK_COLS]; // 1 = present, 0 = broken

void draw_background(void) {
    LCD_Clear(BLACK);
}

void draw_paddle(void) {
    LCD_DrawFillRectangle(
        paddle_x,
        paddle_y,
        paddle_x + PADDLE_WIDTH,
        paddle_y + PADDLE_HEIGHT,
        WHITE
    );
}

void draw_timer(void) {
    char timer_text[20];
    sprintf(timer_text, "Time: %d", time_left);

    // Clear old timer area
    LCD_DrawFillRectangle(LCD_W - 80, LCD_H - 20, LCD_W - 10, LCD_H, BLACK);

    LCD_DrawString(LCD_W - 70, LCD_H - 18, WHITE, BLACK, timer_text, 16, 0);
}


void erase_paddle(void) {
    LCD_DrawFillRectangle(
        paddle_x,
        paddle_y,
        paddle_x + PADDLE_WIDTH,
        paddle_y + PADDLE_HEIGHT,
        BLACK
    );
}

void draw_bricks(void) {
    u16 row_colors[3] = { RED, GREEN, BLUE };

    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int col = 0; col < BRICK_COLS; col++) {
            if (brick_exists[row][col]) {
                int x1 = BRICK_START_X + col * (BRICK_WIDTH + BRICK_GAP);
                int y1 = BRICK_START_Y + row * (BRICK_HEIGHT + BRICK_GAP);
                LCD_DrawFillRectangle(x1, y1, x1 + BRICK_WIDTH, y1 + BRICK_HEIGHT, row_colors[row]);
            }
        }
    }
}

void erase_ball(void) {
    LCD_DrawFillRectangle(
        ball_x - BALL_RADIUS, ball_y - BALL_RADIUS,
        ball_x + BALL_RADIUS, ball_y + BALL_RADIUS,
        BLACK
    );
}

void draw_ball(void) {
    LCD_Circle(ball_x, ball_y, BALL_RADIUS, 1, WHITE);
}

void display_high_score(void) {
    draw_background();
    high_score = read_high_score();  // Load from EEPROM

    LCD_DrawFillRectangle(20, 130, 220, 190, WHITE);

    char score_text[30];
    sprintf(score_text, "Prev High Score: %d", high_score);
    LCD_DrawString(50, 140, BLACK, WHITE, score_text, 16, 0);

    for (volatile int i = 0; i < 3000000; i++);
    draw_background();
}


void draw_score(void) {
    char score_text[20];
    sprintf(score_text, "Score: %d", score);

    // Clear old score area
    LCD_DrawFillRectangle(10, LCD_H - 20, 100, LCD_H, BLACK);

    LCD_DrawString(10, LCD_H - 18, WHITE, BLACK, score_text, 16, 0);
}

void setup_game_screen(void) {
    display_high_score();
    draw_background();

    // Initialize bricks
    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            brick_exists[r][c] = 1;
        }
    }

    draw_bricks();
    draw_paddle();
    draw_ball();
    draw_score();
    draw_timer();

}

// Movement functions
void move_paddle_left(void) {
    if (paddle_x > 0) {
        erase_paddle();
        paddle_x -= 2;
        draw_paddle();
        
        if (!ball_moving) {
            erase_ball();
            ball_x -= 2;
            draw_ball();
        }
    }
}

void move_paddle_right(void) {
    if (paddle_x + PADDLE_WIDTH < LCD_W) {
        erase_paddle();
        paddle_x += 2;
        draw_paddle();
        
        if (!ball_moving) {
            erase_ball();
            ball_x += 2;
            draw_ball();
        }
    }
}

void launch_ball(void) {
    if (!ball_moving) {
        ball_moving = 1;
    }
}

int check_brick_collision(void) {
    init_buzzer();
    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int col = 0; col < BRICK_COLS; col++) {
            if (brick_exists[row][col]) {
                int x1 = BRICK_START_X + col * (BRICK_WIDTH + BRICK_GAP);
                int y1 = BRICK_START_Y + row * (BRICK_HEIGHT + BRICK_GAP);
                int x2 = x1 + BRICK_WIDTH;
                int y2 = y1 + BRICK_HEIGHT;

                if (ball_x >= x1 && ball_x <= x2 && ball_y >= y1 && ball_y <= y2) {
                    // Hit a brick
                    brick_exists[row][col] = 0;
                    LCD_DrawFillRectangle(x1, y1, x2, y2, BLACK); // Erase brick
                    buzz_with_dac(500, 200);
                    return 1;
                }
            }
        }
    }
    return 0;
}

// Button read
int is_button_pressed(int button_num) {
    if (button_num == 1) {
        return (GPIOA->IDR & (1 << 1)) == 0; // PA1
    } else if (button_num == 2) {
        return (GPIOA->IDR & (1 << 2)) == 0; // PA2
    } else if (button_num == 3) {
        return (GPIOA->IDR & (1 << 3)) == 0; // PA3
    }
    return 0;
}

// Button setup
void init_buttons(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    GPIOA->MODER &= ~(0x3 << (2 * 1)); // PA1
    GPIOA->MODER &= ~(0x3 << (2 * 2)); // PA2
    GPIOA->MODER &= ~(0x3 << (2 * 3)); // PA3

    GPIOA->PUPDR &= ~(0x3 << (2 * 1));
    GPIOA->PUPDR &= ~(0x3 << (2 * 2));
    GPIOA->PUPDR &= ~(0x3 << (2 * 3));

    GPIOA->PUPDR |= (0x1 << (2 * 1));  // pull-up PA1
    GPIOA->PUPDR |= (0x1 << (2 * 2));  // pull-up PA2
    GPIOA->PUPDR |= (0x1 << (2 * 3));  // pull-up PA3
}

// Main game function
void play_game(void) {
    init_buttons();
    enable_ports_eeprom();
    init_i2c_eeprom();
    enable_ports_gesture();
    init_i2c_gesture();
    apds9960init();           // Init the sensor
    enableGestureSensor(0);
    //i2c_start_prom(0x000, 3, 0);
    setup_game_screen();

    int frame_counter = 0;  // Frame counter for timing seconds
    
    while (1) {
        char gesture = detect_left_or_right_gesture();
        if (gesture == 'R') {
            move_paddle_left();
        }
        if (gesture == 'L') {
            move_paddle_right();
        }

        if (is_button_pressed(3)) {
            init_buzzer();
            buzz_with_dac(10, 200);
            launch_ball();
        }

        if (ball_moving) {
            erase_ball();
            ball_y -= 2; // Move ball up

            if (check_brick_collision()) {
                // Hit brick, reset ball to paddle
                ball_moving = 0;
                ball_x = paddle_x + PADDLE_WIDTH / 2;
                ball_y = paddle_y - BALL_RADIUS - 1;
                score += 10;  // Increase score
                draw_score();
            }

            if (ball_y < 0) {
                // Ball went off screen, reset to paddle
                ball_moving = 0;
                ball_x = paddle_x + PADDLE_WIDTH / 2;
                ball_y = paddle_y - BALL_RADIUS - 1;
            }

            draw_ball();
        }

        for (volatile int i = 0; i < 30000; i++); // small delay

        frame_counter++;

        if (frame_counter >= 33) { // Approximately 1 second (33 * small delays)
            frame_counter = 0;
            time_left--;
            draw_timer();

            if (time_left <= 0) {
                break; // End game when timer runs out
            }
        }
    }

    // After game ends
    draw_background();
    char end_text[40];
    sprintf(end_text, "Game Over! Score: %d", score);
    LCD_DrawString(30, 100, WHITE, BLACK, end_text, 16, 0);
    if (score > high_score) {
        high_score = score;
        save_high_score(high_score);  // Save new high score to EEPROM
        for (volatile int i = 0; i < 100000; i++); // short delay (~5ms)
    }

    char hs_text[40];
    sprintf(hs_text, "High Score: %d", high_score);
    LCD_DrawString(30, 130, WHITE, BLACK, hs_text, 16, 0);
    //i2c_stop_prom();
    while(1); // Stay stuck after game ends
}
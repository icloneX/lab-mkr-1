#include "project.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define ROWS 4
#define COLS 3
#define DISPLAY_SIZE 8
#define BLANK 0xFF

const uint8_t ascii_font[128] = {
    ['0']=0xC0, ['1']=0xF9, ['2']=0xA4, ['3']=0xB0,
    ['4']=0x99, ['5']=0x92, ['6']=0x82, ['7']=0xF8,
    ['8']=0x80, ['9']=0x90, ['P']=0x8C, ['O']=0xC0, 
    ['E']=0x86, ['N']=0xAB, ['-']=0xBF, [' ']=0xFF, ['_']=0xF7
};

typedef enum {
    STATE_LOCKED,
    STATE_PENALTY,
    STATE_OPEN_ANIM,
    STATE_STOPWATCH,
    STATE_MEMO
} safe_state_t;

volatile uint8_t display_data[DISPLAY_SIZE] = {BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK};
volatile uint32_t system_tick_ms = 0;

static const char key_map[ROWS][COLS] = {
    {'1','2','3'}, {'4','5','6'}, {'7','8','9'}, {'*','0','#'}
};
static uint8_t last_keys[ROWS][COLS];

void HC595_sendByte(uint8_t data) {
    for(uint8_t i = 0; i < 8; i++) {
        Pin_DO_Write((data & (0x80 >> i)) ? 1 : 0);
        Pin_CLK_Write(1);
        Pin_CLK_Write(0);
    }
}

CY_ISR(Timer_Int_Handler) {
    static uint8_t digit_idx = 0;
    HC595_sendByte((uint8_t)~(1u << digit_idx));
    HC595_sendByte(display_data[digit_idx]);
    Pin_Latch_Write(1);
    Pin_Latch_Write(0);
    
    digit_idx++;
    if(digit_idx >= DISPLAY_SIZE) digit_idx = 0;
    system_tick_ms++;
}

void rgb_set(uint8_t r, uint8_t g, uint8_t b) {
    LED_R_Write(r); LED_G_Write(g); LED_B_Write(b);
}

void clear_display(void) {
    for(uint8_t i = 0; i < DISPLAY_SIZE; i++) display_data[i] = BLANK;
}

void matrix_init(void) {
    COLUMN_0_SetDriveMode(COLUMN_0_DM_DIG_HIZ);
    COLUMN_1_SetDriveMode(COLUMN_1_DM_DIG_HIZ);
    COLUMN_2_SetDriveMode(COLUMN_2_DM_DIG_HIZ);
    for(uint8_t r = 0; r < ROWS; r++) {
        for(uint8_t c = 0; c < COLS; c++) last_keys[r][c] = 1;
    }
}

char get_key(void) {
    char pressed = 0;
    for(uint8_t c = 0; c < COLS; c++) {
        if(c==0) { COLUMN_0_SetDriveMode(COLUMN_0_DM_STRONG); COLUMN_0_Write(0); }
        else if(c==1) { COLUMN_1_SetDriveMode(COLUMN_1_DM_STRONG); COLUMN_1_Write(0); }
        else if(c==2) { COLUMN_2_SetDriveMode(COLUMN_2_DM_STRONG); COLUMN_2_Write(0); }
        
        CyDelayUs(20);
        uint8_t rows[ROWS] = {ROW_0_Read(), ROW_1_Read(), ROW_2_Read(), ROW_3_Read()};
        
        for(uint8_t r = 0; r < ROWS; r++) {
            if((rows[r] == 0) && (last_keys[r][c] == 1)) pressed = key_map[r][c];
            last_keys[r][c] = rows[r];
        }
        
        if(c==0) { COLUMN_0_Write(1); COLUMN_0_SetDriveMode(COLUMN_0_DM_DIG_HIZ); }
        else if(c==1) { COLUMN_1_Write(1); COLUMN_1_SetDriveMode(COLUMN_1_DM_DIG_HIZ); }
        else if(c==2) { COLUMN_2_Write(1); COLUMN_2_SetDriveMode(COLUMN_2_DM_DIG_HIZ); }
    }
    return pressed;
}

int main(void) {
    CyGlobalIntEnable;
    Timer_Start();
    Timer_Int_StartEx(Timer_Int_Handler);
    matrix_init();
    
    safe_state_t current_state = STATE_LOCKED;
    
    uint32_t last_action_ms = 0;
    uint32_t action_timer_ms = 0;
    
    char pin_buf[5] = {0};
    uint8_t input_idx = 0;
    uint8_t scroll_step = 0;
    bool is_scrolling = false;

    rgb_set(1, 1, 1);

    for(;;) {
        char key = get_key();
        
        if(key == '*') {
            current_state = STATE_LOCKED;
            input_idx = 0;
            memset(pin_buf, 0, sizeof(pin_buf));
            clear_display();
            rgb_set(0, 1, 1);
        }

        switch(current_state) {
            
            // ПАРОЛЬ
            case STATE_LOCKED:
                rgb_set(0, 1, 1);
                clear_display();
                
                for(uint8_t i = 0; i < 4; i++) {
                    if(i < input_idx) display_data[i] = ascii_font[(uint8_t)pin_buf[i]];
                    else display_data[i] = 0xBF;
                }

                if((key >= '0') && (key <= '9') && (input_idx < 4)) {
                    pin_buf[input_idx++] = key;
                }
                
                if((key == '#') && (input_idx == 4)) {
                    if(strncmp(pin_buf, "2026", 4) == 0) {
                        current_state = STATE_OPEN_ANIM;
                        scroll_step = 0;
                        last_action_ms = system_tick_ms;
                    } else {
                        current_state = STATE_PENALTY;
                        action_timer_ms = 15000;
                        last_action_ms = system_tick_ms;
                    }
                    input_idx = 0;
                }
                break;

            // ТАЙМЕР
            case STATE_PENALTY:
                clear_display();
                rgb_set(0, 1, 1);
                
                if((system_tick_ms - last_action_ms) >= 1000) {
                    if(action_timer_ms >= 1000) action_timer_ms -= 1000;
                    else current_state = STATE_LOCKED;
                    last_action_ms = system_tick_ms;
                }
                
                uint8_t sec = action_timer_ms / 1000;
                display_data[0] = ascii_font['P'];
                display_data[2] = ascii_font[sec / 10 + '0'];
                display_data[3] = ascii_font[sec % 10 + '0'];
                break;

            // ДЗЕРКАЛЬНИЙ ТЕКСТ
            case STATE_OPEN_ANIM:
                clear_display();
                rgb_set(1, 0, 1);
                
                const char* txt = "OPE     ";
                if((system_tick_ms - last_action_ms) >= 300) {
                    scroll_step++;
                    if(scroll_step >= 6) {
                        current_state = STATE_STOPWATCH;
                        action_timer_ms = 0;
                        last_action_ms = system_tick_ms;
                    }
                    last_action_ms = system_tick_ms;
                }
                
                for(uint8_t i = 0; i < 4; i++) {
                    uint8_t symbol = ascii_font[(uint8_t)txt[(scroll_step + i) % 8]];
                    display_data[i] = symbol;
                    display_data[7 - i] = symbol;
                }
                break;

            // СЕКУНДОМІР
            case STATE_STOPWATCH:
                clear_display();
                rgb_set(1, 0, 1);
                
                if(key == '9') {
                    current_state = STATE_MEMO;
                    input_idx = 0;
                    is_scrolling = false;
                    memset(pin_buf, 0, sizeof(pin_buf));
                }

                if((system_tick_ms - last_action_ms) >= 10) {
                    action_timer_ms += 10;
                    last_action_ms = system_tick_ms;
                }
                
                uint32_t t_sec = action_timer_ms / 1000;
                display_data[4] = ascii_font[(t_sec / 60) % 10 + '0'] & 0x7F;
                display_data[5] = ascii_font[(t_sec % 60) / 10 + '0'];
                display_data[6] = ascii_font[(t_sec % 60) % 10 + '0'] & 0x7F;
                display_data[7] = ascii_font[((action_timer_ms % 1000) / 100) + '0'];
                break;

            // РУХОМИЙ РЯДОК
            case STATE_MEMO:
                clear_display();
                rgb_set(1, 1, 0);
                
                if((key >= '0') && (key <= '9') && (input_idx < 4)) {
                    pin_buf[input_idx++] = key;
                }
                if(key == '#') {
                    is_scrolling = !is_scrolling;
                    last_action_ms = system_tick_ms;
                }

                if(!is_scrolling) {
                    for(uint8_t i = 0; i < 4; i++) {
                        if(i < input_idx) display_data[i] = ascii_font[(uint8_t)pin_buf[i]];
                    }
                } else {
                    if((system_tick_ms - last_action_ms) >= 350) {
                        scroll_step++;
                        if(scroll_step >= 8) scroll_step = 0;
                        last_action_ms = system_tick_ms;
                    }
                    for(uint8_t i = 0; i < 8; i++) {
                        display_data[i] = ascii_font[(uint8_t)pin_buf[(scroll_step + i) % 4]];
                    }
                }
                break;
        }
        CyDelay(10);
    }
}
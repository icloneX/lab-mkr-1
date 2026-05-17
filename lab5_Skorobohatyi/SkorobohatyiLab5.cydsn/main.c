#include "project.h"

static uint8_t LED_NUM[] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 
    0x80, 0x90, 0xBF, 0x88, 0x83, 0xC6, 0xA1, 0x86, 
    0x8E, 0x7F
};

static void FourDigit74HC595_sendData(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        if (data & (0x80 >> i)) {
            Pin_DO_Write(1);
        } else {
            Pin_DO_Write(0);
        }
        Pin_CLK_Write(1);
        Pin_CLK_Write(0);
    }
}

static void FourDigit74HC595_sendOneDigit(uint8_t position, uint8_t digit, uint8_t dot) {
    if (position >= 8) {
        FourDigit74HC595_sendData(0xFF);
        FourDigit74HC595_sendData(0xFF);
    }
    
    FourDigit74HC595_sendData(0xFF & ~(1 << position));
    
    if (dot) {
        FourDigit74HC595_sendData(LED_NUM[digit] & 0x7F);
    } else {
        FourDigit74HC595_sendData(LED_NUM[digit]);
    }
    
    Pin_Latch_Write(1);
    Pin_Latch_Write(0);
}

const uint8_t keyMap[4][3] = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9},
    {10, 0, 11} 
};

uint8_t ScanMatrixKeypad(void) {
    uint8_t row;
    for (row = 0; row < 4; row++) {
        ROW_0_Write(1); 
        ROW_1_Write(1); 
        ROW_2_Write(1); 
        ROW_3_Write(1);
        
        if(row == 0) ROW_0_Write(0);
        if(row == 1) ROW_1_Write(0);
        if(row == 2) ROW_2_Write(0);
        if(row == 3) ROW_3_Write(0);
        
        CyDelayUs(50);
      
        if (COLUMN_0_Read() == 0) return keyMap[row][0];
        if (COLUMN_1_Read() == 0) return keyMap[row][1];
        if (COLUMN_2_Read() == 0) return keyMap[row][2];
    }
    return 0xFF; 
}

// --- ПОЧАТОК КОДУ З ПУНКТУ 5 ---

// Змінна для відстеження поточного індикатора (рис. 7)
static uint8_t led_counter = 0;

//Обробник переривання таймера 
CY_ISR(Timer_Int_Handler2)
{
    uint8_t data[8] = {7, 6, 5, 4, 3, 2, 1, 0};
    FourDigit74HC595_sendOneDigit(led_counter, data[led_counter], 0);
    led_counter++;
    if(led_counter > 7) led_counter = 0;
}

int main(void)
{
    CyGlobalIntEnable;

    // Запускаємо таймер
    Timer_Start();
    
    // Запускаємо обробник переривання
    Timer_Int_StartEx(Timer_Int_Handler2);

    for(;;)
    {
    }
}
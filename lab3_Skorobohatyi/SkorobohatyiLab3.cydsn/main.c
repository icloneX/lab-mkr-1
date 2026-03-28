#include "project.h"

#define PASS_LEN 3
const uint8_t PASSWORD[PASS_LEN] = {1, 2, 3}; 

uint8_t input_buffer[PASS_LEN];
uint8_t input_index = 0;

void setLEDColor(uint8_t r, uint8_t g, uint8_t b) {
    RED_Write(r);
    GREEN_Write(g);
    BLUE_Write(b);
}

static void (*COLUMN_X_SetDriveMode[3]) (uint8_t mode) = {
    COLUMN_0_SetDriveMode,
    COLUMN_1_SetDriveMode,
    COLUMN_2_SetDriveMode
};

static void (*COLUMN_x_Write[3]) (uint8_t value) = {
    COLUMN_0_Write,
    COLUMN_1_Write,
    COLUMN_2_Write
};

static uint8 (*ROW_x_Read[4]) () = {
    ROW_0_Read,
    ROW_1_Read,
    ROW_2_Read,
    ROW_3_Read
};

static uint8_t keys[4][3];

static const uint8_t key_map[4][3] = {
    {1,  2,  3},
    {4,  5,  6},
    {7,  8,  9},
    {10, 11, 12} 
};

static void initMatrix()
{
    for(int column_index = 0; column_index < 3; column_index++)
    {
        COLUMN_X_SetDriveMode[column_index](COLUMN_0_DM_DIG_HIZ);
    }
}

static void readMatrix()
{
    for(int column_index = 0; column_index < 3; column_index++)
    {
        COLUMN_X_SetDriveMode[column_index](COLUMN_0_DM_STRONG);
        COLUMN_x_Write[column_index](0);

        for(int row_index = 0; row_index < 4; row_index++)
        {
            keys[row_index][column_index] = ROW_x_Read[row_index]();
        }

        COLUMN_X_SetDriveMode[column_index](COLUMN_0_DM_DIG_HIZ);
    }
}

void printMatrix()
{
    SW_Tx_UART_PutCRLF();
    for (int row_index = 0; row_index < 4; row_index++)
    {
        for (int column_index = 0; column_index < 3; column_index++)
        {
            SW_Tx_UART_PutHexInt(keys[row_index][column_index]);
            SW_Tx_UART_PutString(" ");
        }
        SW_Tx_UART_PutCRLF();
    }
    SW_Tx_UART_PutCRLF();
}

int main(void)
{
    CyGlobalIntEnable; 
    
    SW_Tx_UART_Start();
    CyDelay(50); 
    
    SW_Tx_UART_PutCRLF();
    SW_Tx_UART_PutString("Software Transmit UART");
    SW_Tx_UART_PutCRLF();

    initMatrix();
    setLEDColor(0, 0, 0); 
    
    uint8_t last_pressed = 0;
    uint8_t current_pressed = 0;

    for(;;)
    {
        readMatrix();
        current_pressed = 0; 
        
        for (int r = 0; r < 4; r++)
        {
            for (int c = 0; c < 3; c++)
            {
                if (keys[r][c] == 0)
                {
                    current_pressed = key_map[r][c];
                }
            }
        }

        if (current_pressed != last_pressed)
        {
            if (current_pressed == 0)
            {
                SW_Tx_UART_PutString("Button released\r\n");
                setLEDColor(1, 1, 1);
                printMatrix();
            }
            else
            {
                switch (current_pressed)
                {
                    case 1:
                        SW_Tx_UART_PutString("Button 1 pressed\r\n");
                        setLEDColor(0, 1, 1);
                        break;
                    case 2:
                        SW_Tx_UART_PutString("Button 2 pressed\r\n");
                        setLEDColor(1, 0, 1);
                        break;
                    case 3:
                        SW_Tx_UART_PutString("Button 3 pressed\r\n");
                        setLEDColor(1, 1, 0);
                        break;
                    case 4:
                        SW_Tx_UART_PutString("Button 4 pressed\r\n");
                        setLEDColor(0, 0, 1);
                        break;
                    case 5:
                        SW_Tx_UART_PutString("Button 5 pressed\r\n");
                        setLEDColor(0, 1, 0);
                        break;
                    case 6:
                        SW_Tx_UART_PutString("Button 6 pressed\r\n");
                        setLEDColor(1, 0, 0);
                        break;
                    case 7:
                        SW_Tx_UART_PutString("Button 7 pressed\r\n");
                        setLEDColor(0, 1, 1);
                        break;
                    case 8:
                        SW_Tx_UART_PutString("Button 8 pressed\r\n");
                        setLEDColor(1, 0, 1);
                        break;
                    case 9:
                        SW_Tx_UART_PutString("Button 9 pressed\r\n");
                        setLEDColor(1, 1, 0);
                        break;
                    case 10:
                        SW_Tx_UART_PutString("Button * pressed\r\n");
                        setLEDColor(0, 0, 1);
                        break;
                    case 11:
                        SW_Tx_UART_PutString("Button 0 pressed\r\n");
                        setLEDColor(0, 1, 0);
                        break;
                    case 12:
                        SW_Tx_UART_PutString("Button # pressed\r\n");
                        setLEDColor(1, 0, 0);
                        break;
                }
                printMatrix();

                /* ЛОГІКА ПАРОЛЯ */
                input_buffer[input_index] = current_pressed;
                input_index++;

                /* Якщо введено достатню кількість символів */
                if (input_index >= PASS_LEN)
                {
                    uint8_t is_correct = 1;
                    for (int i = 0; i < PASS_LEN; i++)
                    {
                        if (input_buffer[i] != PASSWORD[i])
                        {
                            is_correct = 0;
                            break;
                        }
                    }

                    SW_Tx_UART_PutCRLF();
                    if (is_correct == 1)
                    {
                        SW_Tx_UART_PutString("=== Access allowed ===\r\n");
                    }
                    else
                    {
                        SW_Tx_UART_PutString("=== Access denied ===\r\n");
                    }
                    SW_Tx_UART_PutCRLF();
                    
                    input_index = 0; /* Скидаємо лічильник для нового введення */
                }
            }
            last_pressed = current_pressed;
        }
        CyDelay(20); 
    }
}


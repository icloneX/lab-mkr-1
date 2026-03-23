#include "project.h"

/* arrays of pointers */
/* function of drive mode configuration */
static void (*COLUMN_x_SetDriveMode[3])(uint8_t mode) = {
    COLUMN_0_SetDriveMode,
    COLUMN_1_SetDriveMode,
    COLUMN_2_SetDriveMode
};

/* column write function */
static void (*COLUMN_x_Write[3])(uint8_t value) = {
    COLUMN_0_Write,
    COLUMN_1_Write,
    COLUMN_2_Write
};

/* read row function */
static uint8 (*ROW_x_Read[4])() = {
    ROW_0_Read,
    ROW_1_Read,
    ROW_2_Read,
    ROW_3_Read
};

/* [ROW][COLUMN] */
static uint8_t keys[4][3] = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9},
    {10, 0, 11},
};

/* matrix initialization function */
static void initMatrix()
{
    for(int column_index = 0; column_index < 3; column_index++)
    {
        COLUMN_x_SetDriveMode[column_index](COLUMN_0_DM_DIG_HIZ);
    }
}

/* keys matrix read function */
static void readMatrix()
{
    uint8_t row_counter = sizeof(ROW_x_Read)/sizeof(ROW_x_Read[0]);
    uint8_t column_counter = sizeof(COLUMN_x_Write)/sizeof(COLUMN_x_Write[0]);

    for(int column_index = 0; column_index < column_counter; column_index++)
    {
        COLUMN_x_SetDriveMode[column_index](COLUMN_0_DM_STRONG);
        COLUMN_x_Write;

        for(int row_index = 0; row_index < row_counter; row_index++)
        {
            keys[row_index][column_index] = ROW_x_Read[row_index]();
        }

        COLUMN_x_SetDriveMode[column_index](COLUMN_0_DM_DIG_HIZ);
    }
}

/* matrix print function */
void printMatrix()
{
    SW_Tx_UART_PutCRLF();

    for (int row_index = 0; row_index < 4; row_index++)
    {
        for(int column_index = 0; column_index < 3; column_index++)
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
    SW_Tx_UART_PutCRLF();
    SW_Tx_UART_PutString("Software Transmit UART");
    SW_Tx_UART_PutCRLF();

    initMatrix();

    uint8_t last_state = 0;

    for(;;)
    {
        readMatrix();

        if(keys[0][0] == 0 && last_state != 1)
        {
            last_state = 1;
            SW_Tx_UART_PutString("Button 1 pressed");
            SW_Tx_UART_PutCRLF();
            LED_R_Write(0);
            LED_G_Write(0);
            LED_B_Write(0);
        }

        if(keys[0][1] == 0 && last_state != 2)
        {
            last_state = 2;
            SW_Tx_UART_PutString("Button 2 pressed");
            SW_Tx_UART_PutCRLF();
            LED_R_Write(0);
            LED_G_Write(0);
            LED_B_Write(0);
        }

        if(keys[0][2] == 0 && last_state != 3)
        {
            last_state = 3;
            SW_Tx_UART_PutString("Button 3 pressed");
            SW_Tx_UART_PutCRLF();
            LED_R_Write(0);
            LED_G_Write(0);
            LED_B_Write(0);
        }

        if(keys[1][0] == 0 && last_state != 4)
        {
            last_state = 4;
            SW_Tx_UART_PutString("Button 4 pressed");
            SW_Tx_UART_PutCRLF();
            LED_R_Write(0);
            LED_G_Write(0);
            LED_B_Write(0);
        }

        if(keys[1][1] == 0 && last_state != 5)
        {
            last_state = 5;
            SW_Tx_UART_PutString("Button 5 pressed");
            SW_Tx_UART_PutCRLF();
            LED_R_Write(0);
            LED_G_Write(0);
            LED_B_Write(0);
        }

        if(keys[1][2] == 0 && last_state != 6)
        {
            last_state = 6;
            SW_Tx_UART_PutString("Button 6 pressed");
            SW_Tx_UART_PutCRLF();
            LED_R_Write(0);
            LED_G_Write(0);
            LED_B_Write(0);
        }

        if(keys[2][0] == 0 && last_state != 7)
        {
            last_state = 7;
            SW_Tx_UART_PutString("Button 7 pressed");
            SW_Tx_UART_PutCRLF();
            LED_R_Write(0);
            LED_G_Write(0);
            LED_B_Write(0);
        }

        if(keys[2][1] == 0 && last_state != 8)
        {
            last_state = 8;
            SW_Tx_UART_PutString("Button 8 pressed");
            SW_Tx_UART_PutCRLF();
            LED_R_Write(0);
            LED_G_Write(0);
            LED_B_Write(0);
        }

        if(keys[2][2] == 0 && last_state != 9)
        {
            last_state = 9;
            SW_Tx_UART_PutString("Button 9 pressed");
            SW_Tx_UART_PutCRLF();
            LED_R_Write(0);
            LED_G_Write(0);
            LED_B_Write(0);
        }

        if(keys[3][0] == 0 && last_state != 10)
        {
            last_state = 10;
            SW_Tx_UART_PutString("Button * pressed");
            SW_Tx_UART_PutCRLF();
            LED_R_Write(0);
            LED_G_Write(0);
            LED_B_Write(0);
        }

        if(keys[3][1] == 0 && last_state != 0)
        {
            last_state = 0;
            SW_Tx_UART_PutString("Button 0 pressed");
            SW_Tx_UART_PutCRLF();
            LED_R_Write(0);
            LED_G_Write(0);
            LED_B_Write(0);
        }

        if(keys[3][2] == 0 && last_state != 11)
        {
            last_state = 11;
            SW_Tx_UART_PutString("Button # pressed");
            SW_Tx_UART_PutCRLF();
            LED_R_Write(0);
            LED_G_Write(0);
            LED_B_Write(0);
        }
    }
}
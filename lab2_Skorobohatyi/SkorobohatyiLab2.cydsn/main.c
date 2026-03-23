#include "project.h"
int main(void)
{
CyGlobalIntEnable; /* Enable global interrupts. */
for(;;)
{
RED_Write(0);
CyDelay(500);
RED_Write(1);
CyDelay(500);
BLUE_Write(0);
CyDelay(500);
BLUE_Write(1);
CyDelay(500);
RED_Write(0);
CyDelay(500);
RED_Write(1);
CyDelay(500);
GREEN_Write(0);
CyDelay(500);
GREEN_Write(1);
CyDelay(500);
}
}
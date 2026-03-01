
#include "project.h"
int main(void)
{
    CyGlobalIntEnable;
    for(;;)
    {
     if(Button_Read() == 1)
    {
     BLUE_Write(0);
     RED_Write(0);
     GREEN_Write(1);
    }
    else
    {
     BLUE_Write(1);
     RED_Write(1);
     GREEN_Write(0);
}
    }
}

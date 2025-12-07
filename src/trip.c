#include "trip.h"
#include "mcu.h"
#include <stdio.h>

void Trip_Arm(void)
{
    GPIO_WriteBit(TRIP_EN_GPIO, TRIP_EN_PIN, 1);
    Micros_Wait(100);
    ADC_WD_ON();
}

void Trip_Disarm(void)
{
    ADC_WD_OFF();
    GPIO_WriteBit(TRIP_EN_GPIO, TRIP_EN_PIN, 0);
}

void Trip_Calibrate(void)
{
    Millis_Wait(1000);
    int low = ADC_Read();
    GPIO_WriteBit(TRIP_EN_GPIO, TRIP_EN_PIN, 1);
    Millis_Wait(200);
    int high = ADC_Read();
    GPIO_WriteBit(TRIP_EN_GPIO, TRIP_EN_PIN, 0);
    Millis_Wait(200);
    int threshold = (low + high) /  2;
    ADC_SetThreshold(threshold);
    printf("Set threshold %d\n", threshold);
}
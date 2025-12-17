#include "trip.h"
#include "mcu.h"
#include "led.h"
#include <stdio.h>

#define TRIP_THRESHOLD  900

void Trip_Arm(void)
{
    GPIO_WriteBit(TRIP_EN_GPIO, TRIP_EN_PIN, 1);
    Millis_Wait(100);
    ADC_WD_ON();
}

void Trip_Disarm(void)
{
    ADC_WD_OFF();
    GPIO_WriteBit(TRIP_EN_GPIO, TRIP_EN_PIN, 0);
}

void Trip_Calibrate(void)
{
    GPIO_WriteBit(TRIP_EN_GPIO, TRIP_EN_PIN, 1);
    int light;

    while ((light = ADC_Read()) < TRIP_THRESHOLD) {
        printf("Low trip light level: %d (need %d)\n", light, TRIP_THRESHOLD);
        Led_StatusBlink(1, 50);
        Millis_Wait(500);
    }
    Millis_Wait(2000); 
    GPIO_WriteBit(TRIP_EN_GPIO, TRIP_EN_PIN, 0);

    ADC_SetThreshold(TRIP_THRESHOLD);
}
#include "led.h"
#include "mcu.h"

void Led_LinkSet(bool state)
{
    GPIO_WriteBit(LED_LINK_GPIO, LED_LINK_PIN, state);
}

void Led_StatusSet(bool state)
{
    GPIO_WriteBit(LED_STAT_GPIO, LED_STAT_PIN, state);
}

void Led_StatusToggle(void)
{
    GPIO_WriteBit(LED_STAT_GPIO, LED_STAT_PIN, !GPIO_ReadOutputDataBit(LED_STAT_GPIO, LED_STAT_PIN));
}

void Led_StatusBlink(unsigned times, unsigned duration)
{
    while (times--) {
        Led_StatusSet(1);
        Millis_Wait(duration);
        Led_StatusSet(0);
        Millis_Wait(duration);
    }
}

#include "battery.h"
#include "mcu.h"
#include "led.h"

static uint16_t voltage;

unsigned Battery_GetVoltage(void)
{
    return voltage;
}

static void Update(void)
{
    if (GPIO_ReadOutputDataBit(BATTCTL_GPIO, BATTCTL_PIN) == 1) {
        if (ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC) == SET) {
            uint16_t adcVal = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
            voltage = (uint32_t)adcVal * 4950 / 1023;
            GPIO_ResetBits(BATTCTL_GPIO, BATTCTL_PIN);
            ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);
            ADC_ClearFlag(ADC1, ADC_FLAG_JSTRT);

            TASK_IndicateLowBattery.enabled = voltage < 3400;
        }
        else {
            if (!ADC_GetFlagStatus(ADC1, ADC_FLAG_JSTRT))
                ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);
        }
    }
    else {
        GPIO_SetBits(BATTCTL_GPIO, BATTCTL_PIN);
    }
}

static void IndicateLowBattery(void)
{
    Led_StatusToggle();
}

struct SchedulerTask TASK_CheckBattery = {
    .execute = Update,
    .period = 3333,
    .enabled = 1
};

struct SchedulerTask TASK_IndicateLowBattery = {
    .execute = IndicateLowBattery,
    .period = 150,
    .enabled = 0
};
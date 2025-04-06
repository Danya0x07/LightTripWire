#include "mcu.h"
#include <stdio.h>

#include <sheduler.h>
#include <shell.h>

static void _Blink(void)
{
    static bool state = 0;

    GPIO_WriteBit(LED_LINK_GPIO, LED_LINK_PIN, state);
    state = !state;
}
static struct ShedulerTask TASK_BlinkLed = {.execute = _Blink, .period = 500, .enabled = 1};

static void _PrintBatteryValue(void)
{
    GPIO_WriteBit(BATTCTL_GPIO, BATTCTL_PIN, 1);
    Millis_Wait(1);

    unsigned adcVal= ADC_Read(BATTSENS_CH);
    GPIO_WriteBit(BATTCTL_GPIO, BATTCTL_PIN, 0);

    printf("Batt_Raw: %d\n", adcVal);
}
static struct ShedulerTask TASK_PrintBattery = {.execute = _PrintBatteryValue, .period = 200, .enabled = 0};

static void _PrintLightValue(void)
{
#ifdef LIGHTCTL_PIN
    GPIO_WriteBit(LIGHTCTL_GPIO, LIGHTCTL_PIN, 1);
    Millis_Wait(1);
#endif

    unsigned adcVal= ADC_Read(LIGHTSENS_CH);

#ifdef LIGHTCTL_PIN
    GPIO_WriteBit(LIGHTCTL_GPIO, LIGHTCTL_PIN, 0);
#endif

    printf("Light_Raw: %d\n", adcVal);
}
static struct ShedulerTask TASK_PrintLight = {.execute = _PrintLightValue, .period = 200, .enabled = 0};


static struct ShedulerTask *const shedulerTasks[] = {
    &TASK_BlinkLed,
    &TASK_PrintBattery,
    &TASK_PrintLight,
    NULL
};

static int SetTelemetryMode(int argc, char *argv[])
{
    if (argc != 1)
        return -1;

    if (argv[0][0] == 'l') {
        TASK_PrintLight.enabled = 1;
        TASK_PrintBattery.enabled = 0;
    }
    else if (argv[0][0] == 'b') {
        TASK_PrintLight.enabled = 0;
        TASK_PrintBattery.enabled = 1;
    }
    else if (argv[0][0] == 'n') {
        TASK_PrintLight.enabled = 0;
        TASK_PrintBattery.enabled = 0;
    }
    return 0;
}

static const struct ShellCommand *const shellCommands[] = {
    &(struct ShellCommand){.name = "telemode", .execute = SetTelemetryMode},
    NULL
};

static void PrintClocks(void)
{
    printf("MCU clocks:\n");
    printf("HCLK: %ld\n", MCU_rccClocks.HCLK_Frequency);
    printf("SYSCLK: %ld\n", MCU_rccClocks.SYSCLK_Frequency);
    printf("ADCCLK: %ld\n", MCU_rccClocks.ADCCLK_Frequency);
    printf("PCLK1: %ld\n", MCU_rccClocks.PCLK1_Frequency);
    printf("PCLK2: %ld\n", MCU_rccClocks.PCLK2_Frequency);
}

int main(void)
{

    MCU_Init();
    PrintClocks();
    GPIO_WriteBit(TRIP_EN_GPIO, TRIP_EN_PIN, 1);

    Sheduler_Setup(shedulerTasks);

    while (1) {
        Sheduler_SpinRegular(shedulerTasks);
        Shell_Spin(shellCommands);
    }
    return 0;
}

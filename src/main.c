#include "mcu.h"
#include "radio.h"
#include "button.h"
#include <stdio.h>

#include <sheduler.h>
#include <shell.h>

static void _Blink(void)
{
    static bool state = 0;

    GPIO_WriteBit(LED_LINK_GPIO, LED_LINK_PIN, state);
    state = !state;
}
static struct ShedulerTask TASK_BlinkLed = {.execute = _Blink, .period = 500, .enabled = 0};

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

static bool armed;

static void _SendRadioMessage(void)
{
    uint8_t response;
    if (Radio_Send(armed) < 0) {
        //
    }
    if (Radio_CheckResponse(&response)) {
        //printf("resp: %x\n", response);
        GPIO_WriteBit(LED_LINK_GPIO, LED_LINK_PIN, 1);
    }
    else {
        GPIO_WriteBit(LED_LINK_GPIO, LED_LINK_PIN, 0);
    }
}

static struct ShedulerTask TASK_SendRadio = {.execute = _SendRadioMessage, .period = 100, .enabled = 1};

static struct ShedulerTask *const shedulerTasks[] = {
    &TASK_BlinkLed,
    &TASK_PrintBattery,
    &TASK_PrintLight,
    &TASK_SendRadio,
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

static int SetRadioTransmission(int argc, char *argv[])
{
    if (argc != 1)
        return -1;

    if (argv[0][0] == '1') {
        TASK_SendRadio.enabled = 1;
    }
    else if (argv[0][0] == '0') {
        TASK_SendRadio.enabled = 0;
    }
    return 0;
}

static const struct ShellCommand *const shellCommands[] = {
    &(struct ShellCommand){.name = "telemode", .execute = SetTelemetryMode},
    &(struct ShellCommand){.name = "radio", .execute = SetRadioTransmission},
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

#if (HWVER == 1)
    bool finish = GPIO_ReadInputDataBit(BTN_GPIO, BTN_PIN);
    while (GPIO_ReadInputDataBit(BTN_GPIO, BTN_PIN))
        ;
    Millis_Wait(10);
#elif (HWVER == 2)
    bool finish = GPIO_ReadInputDataBit(FINISH_GPIO, FINISH_PIN);
#endif
    armed = true;
    if (Radio_Init(finish) < 0) {
        printf("Radio init failed\n");
    }

    Sheduler_Setup(shedulerTasks);

    ButtonEvent buttonEvent;
    for (;;) {
        Sheduler_SpinRegular(shedulerTasks);
        Shell_Spin(shellCommands);

        buttonEvent = Button_GetEvent();
        if (buttonEvent == ButtonEvent_PRESS) {
            armed = false;
            GPIO_WriteBit(LED_STAT_GPIO, LED_STAT_PIN, 1);
        }
        else if (buttonEvent == ButtonEvent_RELEASE) {
            armed = true;
            GPIO_WriteBit(LED_STAT_GPIO, LED_STAT_PIN, 0);
        }
        if (buttonEvent != ButtonEvent_NOTHING) {
            _SendRadioMessage();
        }
    }
    return 0;
}

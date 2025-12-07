#include "mcu.h"
#include "radio.h"
#include "button.h"
#include "protocol.h"
#include "trip.h"
#include "led.h"
#include <stdio.h>

#include <scheduler.h>
#include <shell.h>

volatile Role role;
volatile TimerState state = TimerState_HALT;

// static void _PrintBatteryValue(void)
// {
//     GPIO_WriteBit(BATTCTL_GPIO, BATTCTL_PIN, 1);
//     Millis_Wait(1);

//     unsigned adcVal= ADC_Read(BATTSENS_CH);
//     GPIO_WriteBit(BATTCTL_GPIO, BATTCTL_PIN, 0);

//     printf("Batt_Raw: %d\n", adcVal);
// }
// static struct ShedulerTask TASK_PrintBattery = {.execute = _PrintBatteryValue, .period = 200, .enabled = 0};

static void _PrintLightValue(void)
{
#ifdef LIGHTCTL_PIN
    GPIO_WriteBit(LIGHTCTL_GPIO, LIGHTCTL_PIN, 1);
    Millis_Wait(1);
#endif

    unsigned adcVal = ADC_Read();

#ifdef LIGHTCTL_PIN
    GPIO_WriteBit(LIGHTCTL_GPIO, LIGHTCTL_PIN, 0);
#endif

    printf("Light_Raw: %d\n", adcVal);
}
static struct SchedulerTask TASK_PrintLight = {.execute = _PrintLightValue, .period = 200, .enabled = 0};

static void _PerformRadioCommunication(void)
{
    static int retryCounter = 0;
    struct ProtocolMessage response;

    ADC_IRQ_OFF();
    if (Radio_ReadResponse(&response)) {
        retryCounter = 0;
        TimerState requestedState = (TimerState)response.timerState;

        if (state == TimerState_HALT && requestedState == TimerState_IDLE) {
            if (role == Role_START) {
                Trip_Arm();
            }
            state = TimerState_IDLE;
        }
        else if (state == TimerState_IDLE && requestedState == TimerState_RUN) {
            if (role == Role_START) {
                Trip_Disarm();
            }
            else if (role == Role_FINISH) {
                Trip_Arm();
                state = TimerState_RUN;
            }
        }
        else if (state == TimerState_RUN && requestedState == TimerState_HALT) {
            if (role == Role_FINISH) {
                Trip_Disarm();
            }
            state = TimerState_HALT;
        }
    }
    else if (retryCounter < 10) {
        retryCounter++;
    }
    Led_LinkSet(retryCounter < 10);
    Radio_Send(&(struct ProtocolMessage){state}, false);
    ADC_IRQ_ON();
}

static struct SchedulerTask TASK_SendRadio = {.execute = _PerformRadioCommunication, .period = 100, .enabled = 1};

static struct SchedulerTask *const shedulerTasks[] = {
    //&TASK_PrintBattery,
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
        //TASK_PrintBattery.enabled = 0;
    }
    else if (argv[0][0] == 'b') {
        TASK_PrintLight.enabled = 0;
        //TASK_PrintBattery.enabled = 1;
    }
    else if (argv[0][0] == 'n') {
        TASK_PrintLight.enabled = 0;
        //TASK_PrintBattery.enabled = 0;
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

#if (HWVER == 1)
    role = GPIO_ReadInputDataBit(BTN_GPIO, BTN_PIN) == 1 ? Role_FINISH : Role_START;
    while (GPIO_ReadInputDataBit(BTN_GPIO, BTN_PIN))
        ;
    Millis_Wait(10);
#elif (HWVER == 2)
    bool finish = GPIO_ReadInputDataBit(FINISH_GPIO, FINISH_PIN);
#endif
    
    Trip_Calibrate();
    Sheduler_Setup(shedulerTasks);

    if (role == Role_FINISH) {
        printf("Init as finish detector\n");
        Led_StatusBlink(1, 100);
        if (Radio_Init(1) < 0) {
            printf("Radio init failed\n");
        }
    }
    else {
        printf("Init as start detector\n");
        if (Radio_Init(0) < 0) {
            printf("Radio init failed\n");
        }
    }

    ButtonEvent buttonEvent;
    for (;;) {
        Sheduler_SpinRegular(shedulerTasks);
        Shell_Spin(shellCommands);

        buttonEvent = Button_GetEvent();
        if (buttonEvent == ButtonEvent_PRESS) {
            ADC_IRQ_OFF();
            if (role == Role_START) {
                state = TimerState_RUN;
            }
            else if (role == Role_FINISH) {
                state = TimerState_HALT;
            }
            Radio_Send(&(struct ProtocolMessage){state}, false);
            ADC_IRQ_ON();
        }
    }
    return 0;
}

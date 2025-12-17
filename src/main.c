#include "mcu.h"
#include "radio.h"
#include "button.h"
#include "protocol.h"
#include "trip.h"
#include "led.h"
#include "battery.h"
#include <stdio.h>

#include <scheduler.h>
#include <shell.h>

const Role role = ROLE;
volatile TimerState state = TimerState_HALT;

static void _PrintBatteryVoltage(void)
{
    printf("Battery: %dmV\n", Battery_GetVoltage());
}
static struct SchedulerTask TASK_PrintBatteryVoltage = {.execute = _PrintBatteryVoltage, .period = 10000, .enabled = 0};

static void _PrintLightValue(void)
{
    printf("Light: %d\n", ADC_Read());
}
static struct SchedulerTask TASK_PrintLightValue = {.execute = _PrintLightValue, .period = 200, .enabled = 0};

static void _PerformIdleRadioCommunication(void)
{
    static int retryCounter = 10;
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
            }
            state = TimerState_RUN;
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

static struct SchedulerTask TASK_PerformIdleRadioCommunication = {.execute = _PerformIdleRadioCommunication, .period = 100, .enabled = 1};

static struct SchedulerTask *const shedulerTasks[] = {
    &TASK_PrintBatteryVoltage,
    &TASK_PrintLightValue,
    &TASK_PerformIdleRadioCommunication,
    &TASK_CheckBattery,
    &TASK_IndicateLowBattery,
    NULL
};

static int SetTelemetryMode(int argc, char *argv[])
{
    if (argc != 1)
        return -1;

    if (argv[0][0] == 'l') {
        TASK_PrintLightValue.enabled = 1;
        TASK_PrintBatteryVoltage.enabled = 0;
    }
    else if (argv[0][0] == 'b') {
        TASK_PrintLightValue.enabled = 0;
        TASK_PrintBatteryVoltage.enabled = 1;
    }
    else if (argv[0][0] == 'n') {
        TASK_PrintLightValue.enabled = 0;
        TASK_PrintBatteryVoltage.enabled = 0;
    }
    return 0;
}

static int SwitchRadioTransmission(int argc, char *argv[])
{
    if (argc != 1)
        return -1;

    if (argv[0][0] == '1') {
        TASK_PerformIdleRadioCommunication.enabled = 1;
    }
    else if (argv[0][0] == '0') {
        TASK_PerformIdleRadioCommunication.enabled = 0;
    }
    return 0;
}

static const struct ShellCommand *const shellCommands[] = {
    &(struct ShellCommand){.name = "telemode", .execute = SetTelemetryMode},
    &(struct ShellCommand){.name = "radio", .execute = SwitchRadioTransmission},
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

#pragma once

#include <scheduler.h>

unsigned Battery_GetVoltage(void);
extern struct SchedulerTask TASK_CheckBattery;
extern struct SchedulerTask TASK_IndicateLowBattery;
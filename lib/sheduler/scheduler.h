#ifndef _INC_SCHEDULER_H
#define _INC_SCHEDULER_H

/// Simple regular & oneshot sheduler implementation.

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

struct SchedulerTask {
    void (*execute)(void);
    uint32_t period;
    bool enabled;
    uint32_t lastTime;
};

void Sheduler_Setup(struct SchedulerTask *const *tasks);
void Sheduler_SpinOneShot(struct SchedulerTask *const *tasks);
void Sheduler_SpinRegular(struct SchedulerTask *const *tasks);

#endif // _INC_SCHEDULER_H
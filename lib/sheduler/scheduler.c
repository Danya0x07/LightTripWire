#include "scheduler.h"
#include "scheduler_port.h"

#ifndef FOR_EACH_PP
#   define FOR_EACH_PP(parr)   for (void **pp = (void **)parr; *(pp); pp++)
#endif

void Sheduler_Setup(struct SchedulerTask *const *tasks)
{
    struct SchedulerTask *task;

    FOR_EACH_PP(tasks) {
        task = (*pp);
        task->lastTime = Millis_Get();
    }
}

void Sheduler_SpinOneShot(struct SchedulerTask *const *tasks)
{
    struct SchedulerTask *task;

    FOR_EACH_PP(tasks) {
        task = (*pp);
        if (task->enabled && Millis_Get() - task->lastTime >= task->period) {
            task->execute();
            task->enabled = false;
        }
    }
}

void Sheduler_SpinRegular(struct SchedulerTask *const *tasks)
{
    struct SchedulerTask *task;

    FOR_EACH_PP(tasks) {
        task = (*pp);
        if (task->enabled && Millis_Get() - task->lastTime >= task->period) {
            task->lastTime = Millis_Get();
            task->execute();
        }
    }
}

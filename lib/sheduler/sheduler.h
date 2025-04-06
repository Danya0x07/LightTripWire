#ifndef _INC_SHEDULER_H
#define _INC_SHEDULER_H

/// Simple regular & oneshot sheduler implementation.

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

struct ShedulerTask {
    void (*execute)(void);
    uint32_t period;
    bool enabled;
    uint32_t lastTime;
};

void Sheduler_Setup(struct ShedulerTask *const *tasks);
void Sheduler_SpinOneShot(struct ShedulerTask *const *tasks);
void Sheduler_SpinRegular(struct ShedulerTask *const *tasks);

#endif // _INC_SHEDULER_H
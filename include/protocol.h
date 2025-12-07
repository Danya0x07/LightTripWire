#ifndef _INC_PROTOCOL_H
#define _INC_PROTOCOL_H

#include <stdint.h>

#define PROTOCOL_RADIO_CHANNEL  7
#define PROTOCOL_LTW_START_ADDRESS  {0xC7, 0x68, 0xAC}
#define PROTOCOL_LTW_FINISH_ADDRESS {0xCA, 0x86, 0x7C}

#ifndef __packed
#   define __packed __attribute__((packed))
#endif

__packed struct ProtocolMessage {
    uint8_t timerState;
};

_Static_assert(sizeof(struct ProtocolMessage) == 1, "incorrect ProtocolMessage size");

typedef enum {
    TimerState_IDLE,
    TimerState_RUN,
    TimerState_HALT
} TimerState;

typedef enum {
    Role_START,
    Role_FINISH
} Role;

#endif // _INC_PROTOCOL_H
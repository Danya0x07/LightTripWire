#ifndef _INC_PROTOCOL_H
#define _INC_PROTOCOL_H

#include <stdint.h>

#define PROTOCOL_RADIO_CHANNEL  7
#define PROTOCOL_LTW_START_ADDRESS  {0xC7, 0x68, 0xAC}
#define PROTOCOL_LTW_FINISH_ADDRESS {0xCA, 0x86, 0x7C}

#ifndef __packed
#   define __packed __attribute__((packed))
#endif

enum __packed ProtocolCommandTMU {
    ProtocolCommandTMU_ARM,
    ProtocolCommandTMU_DISARM
};

struct ProtocolMessageTMU {
    enum ProtocolCommandTMU cmd;
};

_Static_assert(sizeof(struct ProtocolMessageTMU) == 1, "Incorrect TMU payload size.");

enum __packed ProtocolStatusLTW {
    ProtocolStatusLTW_DISARMED,
    ProtocolStatusLTW_ARMED
};

struct ProtocolMessageLTW {
    enum ProtocolStatusLTW status;
};

_Static_assert(sizeof(struct ProtocolMessageLTW) == 1, "Incorrect LTW payload size.");

#endif // _INC_PROTOCOL_H
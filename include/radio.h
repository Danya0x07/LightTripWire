#ifndef _INC_RADIO_H
#define _INC_RADIO_H

#include <stdint.h>
#include <stdbool.h>
#include "protocol.h"

int Radio_Init(bool useFinishAddress);
void Radio_Send(const struct ProtocolMessage *message, bool overwrite);
bool Radio_ReadResponse(struct ProtocolMessage *response);

#endif // _INC_RADIO_H
#ifndef _INC_RADIO_H
#define _INC_RADIO_H

#include <stdint.h>
#include <stdbool.h>

int Radio_Init(bool finish);
int Radio_Send(bool armed);
bool Radio_CheckResponse(uint8_t *response);

#endif // _INC_RADIO_H
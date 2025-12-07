#pragma once
#include <stdint.h>
#include <stdbool.h>

void Led_LinkSet(bool state);
void Led_StatusSet(bool state);
void Led_StatusToggle(void);
void Led_StatusBlink(unsigned times, unsigned duration);
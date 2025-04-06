#ifndef _INC_SHELL_PORT_H
#define _INC_SHELL_PORT_H

#include <stdbool.h>
#include <uart_io.h>
#include <stdio.h>

static inline int ReadLine(char *buff, unsigned size)
{
    UART_ReadLine(buff, size);
    UART_Flush();
    return size;
}

static inline bool LineAvailable(void)
{
    return UART_LineReceived();
}

static inline void PrintChar(char c)
{
    UART_SendChar(c);
}

static inline void PrintString(const char *s)
{
    UART_SendString(s);
}

static inline void PrintInt(int n)
{
    printf("%d", n);
}

#endif // _INC_SHELL_PORT_H
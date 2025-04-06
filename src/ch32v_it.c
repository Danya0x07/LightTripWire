#include "mcu.h"
#include <uart_io.h>

__attribute__((interrupt("WCH-Interrupt-fast")))
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        UART_RxCallback();
    }
}
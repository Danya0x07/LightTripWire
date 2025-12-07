#include "mcu.h"
#include <uart_io.h>
#include <stdio.h>
#include "protocol.h"
#include "trip.h"
#include "radio.h"

__attribute__((interrupt("WCH-Interrupt-fast")))
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        UART_RxCallback();
    }
}

__attribute__((interrupt("WCH-Interrupt-fast")))
void ADC1_IRQHandler()
{
    extern volatile Role role;
    extern volatile TimerState state;

    if (ADC_GetITStatus(ADC1, ADC_IT_AWD)) {
        // uint16_t val = ADC_GetConversionValue(ADC1);
        // printf("ACWD %d\n", val);
        if (role == Role_START) {
            state = TimerState_RUN;
        }
        else if (role == Role_FINISH) {
            state = TimerState_HALT;
        }
        Trip_Disarm();
        Radio_Send(&(struct ProtocolMessage){state}, true);
        ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
    }
}
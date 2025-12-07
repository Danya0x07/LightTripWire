#ifndef _INC_MCU_H
#define _INC_MCU_H

#include <ch32v00x.h>
#include <stddef.h>

#define LED_LINK_GPIO    GPIOD
#define LED_LINK_PIN     GPIO_Pin_2

#define LED_STAT_GPIO    GPIOD
#define LED_STAT_PIN     GPIO_Pin_3

#define BTN_GPIO         GPIOD
#define BTN_PIN          GPIO_Pin_0

#define UART_GPIO        GPIOC
#define UART_TX_PIN      GPIO_Pin_0
#define UART_RX_PIN      GPIO_Pin_1

#define BATTCTL_GPIO     GPIOA
#define BATTCTL_PIN      GPIO_Pin_1

#define BATTSENS_GPIO    GPIOD
#define BATTSENS_PIN     GPIO_Pin_6
#define BATTSENS_CH      ADC_Channel_6

#define LIGHTSENS_GPIO   GPIOD
#define LIGHTSENS_PIN    GPIO_Pin_5
#define LIGHTSENS_CH     ADC_Channel_5

#define TRIP_EN_GPIO     GPIOD
#define TRIP_EN_PIN      GPIO_Pin_4

#define NRF_CS_GPIO      GPIOC
#define NRF_CS_PIN       GPIO_Pin_4

#define NRF_CE_GPIO      GPIOC
#define NRF_CE_PIN       GPIO_Pin_3

#define NRF_IRQ_GPIO     GPIOC
#define NRF_IRQ_PIN      GPIO_Pin_2

#define SPI_GPIO         GPIOC
#define SPI_SCK_PIN      GPIO_Pin_5
#define SPI_MOSI_PIN     GPIO_Pin_6
#define SPI_MISO_PIN      GPIO_Pin_7


void MCU_Init(void);

void ADC_SetThreshold(uint16_t threshold);
unsigned ADC_Read(void);

#define ADC_WD_ON()     ADC_AnalogWatchdogCmd(ADC1, ADC_AnalogWatchdog_SingleRegEnable)
#define ADC_WD_OFF()    ADC_AnalogWatchdogCmd(ADC1, ADC_AnalogWatchdog_None);
#define ADC_IRQ_ON()    NVIC_EnableIRQ(ADC_IRQn)
#define ADC_IRQ_OFF()   NVIC_DisableIRQ(ADC_IRQn)

uint32_t Micros_Get(void);
void Micros_Wait(uint32_t ms);

uint32_t Millis_Get(void);
void Millis_Wait(uint32_t ms);

uint8_t SPI_TransferByte(uint8_t data);
void SPI_TransferBytes(uint8_t *in, const uint8_t *out, uint16_t len);

extern RCC_ClocksTypeDef MCU_rccClocks;

#endif // _INC_MCU_H
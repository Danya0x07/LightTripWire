#include "mcu.h"

RCC_ClocksTypeDef MCU_rccClocks;

static void initRCC(void)
{
    FLASH_SetLatency(FLASH_Latency_1);

    RCC_HCLKConfig(RCC_SYSCLK_Div3); // 8 МГц
    RCC_GetClocksFreq(&MCU_rccClocks);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1
            | RCC_APB2Periph_AFIO
            | RCC_APB2Periph_GPIOA
            | RCC_APB2Periph_GPIOC
            | RCC_APB2Periph_GPIOD
            | RCC_APB2Periph_SPI1
            | RCC_APB2Periph_TIM1
            | RCC_APB2Periph_USART1, ENABLE);
}

static void initGPIO(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // LEDS
    GPIO_InitStructure.GPIO_Pin = LED_LINK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(LED_LINK_GPIO, &GPIO_InitStructure);

#if (HWVER == 1)
    GPIO_InitStructure.GPIO_Pin = LED_STAT_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(LED_STAT_GPIO, &GPIO_InitStructure);
#endif

    // BTN / FINISH jumper
#if (HWVER == 1)
    GPIO_InitStructure.GPIO_Pin = BTN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(BTN_GPIO, &GPIO_InitStructure);
#elif (HWVER == 2)
    GPIO_InitStructure.GPIO_Pin = FINISH_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(FINISH_GPIO, &GPIO_InitStructure);
#endif

    // UART
    GPIO_PinRemapConfig(GPIO_FullRemap_USART1, ENABLE);
    GPIO_InitStructure.GPIO_Pin = UART_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(UART_GPIO, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = UART_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(UART_GPIO, &GPIO_InitStructure);

    // BATTCTL
    GPIO_InitStructure.GPIO_Pin = BATTCTL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(BATTCTL_GPIO, &GPIO_InitStructure);

    // BATTSENS
    GPIO_InitStructure.GPIO_Pin = BATTSENS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(BATTSENS_GPIO, &GPIO_InitStructure);

#if (HWVER == 2)
    // LIGHTCTL
    GPIO_InitStructure.GPIO_Pin = LIGHTCTL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(LIGHTCTL_GPIO, &GPIO_InitStructure);
#endif

    // LIGHTSENS
    GPIO_InitStructure.GPIO_Pin = LIGHTSENS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(LIGHTSENS_GPIO, &GPIO_InitStructure);

    // TRIP_EN
    GPIO_InitStructure.GPIO_Pin = TRIP_EN_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(TRIP_EN_GPIO, &GPIO_InitStructure);

    // NRF_CS
    GPIO_InitStructure.GPIO_Pin = NRF_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(NRF_CS_GPIO, &GPIO_InitStructure);
    GPIO_WriteBit(NRF_CS_GPIO, NRF_CS_PIN, 1);

    // NRF_CE
    GPIO_InitStructure.GPIO_Pin = NRF_CE_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(NRF_CE_GPIO, &GPIO_InitStructure);

    // NRF_IRQ
    GPIO_InitStructure.GPIO_Pin = NRF_IRQ_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(NRF_IRQ_GPIO, &GPIO_InitStructure);
}

static void initTimers(void)
{
    SysTick->CTLR = 0x9;  // HCLK / 8 = 1 кГц
}

static inline void initUART(uint32_t baudrate)
{
    USART_InitTypeDef USART_InitStructure = {0};

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

static inline void initSPI(void)
{
    SPI_InitTypeDef  SPI_InitStructure = {0};

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
}

static void initADC(void)
{
    ADC_InitTypeDef ADC_InitStructure = {0};

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_Cmd(ADC1, ENABLE);
    __NOP();
    __NOP();

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1))
        ;
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1))
        ;
}

static void initPFIC(void)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    // UART
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void MCU_Init(void)
{
    initRCC();
    initGPIO();
    initTimers();
    initUART(115200);
    initSPI();
    initADC();
    initPFIC();
}

unsigned ADC_Read(unsigned channel)
{
	ADC_RegularChannelConfig(ADC1, (uint16_t)channel, 1, ADC_SampleTime_30Cycles);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        ;
	uint16_t val = ADC_GetConversionValue(ADC1);

	return val;
}

uint32_t Micros_Get(void)
{
    return SysTick->CNT;
}

void Micros_Wait(uint32_t ms)
{
    uint32_t now = Micros_Get();

    while (Micros_Get() - now < ms)
        ;
}

uint32_t Millis_Get(void)
{
    return SysTick->CNT / 1000;
}

void Millis_Wait(uint32_t ms)
{
    uint32_t now = Millis_Get();

    while (Millis_Get() - now < ms)
        ;
}

uint8_t SPI_TransferByte(uint8_t data)
{
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
        ;
    SPI_I2S_SendData(SPI1, data);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
        ;
    return SPI_I2S_ReceiveData(SPI1);
}

void SPI_TransferBytes(uint8_t *in, const uint8_t *out, uint16_t len)
{
    if (in == NULL) {
        while (len--)
            SPI_TransferByte(*out++);
    } else if (out == NULL) {
        while (len--)
            *in++ = SPI_TransferByte(0);
    } else {
        while (len--)
            *in++ = SPI_TransferByte(*out++);
    }
}

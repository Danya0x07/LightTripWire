#if defined(CH32V00X)
#include <ch32v00x.h>
#elif defined(CH32V10X)
#include <ch32v10x.h>
#elif defined(CH32V20X)
#include <ch32v20x.h>
#elif defined(CH32V30X) || defined(CH32V31X)
#include <ch32v30x.h>
#elif defined (CH32X035)
#include <ch32x035.h>
#elif defined(CH32L10X)
#include <ch32l103.h>
#endif
#include <debug.h>


#define BLINKY_GPIO_PORT GPIOD
#define BLINKY_GPIO_PIN_1 GPIO_Pin_3
#define BLINKY_GPIO_PIN_2 GPIO_Pin_2
#if defined(CH32L10X)
#define BLINKY_CLOCK_ENABLE RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOC, ENABLE)
#else
#define BLINKY_CLOCK_ENABLE RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE)
#endif







void tauno_ADC_init(u8 channel)
{
    ADC_InitTypeDef  ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    switch (channel) {
        case 0:  // PA2
        case 1:  // PA1
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
            break;
        case 2:  // PC4
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
            break;
        case 3:  // PD2
        case 4:  // PD3
        case 5:  // PD5
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
        case 6:  // PD6
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
        case 7:  // PD4
            break;
        default:
            break;
    }

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    switch (channel) {
        case 0:  // PA2
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
            break;
        case 1:  // PA1
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
            break;
        case 2:  // PC4
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
            break;
        case 3:  // PD2
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
            break;
        case 4:  // PD3
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
            break;
        case 5:  // PD5
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
            break;
        case 6:  // PD6
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
            break;
        case 7:  // PD4
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
            break;
        default:
            break;
    }

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;

    switch (channel) {
        case 0:  // PA2
        case 1:  // PA1
            GPIO_Init(GPIOA, &GPIO_InitStructure);
            break;
        case 2:  // PC4
            GPIO_Init(GPIOC, &GPIO_InitStructure);
            break;
        case 3:  // PD2
        case 4:  // PD3
        case 5:  // PD5
            GPIO_Init(GPIOD, &GPIO_InitStructure);
        case 6:  // PD6
            GPIO_Init(GPIOD, &GPIO_InitStructure);
        case 7:  // PD4
            break;
        default:
            break;
    }

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Calibration_Vol(ADC1, ADC_CALVOL_50PERCENT);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    //s16 Calibrattion_Val = Get_CalibrationValue(ADC1); ????
}

u16 Get_ADC_Val(u8 channel)
{
    u16 val;

    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_241Cycles);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    val = ADC_GetConversionValue(ADC1);

    return val;
}


u16 Get_ADC_Average(u8 times, u8 channel)
{
    u32 temp_val = 0;
    u8 t;
    u16 val;

    for (t = 0; t < times; t++)
    {
        temp_val += Get_ADC_Val(channel);
        Delay_Ms(5);
    }

    val = temp_val / times;

    return val;
}


float getVoltage()
{

    float rescoef = 1.5f;
    float vref = 3.3f;
    u16 ADCbits;

    GPIO_WriteBit(GPIOA, GPIO_Pin_1, 1);
    ADCbits = Get_ADC_Average(5, 6);
    GPIO_WriteBit(GPIOA, GPIO_Pin_1, 0);


    float adcVoltage = (vref * ADCbits * rescoef)/1024.0f;


    return adcVoltage;
}


int main(void) {
    uint16_t adc_val;  // 0-1023
    u16 i;

    Delay_Init();
    USART_Printf_Init(115200);
    Delay_Ms(3000);  // Give serial monitor time to open

//    tauno_ADC_init(6);
    tauno_ADC_init(5);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_241Cycles);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    Delay_Ms(50);
    ADC_SoftwareStartConvCmd(ADC1, DISABLE);

    //        printf("%d.%04d\n", wholePart, decimalPart);

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_WriteBit(GPIOD, GPIO_Pin_4, 1);

    while (1) {

        int LED_input = Get_ADC_Average(5,5);

        printf("%08d\n", LED_input);
//        printf("%d\n", LED_input == 0);


    }
    return 0;
}

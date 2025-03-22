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
#include <time.h>


#if defined(CH32L10X)
#define BLINKY_CLOCK_ENABLE RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOC, ENABLE)
#else
#define BLINKY_CLOCK_ENABLE RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE)
#endif


// indication:
#define LINK_LED_PORT GPIOD
#define LINK_LED_PIN GPIO_Pin_2


// battery:
#define BATT_ADC_CHANNEL 7
#define BATT_CTL_PORT GPIOD
#define BATT_CTL_PIN GPIO_Pin_3
#define BATT_TIMES_OF_MEASURINGS 5


// light:
#define LIGHT_ADC_CHANNEL 6
#define LIGHT_CTL_PORT GPIOD
#define LIGHT_CTL_PIN GPIO_Pin_5
#define LIGHT_TIMES_OF_MEASURINGS 5

#define MAIN_LED_PORT GPIOD
#define MAIN_LED_PIN GPIO_Pin_0






void tauno_ADC_init(u8 channel)
{
    ADC_InitTypeDef  ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    switch (channel) {
        case 0:  // PA2
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
            break;
        case 1:  // PA1
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
            break;
        case 2:  // PC4
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
            break;
        case 3:  // PD2
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
            break;
        case 4:  // PD3
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
            break;
        case 5:  // PD5
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
            break;
        case 6:  // PD6
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
            break;
        case 7:  // PD4
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
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
        case 6:  // PD6
        case 7:  // PD4
            GPIO_Init(GPIOD, &GPIO_InitStructure);
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


uint16_t getVoltage()
{
    uint16_t ADCbits;

    GPIO_WriteBit(BATT_CTL_PORT, BATT_CTL_PIN, 1);
    Delay_Ms(10);
    ADCbits = Get_ADC_Average(BATT_TIMES_OF_MEASURINGS, BATT_ADC_CHANNEL);
    GPIO_WriteBit(BATT_CTL_PORT, BATT_CTL_PIN, 0);
    Delay_Ms(10);

    return ADCbits;
}


void pinsInitialization(){


    GPIO_InitTypeDef GPIO_InitStructure = {0};


    GPIO_InitStructure.GPIO_Pin = BATT_CTL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BATT_CTL_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LIGHT_CTL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LIGHT_CTL_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = MAIN_LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MAIN_LED_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LINK_LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LINK_LED_PORT, &GPIO_InitStructure);

}


int main(void) {

    Delay_Init();
    USART_Printf_Init(115200);
    Delay_Ms(500);

    tauno_ADC_init(BATT_ADC_CHANNEL);
    tauno_ADC_init(LIGHT_ADC_CHANNEL);



    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_241Cycles);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    Delay_Ms(50);
    ADC_SoftwareStartConvCmd(ADC1, DISABLE);



    pinsInitialization();





    GPIO_WriteBit(MAIN_LED_PORT, MAIN_LED_PIN, 1);

    while (1) {

        GPIO_WriteBit(LIGHT_CTL_PORT, LIGHT_CTL_PIN, 1);
        Delay_Ms(10);
        int LED_input = Get_ADC_Average(LIGHT_TIMES_OF_MEASURINGS,LIGHT_ADC_CHANNEL);
        GPIO_WriteBit(LIGHT_CTL_PORT, LIGHT_CTL_PIN, 0);
        Delay_Ms(10);

        time_t seconds;

        seconds = time(NULL);

        printf("%04d\n", LED_input);


    }
    return 0;
}

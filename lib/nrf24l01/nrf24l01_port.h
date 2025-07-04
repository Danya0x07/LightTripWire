/**
 * @file    nrf24l01_port.h
 * @brief   Платформозависимая часть библиотеки.
 *
 * Путём реализации интерфейса, объявленного в данном файле можно перенести
 * библиотеку на любую платформу.
 *
 * @note
 * В данном файле можно подключать любые заголовочные файлы, необходимые
 * для реализации объявленного интерфейса.
 */

#ifndef _NRF24L01_PORT_H
#define _NRF24L01_PORT_H

/*
 * Здесь может быть подключение пользовательских заголовочных файлов
 */

#include "mcu.h"

/**
 * @name    Макросы для управления выводами трансивера.
 * @{
 */
#define _csn_high() GPIO_SetBits(NRF_CS_GPIO, NRF_CS_PIN)
#define _csn_low()  GPIO_ResetBits(NRF_CS_GPIO, NRF_CS_PIN)
#define _ce_high()  GPIO_SetBits(NRF_CE_GPIO, NRF_CE_PIN)
#define _ce_low()   GPIO_ResetBits(NRF_CE_GPIO, NRF_CE_PIN)
/** @} */

/**
 * @brief   Обменивается байтом по SPI.
 *
 * @note
 * Если в проекте используется несколько библиотек, работающих с SPI, то будет
 * целесообразно вынести соответствующую функцию в отдельный файл, а в этой
 * функции просто делегировать ей работу.
 *
 * @bug
 * Если функция, которой данная функция/макрос делегирует обмен байтом по SPI,
 * находится в отдельной единице трансляции, то как минимум для
 * arm-none-eabi-gcc v6.3.1 имеет место баг, вызванный оптимизацией компилятора.
 * Баг проявляется на уровнях оптимизации, отличных от -O0 и -Og.
 * Симптомы: некорректные значения при передаче/приёме данных по SPI.
 */
static inline uint8_t _spi_transfer_byte(uint8_t byte)
{
    return SPI_TransferByte(byte);
}

/**
 * @brief   Обменивается массивом байт по SPI.
 *
 * @param[in] out   Буфер исходящих данных. NULL, если не исопльзуется.
 * @param[out] in   Буфер входящих данных. NULL, если не исопльзуется.
 * @param len   Количество байт для обмена.
 *
 * @note
 * Если в проекте используется несколько библиотек, работающих с SPI, то будет
 * целесообразно вынести соответствующую функцию в отдельный файл, а в этой
 * функции просто делегировать ей работу.
 *
 * @warning
 * in и out не могут быть NULL одновременно.
 */
static inline void _spi_transfer_bytes(uint8_t *in, const uint8_t *out,
                                       size_t len)
{
    SPI_TransferBytes(in, out, len);
}

/** Функция миллисекундной задержки. */
#define _delay_ms(ms)   Millis_Wait((ms))

/**
 * Функция микросекундной задержки.
 *
 * @note
 * Данная функция не является необходимой. Если микросекундная задержка не
 * доступна, необходимо указать это в файле nrf24l01_conf.h
 */
#define _delay_us(us)   Micros_Wait((us))

#endif /* _NRF24L01_PORT_H */

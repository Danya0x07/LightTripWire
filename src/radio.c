#include "radio.h"
#include "protocol.h"
#include "mcu.h"
#include <nrf24l01.h>

int Radio_Init(bool finish)
{
    struct nrf24l01_tx_config config = {
        .address = finish ? (uint8_t [3])PROTOCOL_LTW_FINISH_ADDRESS : (uint8_t [3])PROTOCOL_LTW_START_ADDRESS,
        .addr_size = NRF24L01_ADDRS_3BYTE,
        .crc_mode = NRF24L01_CRC_1BYTE,
        .datarate = NRF24L01_DATARATE_1MBPS,
        .power = NRF24L01_POWER_0DBM,
        .retr_delay = NRF24L01_RETR_DELAY_250US,
        .retr_count = NRF24L01_RETR_COUNT_10,
        .mode = NRF24L01_TX_MODE_ACK_PAYLOAD,
        .en_irq = 0,
        .rf_channel = PROTOCOL_RADIO_CHANNEL
    };

    Millis_Wait(NRF24L01_PWR_ON_DELAY_MS);

    return nrf24l01_tx_configure(&config);
}

void Radio_Send(const struct ProtocolMessage *message, bool overwrite)
{
    if (nrf24l01_full_tx_fifo() || overwrite)  {
        nrf24l01_flush_tx_fifo();
    }
    if (nrf24l01_get_interrupts() & NRF24L01_IRQ_MAX_RT) {
        nrf24l01_flush_tx_fifo();
        nrf24l01_clear_interrupts(NRF24L01_IRQ_MAX_RT);
    }
    nrf24l01_tx_write_pld(message, sizeof(struct ProtocolMessage));
    nrf24l01_tx_transmit();
}

bool Radio_ReadResponse(struct ProtocolMessage *response)
{
    bool responseReceived = false;
    uint8_t irq = nrf24l01_get_interrupts();

    if (irq & NRF24L01_IRQ_TX_DS) {
        responseReceived = true;
    }
    if (irq & NRF24L01_IRQ_RX_DR) {
        do {
            if (nrf24l01_read_pld_size() != sizeof(struct ProtocolMessage)) {
                nrf24l01_flush_rx_fifo();
                nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);
                responseReceived = false;
                continue;
            }
            nrf24l01_read_pld(response, sizeof(struct ProtocolMessage));
            nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);
            responseReceived = true;
        } while (nrf24l01_data_in_rx_fifo());
    }
    return responseReceived;
}

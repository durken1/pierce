#include "nrf_gzll.h"
#include "nrf_drv_twi.h"

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define SLAVE_ADDR 0x20

#define TWI_INSTANCE_ID 0

static uint8_t m_data_payload[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];
static uint8_t matrix[8];
static uint8_t data[9];

static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

// Initiate TWI (I2C)
void twi_init(void)
{
    const nrf_drv_twi_config_t twi_config = {
        .scl = 19,
        .sda = 20,
        .frequency = NRF_TWI_FREQ_400K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .clear_bus_init = false
    };

    nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);

    nrf_drv_twi_enable(&m_twi);
}

// Send scanned matrix to pro micro (or equivalent)
void i2c_send_to_slave(void)
{
    data[0] = 0;
    for (int i = 0; i < 8; ++i)
    {
        data[i+1] = matrix[i];
    }
    nrf_drv_twi_tx(&m_twi, 0x20U, data, 9, false);
}

// Recieve data from device and save to local matrix array
void nrf_gzll_host_rx_data_ready(uint32_t pipe, nrf_gzll_host_rx_info_t rx_info)
{
    uint32_t data_payload_length = 4;

    nrf_gzll_fetch_packet_from_rx_fifo(pipe, m_data_payload, &data_payload_length);

    if (data_payload_length > 0)
    {
        if (pipe == 0)
        {
            for(uint8_t i = 0;i < data_payload_length; ++i)
            {
                matrix[i] = m_data_payload[i];
            }
        }

        else
        {
            for(uint8_t i = 0;i < data_payload_length; ++i)
            {
                matrix[i+4] = m_data_payload[i];
            }
        }
    }
}

// Initiate and enable TWI, gazell and start sending matrix over TWI
int main(void)
{
    twi_init();

    nrf_gzll_init(NRF_GZLL_MODE_HOST);

    nrf_gzll_enable();

    while(true)
    {
        i2c_send_to_slave();
    }
}

// Empty required callbacks
void nrf_gzll_device_tx_success(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
}

void nrf_gzll_device_tx_failed(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
}

void nrf_gzll_disabled()
{
}

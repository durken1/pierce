#include "host.h"
#include "nrf_gzll.h"
#include "nrf_drv_twi.h"
#include "nrf_gpio.h"

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define SLAVE_ADDR 0x20

#define TWI_INSTANCE_ID 0

static uint32_t data_payload_length = 4;
static uint8_t m_data_payload[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];
static uint8_t matrix[8];
static uint8_t data[9];
static uint32_t input;

static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

// Configure and initiate GPIO pins
static void gpio_init(void)
{
    nrf_gpio_cfg_input(R0C0, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(R0C1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(R0C2, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(R0C3, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(R0C4, NRF_GPIO_PIN_PULLUP);

    nrf_gpio_cfg_input(R1C0, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(R1C1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(R1C2, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(R1C3, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(R1C4, NRF_GPIO_PIN_PULLUP);

    nrf_gpio_cfg_input(R2C0, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(R2C1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(R2C2, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(R2C3, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(R2C4, NRF_GPIO_PIN_PULLUP);

    nrf_gpio_cfg_input(R3C0, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(R3C1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(R3C2, NRF_GPIO_PIN_PULLUP);
}

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

// Scan the key matrix
static void matrix_scan()
{
    input = ~(NRF_GPIO->IN);

    matrix[4] = (((input >> R0C0) & 1) |
            (((input >> R0C1) & 1) << 1) |
            (((input >> R0C2) & 1) << 2) |
            (((input >> R0C3) & 1) << 3) |
            (((input >> R0C4) & 1) << 4)) &
        (0x1F);

    matrix[5] = (((input >> R1C0) & 1) |
            (((input >> R1C1) & 1) << 1) |
            (((input >> R1C2) & 1) << 2) |
            (((input >> R1C3) & 1) << 3) |
            (((input >> R1C4) & 1) << 4)) &
        (0x1F);

    matrix[6] = (((input >> R2C0) & 1) |
            (((input >> R2C1) & 1) << 1) |
            (((input >> R2C2) & 1) << 2) |
            (((input >> R2C3) & 1) << 3) |
            (((input >> R2C4) & 1) << 4)) &
        (0x1F);

    matrix[7] = (((input >> R3C0) & 1)  |
            (((input >> R3C1) & 1) << 1) |
            (((input >> R3C2) & 1) << 2)) &
        (0x1F);
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
    gpio_init();
    twi_init();
    nrf_gzll_init(NRF_GZLL_MODE_HOST);

    nrf_gzll_enable();

    while(true)
    {
        matrix_scan();
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

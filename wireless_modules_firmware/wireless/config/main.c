#include "pierce.h"
#include "nrf_gzll.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_rtc.h"

// Set RTC instance
const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(0); 

// Frequency of RTC = 32.768 / (RTC_PRESCALER + 1)
#define RTC_PRESCALER 32 // 32 -> RTC frequency ~ 1kHz

// Number of inactive ticks before going to deep sleep
#define INACTIVITY_THRESHOLD 500 // 0.5sec with 1kHz RTC

// Maximum number of transmission attempts
#define MAX_TX_ATTEMPTS         100 

static uint32_t input;
static uint8_t matrix[ROWS] = {0, 0, 0, 0};

// Configure and initiate GPIO pins
static void gpio_init(void)
{
    nrf_gpio_cfg_sense_input(R0C0, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(R0C1, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(R0C2, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(R0C3, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(R0C4, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);

    nrf_gpio_cfg_sense_input(R1C0, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(R1C1, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(R1C2, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(R1C3, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(R1C4, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);

    nrf_gpio_cfg_sense_input(R2C0, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(R2C1, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(R2C2, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(R2C3, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(R2C4, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);

    nrf_gpio_cfg_sense_input(R3C2, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(R3C3, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(R3C4, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
}

// Scan the key matrix
static void matrix_scan(uint8_t *matrix)
{
    input = ~(NRF_GPIO->IN);

#ifdef RIGHT
    matrix[0] = (((input >> R0C4) & 1) |
            (((input >> R0C3) & 1) << 1) |
            (((input >> R0C2) & 1) << 2) |
            (((input >> R0C1) & 1) << 3) |
            (((input >> R0C0) & 1) << 4)) &
        (0x1F);

    matrix[1] = (((input >> R1C4) & 1) |
            (((input >> R1C3) & 1) << 1) |
            (((input >> R1C2) & 1) << 2) |
            (((input >> R1C1) & 1) << 3) |
            (((input >> R1C0) & 1) << 4)) &
        (0x1F);

    matrix[2] = (((input >> R2C4) & 1) |
            (((input >> R2C3) & 1) << 1) |
            (((input >> R2C2) & 1) << 2) |
            (((input >> R2C1) & 1) << 3) |
            (((input >> R2C0) & 1) << 4)) &
        (0x1F);

    matrix[3] = (((input >> R3C4) & 1)  |
            (((input >> R3C3) & 1) << 1) |
            (((input >> R3C2) & 1) << 2)) &
        (0x1F);

#else
    matrix[0] = (((input >> R0C0) & 1) |
            (((input >> R0C1) & 1) << 1) |
            (((input >> R0C2) & 1) << 2) |
            (((input >> R0C3) & 1) << 3) |
            (((input >> R0C4) & 1) << 4)) &
        (0x1F);

    matrix[1] = (((input >> R1C0) & 1) |
            (((input >> R1C1) & 1) << 1) |
            (((input >> R1C2) & 1) << 2) |
            (((input >> R1C3) & 1) << 3) |
            (((input >> R1C4) & 1) << 4)) &
        (0x1F);

    matrix[2] = (((input >> R2C0) & 1) |
            (((input >> R2C1) & 1) << 1) |
            (((input >> R2C2) & 1) << 2) |
            (((input >> R2C3) & 1) << 3) |
            (((input >> R2C4) & 1) << 4)) &
        (0x1F);

    matrix[3] = ((((input >> R3C2) & 1) << 2) |
            (((input >> R3C3) & 1) << 3) |
            (((input >> R3C4) & 1) << 4)) &
        (0x1F);
#endif
}

// Check if no keys are pressed
static bool empty_matrix(const uint8_t* matrix)
{
    for(int i=0; i < ROWS; i++)
    {
        if (matrix[i])
        {
            return false;
        }
    }
    return true;
}

// Send the matrix status to host
void matrix_send(uint8_t *matrix)
{   
    nrf_gzll_add_packet_to_tx_fifo(PIPE_NUMBER, matrix, ROWS);
}

// Set MCU to go to deep sleep after set number of ticks with no keypresses 
static void handle_inactivity(const uint8_t *matrix)
{
    static uint32_t inactivity_ticks = 0;

    if (empty_matrix(matrix)) {
        inactivity_ticks++;
        if (inactivity_ticks > INACTIVITY_THRESHOLD) {
            nrf_drv_rtc_disable(&rtc);
            inactivity_ticks = 0;

            NRF_POWER->SYSTEMOFF = 1;
        }
    } else {
        inactivity_ticks = 0;
    }
}

// Do this every clock tick
static void tick(nrf_drv_rtc_int_type_t int_type)
{
    matrix_scan(matrix);

    handle_inactivity(matrix);

    matrix_send(matrix);
}

// RTC peripheral configuration
static void rtc_config(void)
{
    nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
    config.prescaler = 32;
    nrf_drv_rtc_init(&rtc, &config, tick);

    nrf_drv_rtc_tick_enable(&rtc,true);

    nrf_drv_rtc_enable(&rtc);
}

// Low frequency clock configuration
static void lfclk_config(void)
{
    nrf_drv_clock_init();

    nrf_drv_clock_lfclk_request(NULL);
}

// Initiate and configure everything, then go to sleep and wait for clock tick interrupt
int main()
{
    gpio_init();
    lfclk_config();
    rtc_config();

    nrf_gzll_init(NRF_GZLL_MODE_DEVICE);
    nrf_gzll_set_max_tx_attempts(MAX_TX_ATTEMPTS);

    nrf_gzll_enable();
    while (1)
    {
        __SEV();
        __WFE();
        __WFE();
    }
}

// Unused callbacks
void nrf_gzll_host_rx_data_ready(uint32_t pipe, nrf_gzll_host_rx_info_t rx_info)
{
}


void nrf_gzll_disabled()
{
}

void  nrf_gzll_device_tx_success(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
}

void nrf_gzll_device_tx_failed(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
}


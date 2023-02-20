#include "stdio.h"
#include "time.h"
#include "string.h"

#include "stdint.h"
#include "stdbool.h"

#include "trans_spi.h"

int main()
{

    TransSpi_t spi_h;
    TransSpiConfig_t spi_config;

    // config for the spi bus
    spi_config.bits_per_word = 8;
    spi_config.speed = 4000000;
    spi_config.protocol_mode = TransSpiProtocolMode_Mode_0;
    strncpy(spi_config.device_name, "/dev/spidev0.0", 15);


    // delay for 100ms
    uint32_t msec = 100;
    struct timespec ts;
    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    TransSpi_create(&spi_h, &spi_config);

    int32_t spi_status;

    const uint8_t BUFFER_SIZE = 4;
    uint8_t tx_buf[BUFFER_SIZE];
    uint8_t rx_buf[BUFFER_SIZE];
    


    while (true)
    {
        nanosleep(&ts, &ts);
        spi_status = TransSpi_transfer(&spi_h, tx_buf, rx_buf, BUFFER_SIZE);
        printf("response from device");
        for (int i=0; i < BUFFER_SIZE; i++)
        {
            printf(" 0x%02x", rx_buf[i]);
        }
        printf("\n");
    }

    TransSpi_destroy(&spi_h);

    return 0;
}
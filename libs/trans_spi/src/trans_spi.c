#include "trans_spi.h"
#include "sys/ioctl.h"
#include "unistd.h"
#include "fcntl.h"
#include "errno.h"
#include "stdio.h"
#include "string.h"

int32_t TransSpi_create(TransSpi_t *spi_h, TransSpiConfig_t *config)
{
    int32_t status = TRANS_SPI_NO_ERROR;

    // attempt to open
    spi_h->fd_ = open(config->device_name, O_RDWR);
    if (spi_h->fd_ < 0)
    {
        printf("TransSpi: Failure to open device %s. Error code %d\n", config->device_name, errno);
        status = TRANS_SPI_OPEN_ERROR;
    }

    // dispatch configuration commands
    if (status == TRANS_SPI_NO_ERROR)
    {
        status |= _TransSpi_set_bus_speed(spi_h, config->speed);
        status |= _TransSpi_set_bits_per_word(spi_h, config->bits_per_word);
        status |= _TransSpi_set_protocol_mode(spi_h, config->protocol_mode);

        // close if failure to configure and opened
        if (status != TRANS_SPI_NO_ERROR)
        {
            printf("TransSpi: Failed to configure device %s it will be closed. Error code %d\n", config->device_name,
                   errno);
            TransSpi_destroy(spi_h);
            status = TRANS_SPI_CONFIG_ERROR;
        }
    }
    // return
    return status;
}

int32_t TransSpi_destroy(TransSpi_t *spi_h)
{
    int32_t status = TRANS_SPI_NO_ERROR;
    // attempt to close
    if (close(spi_h->fd_) != 0)
    {
        printf("TransSpi: Failure to close device. Error code %d\n", errno);
        status = TRANS_SPI_CLOSE_ERROR;
    }

    // return
    return status;
}

int32_t TransSpi_transfer(TransSpi_t *spi_h, int8_t *tx_buf, int8_t *rx_buf, uint32_t buf_size)
{
    int32_t status = TRANS_SPI_NO_ERROR;

    // create transfer structure and populate it
    struct spi_ioc_transfer transfer_buf;
    memset(&transfer_buf, 0, sizeof(struct spi_ioc_transfer));
    transfer_buf.tx_buf = (unsigned long long)tx_buf;
    transfer_buf.rx_buf = (unsigned long long)rx_buf;
    transfer_buf.len = buf_size;
    transfer_buf.speed_hz = spi_h->speed_;
    transfer_buf.bits_per_word = spi_h->bits_per_word_;
    // LM: I have no idea if this should be here and what the consequences are. The docs talk about not being able to
    // trigger the polarity of the cs pin from user space so I assume this is what its referring too and actually it
    // does nothing. This will require proper testing
    transfer_buf.cs_change = 1;

    // read/write transfer structure
    if (ioctl(spi_h->fd_, SPI_IOC_MESSAGE(1), &transfer_buf) < 0)
    {
        printf("TransSpi: failure to transfer data.  Error code %d\n", errno);
        status = TRANS_SPI_TRANSFER_ERROR;
    }

    // return
    return status;
}

int32_t _TransSpi_set_bus_speed(TransSpi_t *spi_h, uint32_t speed)
{
    int32_t status = TRANS_SPI_NO_ERROR;

    // attempt to set speed of spi bus
    if (ioctl(spi_h->fd_, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
    {
        printf("TransSpi: Failed attempt to configure speed of bus Error code %d\n", errno);
        status = TRANS_SPI_CONFIG_ERROR;
    }

    // copy speed so it can be used in transfers
    spi_h->speed_ = speed;

    // return
    return status;
}

int32_t _TransSpi_set_bits_per_word(TransSpi_t *spi_h, int8_t bits_per_word)
{
    int32_t status = TRANS_SPI_NO_ERROR;

    // attempt to set number of bits per word for spi bus
    if (ioctl(spi_h->fd_, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word))
    {
        printf("TransSpi: failed attempt to set bits per word. Error code %d\n", errno);
        status = TRANS_SPI_CONFIG_ERROR;
    }
    // copy bits_per_word so it can be used in transfers
    spi_h->bits_per_word_ = bits_per_word;

    // return
    return status;
}

int32_t _TransSpi_set_protocol_mode(TransSpi_t *spi_h, TransSpiProtocolMode_t protocol_mode)
{
    int32_t status = TRANS_SPI_NO_ERROR;
    uint32_t mode_word;

    // validate arg
    if ((protocol_mode != TransSpiProtocolMode_Mode_0) && (protocol_mode != TransSpiProtocolMode_Mode_1) &&
        (protocol_mode != TransSpiProtocolMode_Mode_2) && (protocol_mode != TransSpiProtocolMode_Mode_3))
    {
        printf("TransSpi: attempt to set invalid bus protocol. Error code %d\n", errno);
        status = TRANS_SPI_BAD_ARGUMENT_ERROR;
    }

    // attempt to get old mode word config
    if (status == TRANS_SPI_NO_ERROR)
    {
        status = _TransSpi_get_mode_word(spi_h, &mode_word);
    }

    if (status == TRANS_SPI_NO_ERROR)
    {
        // mask mode bits
        mode_word &= ~(SPI_MODE_X_MASK);
        // set new protocol mode
        mode_word |= protocol_mode;

        // set new mode word
        status = _TransSpi_set_mode_word(spi_h, &mode_word);
    }
    // return
    return status;
}

int32_t _TransSpi_get_mode_word(TransSpi_t *spi_h, uint32_t *mode_word)
{
    int32_t status = TRANS_SPI_NO_ERROR;

    // attempt to get mode word
    if (ioctl(spi_h->fd_, SPI_IOC_RD_MODE32, mode_word) < 0)
    {
        status = TRANS_SPI_GET_MODE_ERROR;
        printf("TransSpi: Failure to get mode word. Error code %d\n", errno);
    }

    // return
    return status;
}

int32_t _TransSpi_set_mode_word(TransSpi_t *spi_h, uint32_t *mode_word)
{
    int32_t status = TRANS_SPI_NO_ERROR;

    // attempt to set mode word
    if (ioctl(spi_h->fd_, SPI_IOC_WR_MODE32, mode_word) < 0)
    {
        status = TRANS_SPI_SET_MODE_ERROR;
        printf("TransSpi: Failure to set mode word\n");
    }

    // return
    return status;
}

#ifndef TRANS_SPI_H
#define TRANS_SPI_H
/**
 * @file trans_spi.h
 * @author Lucien Morey
 * @brief Generic hardware abstraction layer (HAL) for SPI devices.
 * @version 0.1
 * @date 2023-01-10
 *
 * @copyright Copyright (c) 2023
 *
 */
// #include "io_lib.h"
#include "linux/spi/spidev.h"
#include "stdint.h"

#define TRANS_SPI_NO_ERROR 0
#define TRANS_SPI_BAD_ARGUMENT_ERROR -1
#define TRANS_SPI_OPEN_ERROR -2
#define TRANS_SPI_CLOSE_ERROR -3
#define TRANS_SPI_CONFIG_ERROR -4
#define TRANS_SPI_TRANSFER_ERROR -5
#define TRANS_SPI_GET_MODE_ERROR -6
#define TRANS_SPI_SET_MODE_ERROR -7

/**
 * @brief Structure to represent an SPI connection.
 *
 */
typedef struct
{
    int32_t fd_;
    int8_t bits_per_word_;
    uint32_t speed_;
} TransSpi_t;

/**
 * @brief Protocol modes in SPI. These are typically found in the datasheet or can be seen in an explanation here:
 * https://blog.stratifylabs.dev/device/2019-08-03-SPI-modes/
 *
 */
typedef enum
{
    TransSpiProtocolMode_Mode_0 = SPI_MODE_0,
    TransSpiProtocolMode_Mode_1 = SPI_MODE_1,
    TransSpiProtocolMode_Mode_2 = SPI_MODE_2,
    TransSpiProtocolMode_Mode_3 = SPI_MODE_3
} TransSpiProtocolMode_Enum_t;

typedef int8_t TransSpiProtocolMode_t;

/**
 * @brief  SPI configuration structure.
 *
 */
typedef struct
{
    char device_name[16];
    TransSpiProtocolMode_t protocol_mode;
    int8_t bits_per_word;
    uint32_t speed;
} TransSpiConfig_t;

/**
 * @brief Constructor for an SPI transport. this will instantiate the device with the specified configuration so it is
 * ready for read/write
 *
 * @param spi_h Pointer to communication device to be initialised
 * @param config Pointer to configuration for the SPI device
 * @return int32_t Status code return to indicate success of an operation, with values less than 0 indicating a failure
 * @retval #TRANS_SPI_NO_ERROR @copybrief TRANS_SPI_NO_ERROR
 * @retval #TRANS_SPI_OPEN_ERROR @copybrief TRANS_SPI_OPEN_ERROR
 * @retval #TRANS_SPI_CONFIG_ERROR @copybrief TRANS_SPI_CONFIG_ERROR
 */
int32_t TransSpi_create(TransSpi_t *spi_h, TransSpiConfig_t *config);

/**
 * @brief Destructor for an SPI transport. this will close a previously opened interface.
 *
 * @param spi_h Pointer to an interface that will be destroyed
 * @return int32_t Status code return to indicate success of an operation, with values less than 0 indicating a failure
 * @retval #TRANS_SPI_NO_ERROR @copybrief TRANS_SPI_NO_ERROR
 * @retval #TRANS_SPI_CLOSE_ERROR @copybrief TRANS_SPI_CLOSE_ERROR
 */
int32_t TransSpi_destroy(TransSpi_t *spi_h);

/**
 * @brief Configure the bus speed on an open SPI device.
 *
 * @param spi_h Pointer to an SPI device that has been opened and needs to be configured
 * @param tx_buf Pointer to a buffer of data that will be written out during the transfer process
 * @param rx_buf Pointer to a buffer of data that will be written to during the transfer process
 * @param buf_size size of both the rx and tx buffers
 * @warning tx buf and rx buf should not be smaller than buf size, less data is lost or written to a spot it shouldn't
 * be
 * @return int32_t Status code return to indicate success of an operation, with values less than 0 indicating a failure
 * @retval #TRANS_SPI_NO_ERROR @copybrief TRANS_SPI_NO_ERROR
 * @retval #TRANS_SPI_TRANSFER_ERROR @copybrief TRANS_SPI_TRANSFER_ERROR
 */
int32_t TransSpi_transfer(TransSpi_t *spi_h, int8_t *tx_buf, int8_t *rx_buf, uint32_t buf_size);

/**
 * @brief Configure the bus speed on an open SPI device.
 *
 * @param spi_h Pointer to an SPI device that has been opened and needs to be configured
 * @param speed New speed of the SPI device in Hz
 * @return int32_t Status code return to indicate success of an operation, with values less than 0 indicating a failure
 * @retval #TRANS_SPI_NO_ERROR @copybrief TRANS_SPI_NO_ERROR
 * @retval #TRANS_SPI_CONFIG_ERROR @copybrief TRANS_SPI_CONFIG_ERROR
 */
int32_t _TransSpi_set_bus_speed(TransSpi_t *spi_h, uint32_t speed);

/**
 * @brief Configure the expected number of data bits per word for an opened SPI device.
 *
 * @param spi_h Pointer to an SPI device that has been opened and needs to be configured
 * @param bits_per_word Integer number of bits per word
 * @return int32_t Status code return to indicate success of an operation, with values less than 0 indicating a failure
 * @retval #TRANS_SPI_NO_ERROR @copybrief TRANS_SPI_NO_ERROR
 * @retval #TRANS_SPI_CONFIG_ERROR @copybrief TRANS_SPI_CONFIG_ERROR
 */
int32_t _TransSpi_set_bits_per_word(TransSpi_t *spi_h, int8_t bits_per_word);

/**
 * @brief Configure the bus mode on an open SPI device.
 *
 * @param spi_h Pointer to an SPI device that has been opened and needs to be configured
 * @param protocol_mode New SPI mode for a device
 * @return int32_t Status code return to indicate success of an operation, with values less than 0 indicating a failure
 * @retval #TRANS_SPI_NO_ERROR @copybrief TRANS_SPI_NO_ERROR
 * @retval #TRANS_SPI_BAD_ARGUMENT_ERROR @copybrief TRANS_SPI_BAD_ARGUMENT_ERROR
 * @retval #TRANS_SPI_GET_MODE_ERROR @copybrief TRANS_SPI_GET_MODE_ERROR
 * @retval #TRANS_SPI_SET_MODE_ERROR @copybrief TRANS_SPI_SET_MODE_ERROR
 */
int32_t _TransSpi_set_protocol_mode(TransSpi_t *spi_h, TransSpiProtocolMode_t protocol_mode);

/**
 * @brief get the mode word for an SPI bus (protocol mode, bit order, active high cs, etc)
 *
 * @param spi_h Pointer to an SPI device that has been opened and needs to be configured
 * @param mode_word pointer to an integer that will have its value overriden with the current mode flags of an SPI
 * device
 * @note only protocol mode has been written so far to interface with the mode word and more functions can be written to
 * interface in the same way
 * @return int32_t Status Code return to indicate success of an operation, with values less than 0 indicating a failure
 * @retval #TRANS_SPI_NO_ERROR @copybrief TRANS_SPI_NO_ERROR
 * @retval #TRANS_SPI_GET_MODE_ERROR @copybrief TRANS_SPI_GET_MODE_ERROR
 */
int32_t _TransSpi_get_mode_word(TransSpi_t *spi_h, uint32_t *mode_word);

/**
 * @brief set the mode word for an SPI bus (protocol mode, bit order, active high cs, etc)
 *
 * @param spi_h Pointer to an SPI device that has been opened and needs to be configured
 * @param mode_word pointer to an integer that will have its value written to the SPI device registers
 * @note only protocol mode has been written so far to interface with the mode word and more functions can be written to
 * interface in the same way
 * @return int32_t Status Code return to indicate success of an operation, with values less than 0 indicating a failure
 * @retval #TRANS_SPI_NO_ERROR @copybrief TRANS_SPI_NO_ERROR
 * @retval #TRANS_SPI_SET_MODE_ERROR @copybrief TRANS_SPI_SET_MODE_ERROR
 */
int32_t _TransSpi_set_mode_word(TransSpi_t *spi_h, uint32_t *mode_word);

#endif
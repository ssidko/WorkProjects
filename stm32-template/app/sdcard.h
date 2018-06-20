#ifndef __SDCARD_H
#define __SDCARD_H

#include "stdint.h"
#include "gpio.h"
#include "spi.h"

struct sdc_command {
    uint32_t command :6;
    uint32_t start_bits :2;
    uint32_t argument :32;
    uint32_t stop_bit :1;
    uint32_t crc :7;
} __attribute__((packed));

struct sdc_response {
    uint8_t idle_state :1;
    uint8_t erase_reset :1;
    uint8_t illegal_command :1;
    uint8_t crc_error :1;
    uint8_t erase_sequence_error :1;
    uint8_t adress_error :1;
    uint8_t param_error :1;
    uint8_t zero :1;
} __attribute__((packed));

struct sdc_response_2 {
    uint32_t check_pattern :8;
    uint32_t voltage :4;
    uint32_t reserved :16;
    uint32_t version :4;
} __attribute__((packed));

void sdc_send_command(SPI_TypeDef *spi, sdc_command &cmd);

class SDCard
{
public:
    SDCard(SPI_TypeDef *_spi, GPIO_TypeDef *_sd_cs_port, Pin _sd_cs_pin);
private:
    SPI_TypeDef *spi;
    GPIO_TypeDef *sd_cs_port;
    Pin sd_cs_pin;
};


#endif // __SDCARD_H
#ifndef __SDCARD_H
#define __SDCARD_H

#include "stdint.h"
#include "gpio.h"
#include "spi.h"

struct sdc_command {
    uint32_t command_index :6;
    uint32_t start_bit :1;
    uint32_t transmission_bit :1;
    uint32_t argument :32;
    uint32_t end_bit :1;
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

void crc7_generate_table();
bool sdc_send_command(SPI_TypeDef *spi, sdc_command &cmd);

class SDCard
{
public:
    SDCard(SPI_TypeDef *sdc_spi, GPIO_TypeDef *sdc_cs_port, Pin sdc_cs_pin);
    bool Initialize();
    bool SendCommand(uint8_t command, uint32_t argument);
    bool ReadBlock(uint32_t block, uint8_t *buff, size_t size);
    bool WriteBlcok(uint32_t block, uint8_t *buff, size_t size);
private:
    void Select(void);
    void Deselect(void);
private:
    SPI_TypeDef *spi;
    GPIO_TypeDef *cs_port;
    Pin cs_pin;
};


#endif // __SDCARD_H
#include "sdcard.h"

void sdc_send_command(SPI_TypeDef *spi, sdc_command &cmd)
{
    spi_send_buff(spi, (uint8_t *)&cmd, 6);

    uint32_t wait = 2000;
    uint8_t resp = 0x00;

    do {
        resp = spi_send_receive(spi, 0xFF);
        if (resp == 0x01) {
            break;
        }
    } while(wait--);

    if (wait == 0x00) {


    }


}
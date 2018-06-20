#include "sdcard.h"

uint8_t io_buff[512] = {0};

void sdc_send_command(SPI_TypeDef *spi, sdc_command &cmd)
{
    spi_crc_clear(spi);
    spi_send_buff(spi, (uint8_t *)&cmd, 6);

    uint32_t wait = 10;
    uint8_t resp_length = 1;
    uint8_t resp[5] = {0};
    sdc_response *response = (sdc_response*)resp;
    sdc_response_2 *r2 = (sdc_response_2 *)&resp[1];

    if (cmd.command == 58) {
        resp_length = 5;
    }

    do {
        resp[0] = spi_send_receive(spi, 0xFF);
        if (response->zero == 0) {
            for (int i = 1; i < resp_length; i++) {
                resp[resp_length - i] = spi_send_receive(spi, 0xFF);
            }

            if (cmd.command == 17) {
                for (int i = 0; i < 512; i++) {
                    io_buff[i] = spi_send_receive(spi, 0xFF);
                }
            }

            uint8_t crc[2] = {0};
            crc[0] = spi_send_receive(spi, 0xFF);
            crc[1] = spi_send_receive(spi, 0xFF);

            break;
        }
    } while(wait--);


    int x = 0;
    x++;


}
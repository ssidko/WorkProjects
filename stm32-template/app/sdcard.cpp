#include "sdcard.h"

uint8_t io_buff[512] = {0};

uint8_t crc7_table[256];
 
void crc7_generate_table()
{
    uint8_t crc_poly = 0x89;  // CRC-7 polynom
    for (int i = 0; i < 256; ++i) {
        crc7_table[i] = (i & 0x80) ? i ^ crc_poly : i;
        for (int j = 1; j < 8; ++j) {
            crc7_table[i] <<= 1;
            if (crc7_table[i] & 0x80) {
                crc7_table[i] ^= crc_poly;
            }
        }
    }
}

uint8_t crc7(uint8_t *data, size_t size)
{
    uint8_t crc7 = 0;
    for (int i = 0; i < size; ++i) {
        crc7 = crc7_table[(crc7 << 1) ^ data[i]];
    }
    return crc7;
}

bool sdc_send_command(SPI_TypeDef *spi, sdc_command &cmd)
{
    cmd.crc = crc7((uint8_t *)&cmd, 5);
    spi_send_buff(spi, (uint8_t *)&cmd, 6);

    uint32_t wait = 10;
    uint8_t resp_length = 1;
    uint8_t resp[5] = {0};
    sdc_response *response = (sdc_response*)resp;
    sdc_response_2 *r2 = (sdc_response_2 *)&resp[1];

    if (cmd.command == 58) {
        resp_length = 5;
    }

    bool result = false;

    do {
        resp[0] = spi_send_receive(spi, 0xFF);
        if (response->zero == 0) {
            result = true;
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

    return result;
}


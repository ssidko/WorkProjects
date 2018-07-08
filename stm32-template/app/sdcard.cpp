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

    switch (cmd.command_index) {
        // R2
        case 13:
            resp_length = 2;
            break;
        // R7
        case 8:
        case 58:
            resp_length = 5;
            break;
        // R1
        default:
            resp_length = 1;
            break;
    }

    if (cmd.command_index == 58) {
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

            if (cmd.command_index == 17) {
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

SDCard::SDCard(SPI_TypeDef *sdc_spi, GPIO_TypeDef *sdc_cs_port, Pin sdc_cs_pin) :
    spi(sdc_spi),
    chip_select(sdc_cs_port, sdc_cs_pin, PinConfig::Output_50MHz_PushPull)
{
}

bool SDCard::Initialize(void)
{
    //
    // Suppply ramp up time
    //
    chip_select.High();
    for (uint32_t i = 0; i < 10; i++) {
        spi_send_receive(spi, 0xff);
    }    

    chip_select.Low();

    SendCMD(0, 0);


    chip_select.High();
    return false;
}

bool SDCard::SendCMD(uint8_t command, uint32_t argument)
{
    sdc_command cmd = {0};
    cmd.start_bit = 0;
    cmd.transmission_bit = 1;
    cmd.command_index = command;
    
    uint8_t *src_arg = (uint8_t *)&argument;
    uint8_t *dst_arg = (uint8_t *)&cmd.argument;
    dst_arg[0] = src_arg[3];
    dst_arg[1] = src_arg[2];
    dst_arg[2] = src_arg[1];
    dst_arg[3] = src_arg[0];

    cmd.crc = crc7((uint8_t *)&cmd, 5);
    cmd.end_bit = 1;

    spi_send_buff(spi, (uint8_t *)&cmd, 6);

    SdcardType sdc_type = SdcardType::Unknown;
    size_t response_len = 1;
    switch (command) {
        case 13:
            response_len = 2;
            break;
        case 8:
        case 58:
            response_len = 5;
            break;
    }






    return false;
}

bool SDCard::SendACMD(uint8_t acmd, uint32_t argument)
{
    return false;
}

bool SDCard::ReadBlock(uint32_t block, uint8_t *buff, size_t size)
{
    return false;
}

bool SDCard::WriteBlcok(uint32_t block, uint8_t *buff, size_t size)
{
    return false;
}

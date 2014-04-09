#define HSE_VALUE 8000000

#include "stm32f4xx.h"
#include "usbd_cdc_vcp.h" // подключаем USB CDC
#include "i2c3.h"
#include "stm32f4xx_gpio.h"

#define I2C_CTPM_ADDRESS        (char)0x38

__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

char ch3[]="receive 1\n";
char ch4[]="receive 2\n";
char ch5[]="receive 3\n";
char ch6[]="receive 4\n";

volatile int sys_tick_counter;

void Delay(int delay)
{
	sys_tick_counter = delay;
	while (sys_tick_counter > 0);
}

void PrepareForUpdate(void)
{
	char cmd_buff[10] = {0};


}

#define SLAVE_ADDRESS			(char)0x4A

void i2c1_test(void)
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_4);
	Delay(10);
	GPIO_SetBits(GPIOD, GPIO_Pin_4);
	Delay(50);

	int test = 0;
	char reg = 0x01;
	char val = 0x00;

	I2C1_Write(SLAVE_ADDRESS, &reg, sizeof(reg));
	I2C1_Read(SLAVE_ADDRESS, &val, sizeof(val));

	test++;
	test--;
}

int main(void)
{
	SystemInit();
	SysTick_Config(168000);
 	USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb);

 	//I2C3_Initialize();

 	I2C1_Initialize();
	i2c1_test();

	while (1) {

		if(usb_cdc_kbhit()) { // проверка: приняты данные?
			char c;
			c = usb_cdc_getc();  // получение байта с usb
			switch(c) { // разбор принятого байта
				case '1':
					usb_cdc_printf(ch3); // отправка байта на хост
					break;
				case '2':
					usb_cdc_printf(ch4);
					break;
				case '3':
					usb_cdc_printf(ch5);
					break;
				case '4':
					usb_cdc_printf(ch6);
					break;
				default:
					usb_cdc_printf("recived unknown char\n");
					break;
			}
		}
	}
}

void SysTick_Handler(void)
{
	--sys_tick_counter;
}

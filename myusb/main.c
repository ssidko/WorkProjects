#define HSE_VALUE 8000000

#include "stm32f4xx.h"
#include "usbd_cdc_vcp.h" // подключаем USB CDC

__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;



const char ch3[]="receive 1\x0D\x0A";
const char ch4[]="receive 2\x0D\x0A";
char       ch5[]="receive 3\x0D\x0A";
char       ch6[]="receive 4\x0D\x0A";



int main(void)
{
	SystemInit(); // настройки тактирования

	// включаем usb
	USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb);

	while (1){

		if(usb_cdc_kbhit()){ // проверка: приняты данные?
			char c;
			c = usb_cdc_getc();  // получение байта с usb
			switch(c){ // разбор принятого байта
				case '1':
					usb_cdc_printf((char *)ch3); // отправка байта на хост
					break;
				case '2':
					usb_cdc_printf((char *)ch4);
					break;
				case '3':
					usb_cdc_printf(ch5);
					break;
				case '4':
					usb_cdc_printf(ch6);
					break;
				default:
					usb_cdc_printf("recived unknown char\r\n");
					break;
			}
		}
	}
}

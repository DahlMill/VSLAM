#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <log.h>
#include <app_uart.h>

#define APP_UART 			"/dev/ttyS2"
#define DEVICE_LED_UART_BAUD_RATE 115200

static int livUartFd;

int main(int32_t argc, char** argv)
{
	int ret;
	int8_t buf[512];
	char data_str[4] = {'a','b','c','d'};
	int i;
	log_info(">====Welcome to Uart LED Test=====<");

	livUartFd = app_uart_open(livUartFd, SOCKET_FILE);
	app_uart_Init(livUartFd, DEVICE_LED_UART_BAUD_RATE, 0, 8, 1, 'N');

	while(1) {
		app_uart_write(livUartFd, data_str, 4);
		usleep(800);
		app_uart_read(livUartFd,buf,sizeof(buf),2000);
		for(i=0;i<16;i++) printf("data = %x \r\n",buf[i]);
	}
	return 0;
}

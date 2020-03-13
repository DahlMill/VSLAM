/*
 * @Author: DahlMill
 * @Date: 2020-03-05 17:47:08
 * @LastEditors: DahlMill
 * @LastEditTime: 2020-03-05 17:48:54
 * @FilePath: /VSLAM/include/app_uart.h
 */
#ifndef _APP_UART_H_
#define _APP_UART_H_

#ifdef __cplusplus
extern "C"
{
#endif
int app_uart_open(int fd, const char *port);
int app_uart_write(int fd, unsigned char *send_buf, int data_len);
int app_uart_read(int fd, unsigned char *rcv_buf, int data_len, uint32_t timeout_msec);
int app_uart_Init(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity);
#ifdef __cplusplus
}
#endif

#endif


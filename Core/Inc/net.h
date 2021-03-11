/*
 *  w5500_general.h
 *
 *  Created on: Jan 19, 2021
 *  Author: Anton Shein<anton-shein2008@yandex.ru>
 *  ----------------------------------------------------------------------
 *  Copyright (C) Anton Shein, 2021
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *  ----------------------------------------------------------------------
 */


#ifndef INC_NET_H_
#define INC_NET_H_

#include <main.h>
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "stm32f1xx_hal.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "w5500.h"

#define W5500_USING_SPI     hspi1

#define W5500_GPIO_PORT     GPIOA
#define W5500_GPIO_SCS_PIN  GPIO_PIN_4
#define W5500_GPIO_RST_PIN  GPIO_PIN_3

#define DEBUG_UART_PRINT


enum STATE
{
    W5500_OK    = 0,
    W5500_ERR   = 1
};


/* Init external functions */
uint8_t net_init(void);
uint8_t net_tcp_serv_open(uint8_t socket, uint8_t addr[4], uint32_t port);
uint8_t net_tcp_serv_tx_rx(uint8_t socket_number, char *p_tx_data, uint8_t *p_rx_data);
uint8_t net_tcp_serv_rx(uint8_t socket_number, uint8_t *buff);
uint8_t net_tcp_serv_tx(uint8_t socket_number, char *buff);
uint8_t net_tcp_serv_close(uint8_t socket_number);


#endif /* INC_NET_H_ */

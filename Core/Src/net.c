/*
 *  w5500_general.c
 *
 *  Created on: Jan 19, 2021
 *  Author: Anton Shein<anton-shein2008@yandex.ru>
 *  ----------------------------------------------------------------------
 *  Copyright (C) Anton Shein, 2021
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later Bversion.
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

#include <net.h>


/* Init variables */
wiz_NetInfo g_WIZNETINFO;
uint8_t         mac[6] =        {0x40, 0x08, 0xDC, 0x00, 0xAB, 0xCD},
                ip[4] =         {192, 168, 11, 155},
                netmask[4] =    {255, 255, 255, 0},
                gateway[4] =    {192, 168, 1, 1},
                dnsserv[4] =    {0, 0, 0, 0};
extern SPI_HandleTypeDef        W5500_USING_SPI;
extern UART_HandleTypeDef       huart1;

#ifdef DEBUG_UART_PRINT
/**
 * @brief  Print some text to UART
 */
void uart_print(char *print_data, ...) {
    char sbuf[1024];
    va_list argptr;
    va_start(argptr, print_data);
    vsprintf(sbuf, print_data, argptr);
    va_end(argptr);

    HAL_UART_Transmit(&huart1, (uint8_t*)sbuf, strlen(sbuf), HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart1, (uint8_t*)"\n\r", 2, HAL_MAX_DELAY);
}
#endif

/**
 * @brief  Select W5500 at SPI transaction
 */
void W5500_select(void)
{
    HAL_GPIO_WritePin(W5500_GPIO_PORT, W5500_GPIO_SCS_PIN, GPIO_PIN_RESET);
}


/**
 * @brief  Deselect W5500 after SPI transaction
 */
void W5500_deselect(void)
{
    HAL_GPIO_WritePin(W5500_GPIO_PORT, W5500_GPIO_SCS_PIN, GPIO_PIN_SET);
}


/**
 * @brief  Reset W5500 in case of chip freeze
 */
void W5500_reset(void)
{
    HAL_GPIO_WritePin(W5500_GPIO_PORT, W5500_GPIO_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(W5500_GPIO_PORT, W5500_GPIO_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(100);
}


/**
 * @brief  Read W5500 buffer
 */
void W5500_ReadBuff(uint8_t* buff, uint16_t len)
{
    HAL_SPI_Receive(&W5500_USING_SPI, buff, len, HAL_MAX_DELAY);
}


/**
 * @brief  Write W5500 buffer
 */
void W5500_WriteBuff(uint8_t* buff, uint16_t len)
{
    HAL_SPI_Transmit(&W5500_USING_SPI, buff, len, HAL_MAX_DELAY);
}


/**
 * @brief  Write byte to W5500
 */
void W5500_WriteByte(uint8_t byte)
{
    W5500_WriteBuff( &byte, sizeof(byte) );
}


/**
 * @brief  Read byte from W5500
 */
uint8_t W5500_ReadByte(void)
{
    uint8_t byte;
    W5500_ReadBuff( &byte, sizeof(byte) );
    return byte;
}


/*
 * @brief  W5500 initialazation, set IP, MAC, NETMASK, GATEAWAY, DNS
 * @retval Initialazation result - success or not
 */
uint8_t net_init(void)
{
#ifdef DEBUG_UART_PRINT
    uart_print("Initialisation starts...");
#endif
    /* reset w5500 chip */
    W5500_reset();

#ifdef DEBUG_UART_PRINT
    uart_print("Register W5500 callbacks");
#endif
    /* register callbacks to send and receive data from w5500 */
    reg_wizchip_cs_cbfunc( W5500_select, W5500_deselect );
    reg_wizchip_spi_cbfunc( W5500_ReadByte , W5500_WriteByte );
    reg_wizchip_spiburst_cbfunc( W5500_ReadBuff, W5500_WriteBuff );

#ifdef DEBUG_UART_PRINT
    uart_print("Init wizchip");
#endif
    uint8_t W5500_fifo_size[2][8] = { {2, 2, 2, 2, 2, 2, 2, 2},
                                      {2, 2, 2, 2, 2, 2, 2, 2} };
    if ( ctlwizchip( CW_INIT_WIZCHIP , (void*)W5500_fifo_size ) == -1 )
    {
        return W5500_ERR;
    }

#ifdef DEBUG_UART_PRINT
    uart_print("Sending MAC, IP, etc...");
#endif
    for ( uint8_t i = 0; i <= 5; ++i )
    {
        g_WIZNETINFO.mac[i] = mac[i];
    }
    for ( uint8_t i = 0; i <= 3; ++i)
    {
        g_WIZNETINFO.ip[i] = ip[i];
        g_WIZNETINFO.sn[i] = netmask[i];
        g_WIZNETINFO.gw[i] = gateway[i];
        g_WIZNETINFO.dns[i] = dnsserv[i];
    }
    g_WIZNETINFO.dhcp = NETINFO_STATIC;
    wizchip_setnetinfo(&g_WIZNETINFO);

    /* check parameter set */
    wizchip_getnetinfo(&g_WIZNETINFO);
    for ( uint8_t i = 0; i <= 5; ++i )
    {
        if ( g_WIZNETINFO.mac[i] != mac[i] ) return W5500_ERR;
    }
    for ( uint8_t i = 0; i <= 3; ++i)
    {
        if ( g_WIZNETINFO.ip[i] != ip[i] ) return W5500_ERR;
        if ( g_WIZNETINFO.sn[i] != netmask[i] ) return W5500_ERR;
        if ( g_WIZNETINFO.gw[i] != gateway[i] ) return W5500_ERR;
        if ( g_WIZNETINFO.dns[i] != dnsserv[i] ) return W5500_ERR;
    }

    /* Ѕыло много экспериментов и дебага по поводу того, что бит PHYLINK не устанавливалс€.
     * Ѕыло вы€снено, что цикл ожидани€ выставлени€ этого бита необходим, чтобы гарантировать
     * наличие физического подключени€ */
    uint8_t phycfgr_stat;
    do
    {
        /* check  */
        if( ctlwizchip( CW_GET_PHYLINK, (void*) &phycfgr_stat ) == -1 )
#ifdef DEBUG_UART_PRINT
        uart_print("Error. Exit...");
#endif
    }
    while(phycfgr_stat == PHY_LINK_OFF);

    return W5500_OK;
}


/**
 * @brief  Open TCP socket, prepare to listen
 * @retval Initialazation result - success or not
 */
uint8_t net_tcp_serv_open(uint8_t socket_number, uint8_t addr[4], uint32_t port)
{
#ifdef DEBUG_UART_PRINT
        uart_print("Set IO mode");
#endif
    /* Set non-block io mode */
    uint8_t io_mode = SOCK_IO_NONBLOCK;
    if ( ctlsocket(socket_number, CS_SET_IOMODE, &io_mode ) == SOCKERR_ARG )
        return W5500_ERR;

#ifdef DEBUG_UART_PRINT
        uart_print("Set TCP mode and open socket");
#endif
    /* Open TCP socket */
    uint8_t retval = socket( socket_number, Sn_MR_TCP, port, 0);
    if ( retval != socket_number ) return W5500_ERR;
    /* Wait for status set */
    do
    {
#ifdef DEBUG_UART_PRINT
        uart_print("Sn_SR = %x", getSn_SR(socket_number) );
#endif
        HAL_Delay(10);
    }
    while ( getSn_SR(socket_number) != SOCK_INIT );

    retval = listen(socket_number);
    if ( retval != SOCK_OK ) return W5500_ERR;
    /* Wait for status set */
    do
    {
#ifdef DEBUG_UART_PRINT
        uart_print("Sn_SR = %x", getSn_SR(socket_number) );
#endif
        HAL_Delay(10);
    }
    while ( getSn_SR(socket_number) != SOCK_LISTEN ) ;

#ifdef DEBUG_UART_PRINT
        uart_print("Listening...");
#endif

    return W5500_OK;
}


/**
 * @brief  Transmit data to client and receive data
 * @retval TX/RX result - success or not
 */
uint8_t net_tcp_serv_tx_rx(uint8_t socket_number, char *p_tx_data, uint8_t *p_rx_data)
{
    /* Wait for connection */
    while ( getSn_SR(socket_number) != SOCK_ESTABLISHED ) ;

#ifdef DEBUG_UART_PRINT
    uart_print("Client accepted");
#endif

    /* Get remote information */
    uint8_t remoteIP[4];
    uint16_t remotePort;
    getsockopt(socket_number, SO_DESTIP, remoteIP);
    getsockopt(socket_number, SO_DESTPORT, &remotePort);
#ifdef DEBUG_UART_PRINT
    uart_print("remote IP[PORT] : %03d.%03d.%03d.%03d[%05d]\n",
                remoteIP[0], remoteIP[1], remoteIP[2], remoteIP[3], remotePort);
#endif

    /* Transmit data */
    if ( net_tcp_serv_tx(socket_number, p_tx_data) != W5500_OK )
        return W5500_ERR;

    /* Receive data */
    uint8_t buff[1024];
    while (1)
    {
        if ( net_tcp_serv_rx(socket_number, p_rx_data) != W5500_OK )
            return W5500_ERR;
        /* If data = "0" then quit */
        if ( buff[0] == 0x30 ||
             buff[1] == 0x00 )
            break;
    }

    return W5500_OK;
}


/**
 * @brief  Receive data from W5500 and put it in buffer; before using you need to create buffer variable
 * @retval Result - success or not
 */
uint8_t net_tcp_serv_rx(uint8_t socket_number, uint8_t *buff)
{
#ifdef DEBUG_UART_PRINT
    uart_print("Enter data in putty console");
#endif

    /* Wait for data receive */
    while ( !(getSn_IR(socket_number) & Sn_IR_RECV) ) ;
    /* Clear interrupt bit */
    setSn_IR(socket_number, Sn_IR_RECV);
    /* Wait for cleaning interrupt bit */
    while ( (getSn_IR(socket_number) & Sn_IR_RECV) ) ;

    /* Read data from W5500 buffer */
    uint32_t ret = 0;
    ret = recv(socket_number, buff, 1024);

#ifdef DEBUG_UART_PRINT
    uart_print("Data received");
#endif

    /* Check if data is right */
    if ( ret < 0 )
    {
#ifdef DEBUG_UART_PRINT
        uart_print("Error. Close socket.");
#endif
        close(socket_number);
        return W5500_ERR;
    }
    else if (ret >= 0)
    {
        buff[ret]  = 0x00;
#ifdef DEBUG_UART_PRINT
        uart_print("Recieved data[%d] = %s", ret, (char*)buff);
#endif
    }
    return W5500_OK;
}


/**
 * @brief  Transmit data to W5500
 * @retval Result - success or not
 */
uint8_t net_tcp_serv_tx(uint8_t socket_number, char *buff)
{
    uint32_t ret = 0;
    ret = send(socket_number, (uint8_t*)buff, sizeof(buff));
    if (ret < 0)
    {
#ifdef DEBUG_UART_PRINT
        uart_print("Send failed{%ld}", ret);
#endif
        close(socket_number);
        return W5500_ERR;
    }
    else
    {
        // do smth, if sending success
        return W5500_OK;
    }
}


/**
 * @brief  Close socket
 * @retval Result - success or not
 */
uint8_t net_tcp_serv_close(uint8_t socket_number)
{
    /* Close opened socket */
    if ( close(socket_number) != SOCK_OK ) return W5500_ERR;
#ifdef DEBUG_UART_PRINT
    uart_print("Socket closed");
#endif

    return W5500_OK;
}

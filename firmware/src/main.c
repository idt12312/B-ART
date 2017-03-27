/*
 * main.c
 *
 *  Created on: 2017/01/25
 *      Author: idt12312
 */

#include <stdint.h>
#include <string.h>
#include "app_timer.h"
#include "app_uart.h"
#include "nrf_gpio.h"

#include "SEGGER_RTT.h"
#include "hardware_conf.h"
#include "led.h"


#ifdef DEBUG
#define DBG(...) printf("" __VA_ARGS__)
#else
#define DBG(...)
#endif


#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */


extern void server_main(uint8_t device_id);
extern void client_main(uint8_t device_id);
extern void server_uart_event_handle(app_uart_evt_t * p_event);
extern void client_uart_event_handle(app_uart_evt_t * p_event);

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
	error_info_t *error_info = (error_info_t*)info;
	DBG("[APP_ERROR] code %u at %s:%u\n",
			error_info->err_code,
			error_info->p_file_name,
			error_info->line_num);
}


void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


static uint8_t get_device_id()
{
	uint8_t device_id = 0;
	nrf_gpio_cfg_input(DEVICEID_BIT0_PIN_NUMBER, NRF_GPIO_PIN_PULLUP);
	nrf_gpio_cfg_input(DEVICEID_BIT1_PIN_NUMBER, NRF_GPIO_PIN_PULLUP);
	nrf_gpio_cfg_input(DEVICEID_BIT2_PIN_NUMBER, NRF_GPIO_PIN_PULLUP);

	if (!nrf_gpio_pin_read(DEVICEID_BIT0_PIN_NUMBER)) device_id |= 0x01;
	if (!nrf_gpio_pin_read(DEVICEID_BIT1_PIN_NUMBER)) device_id |= 0x02;
	if (!nrf_gpio_pin_read(DEVICEID_BIT2_PIN_NUMBER)) device_id |= 0x04;

	return device_id;
}


static uint8_t get_mode()
{
	nrf_gpio_cfg_input(DEVICE_MODE_PIN_NUMBER, NRF_GPIO_PIN_PULLUP);

	if (!nrf_gpio_pin_read(DEVICE_MODE_PIN_NUMBER)) return 1;
	else return 0;
}


static void uart_init(app_uart_event_handler_t uart_event_handler)
{
	uint32_t                     err_code;
	const app_uart_comm_params_t comm_params =
	{
			RX_PIN_NUMBER,
			TX_PIN_NUMBER,
			RTS_PIN_NUMBER,
			CTS_PIN_NUMBER,
			APP_UART_FLOW_CONTROL_DISABLED,
			false,
			UART_BAUDRATE_BAUDRATE_Baud115200
	};

	APP_UART_FIFO_INIT( &comm_params,
			UART_RX_BUF_SIZE,
			UART_TX_BUF_SIZE,
			uart_event_handler,
			APP_IRQ_PRIORITY_LOW,
			err_code);
	APP_ERROR_CHECK(err_code);
}


int main()
{
    // Initialize.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
    SEGGER_RTT_Init();
    led_init();

    DBG("\nB-ART initializing...\n");


    uint8_t device_id = get_device_id();
    if (get_mode() == 1) {
    	uart_init(server_uart_event_handle);
    	server_main(device_id);
    }
    else {
    	uart_init(client_uart_event_handle);
    	client_main(device_id);
    }


	return 0;
}

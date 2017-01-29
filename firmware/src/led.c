/*
 * led.c
 *
 *  Created on: 2016/11/29
 *      Author: idt12312
 */

#include "led.h"
#include <stdbool.h>
#include "nrf_gpio.h"
#include "app_error.h"
#include "app_timer.h"
#include "hardware_conf.h"


#define LED_ON_TIME	 20
#define LED_OFF_TIME 180

APP_TIMER_DEF(m_tx_led_timer_id);
APP_TIMER_DEF(m_rx_led_timer_id);

static bool tx_led_runnig;
static bool rx_led_runnig;
static bool tx_led_state;
static bool rx_led_state;


static void tx_led_blink_handler()
{
	uint32_t err_code;

	if (tx_led_state) {
		err_code = app_timer_start(m_tx_led_timer_id, APP_TIMER_TICKS(LED_OFF_TIME, APP_TIMER_PRESCALER), NULL);
		APP_ERROR_CHECK(err_code);

		led_off(TX_LED);
		tx_led_state = false;
	}
	else {
		err_code = app_timer_stop(m_tx_led_timer_id);
		APP_ERROR_CHECK(err_code);

		tx_led_runnig = false;
	}
}


static void rx_led_blink_handler()
{
	uint32_t err_code;

	if (rx_led_state) {
		err_code = app_timer_start(m_rx_led_timer_id, APP_TIMER_TICKS(LED_OFF_TIME, APP_TIMER_PRESCALER), NULL);
		APP_ERROR_CHECK(err_code);

		led_off(RX_LED);
		rx_led_state = false;
	}
	else {
		err_code = app_timer_stop(m_rx_led_timer_id);
		APP_ERROR_CHECK(err_code);

		rx_led_runnig = false;
	}
}


void led_init()
{
	uint32_t err_code;
	nrf_gpio_cfg_output(TX_LED_PIN_NUMBER);
	nrf_gpio_cfg_output(RX_LED_PIN_NUMBER);

	nrf_gpio_pin_set(TX_LED_PIN_NUMBER);
	nrf_gpio_pin_set(RX_LED_PIN_NUMBER);

	err_code = app_timer_create(&m_tx_led_timer_id, APP_TIMER_MODE_SINGLE_SHOT, tx_led_blink_handler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&m_rx_led_timer_id, APP_TIMER_MODE_SINGLE_SHOT, rx_led_blink_handler);
	APP_ERROR_CHECK(err_code);

	tx_led_runnig = false;
	rx_led_runnig = false;

	tx_led_state = false;
	rx_led_state = false;
}


void led_on(LED_t led)
{
	uint32_t err_code;

	if (led == TX_LED) {
		err_code = app_timer_stop(m_tx_led_timer_id);
		APP_ERROR_CHECK(err_code);

		nrf_gpio_pin_clear(TX_LED_PIN_NUMBER);
	}
	else {
		err_code = app_timer_stop(m_rx_led_timer_id);
		APP_ERROR_CHECK(err_code);

		nrf_gpio_pin_clear(RX_LED_PIN_NUMBER);
	}
}


void led_off(LED_t led)
{
	uint32_t err_code;

	if (led == TX_LED) {
		err_code = app_timer_stop(m_tx_led_timer_id);
		APP_ERROR_CHECK(err_code);

		nrf_gpio_pin_set(TX_LED_PIN_NUMBER);
	}
	else {
		err_code = app_timer_stop(m_rx_led_timer_id);
		APP_ERROR_CHECK(err_code);

		nrf_gpio_pin_set(RX_LED_PIN_NUMBER);
	}
}


void led_blink(LED_t led)
{
	uint32_t err_code;


	if (led == TX_LED) {
		if (tx_led_runnig) return;

		err_code = app_timer_start(m_tx_led_timer_id, APP_TIMER_TICKS(LED_ON_TIME, APP_TIMER_PRESCALER), NULL);
		APP_ERROR_CHECK(err_code);

		tx_led_state = true;
		tx_led_runnig = true;
	}
	else {
		if (rx_led_runnig) return;

		err_code = app_timer_start(m_rx_led_timer_id, APP_TIMER_TICKS(LED_ON_TIME, APP_TIMER_PRESCALER), NULL);
		APP_ERROR_CHECK(err_code);

		rx_led_state = true;
		rx_led_runnig = true;
	}

	led_on(led);
}


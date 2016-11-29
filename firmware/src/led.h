/*
 * led.h
 *
 *  Created on: 2016/11/29
 *      Author: idt12312
 */

#ifndef SRC_LED_H_
#define SRC_LED_H_

typedef enum {
	TX_LED, // green
	RX_LED  // red
}LED_t;

void led_init();
void led_on(LED_t led);
void led_off(LED_t led);
void led_blink(LED_t led);


#endif /* SRC_LED_H_ */

/*
 * hardware_conf.h
 *
 *  Created on: 2016/11/29
 *      Author: idt12312
 */

#ifndef SRC_HARDWARE_CONF_H_
#define SRC_HARDWARE_CONF_H_

#define TX_LED_PIN_NUMBER	21
#define RX_LED_PIN_NUMBER	19

#define	TX_PIN_NUMBER	12
#define	RX_PIN_NUMBER	11
#define	RTS_PIN_NUMBER	0
#define	CTS_PIN_NUMBER	0

#define DEVICEID_JUMPER1_PIN_NUMBER 3
#define DEVICEID_JUMPER2_PIN_NUMBER 1
#define DEVICEID_JUMPER3_PIN_NUMBER 2
#define DEVICEID_JUMPER4_PIN_NUMBER 2


#define UART_TX_BUF_SIZE                512                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                512                                         /**< UART RX buffer size. */


/**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_PRESCALER 0

#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */


#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_RC,            \
                                 .rc_ctiv       = 32,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_250_PPM}



#endif /* SRC_HARDWARE_CONF_H_ */

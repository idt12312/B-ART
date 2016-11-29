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

/**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_PRESCALER 0


#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_RC,            \
                                 .rc_ctiv       = 32,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_250_PPM}



#endif /* SRC_HARDWARE_CONF_H_ */

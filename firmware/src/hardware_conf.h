/*
 * hardware_conf.h
 *
 *  Created on: 2016/11/29
 *      Author: idt12312
 */

#ifndef SRC_HARDWARE_CONF_H_
#define SRC_HARDWARE_CONF_H_



#define	RX_PIN_NUMBER	23
#define	TX_PIN_NUMBER	21
#define	RTS_PIN_NUMBER	0
#define	CTS_PIN_NUMBER	0



#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_RC,            \
                                 .rc_ctiv       = 32,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_250_PPM}



#endif /* SRC_HARDWARE_CONF_H_ */

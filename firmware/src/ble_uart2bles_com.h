/*
 * ble_uart2bles_com.h
 *
 *  Created on: 2017/01/25
 *      Author: idt12312
 */

#ifndef SRC_BLE_UART2BLES_COM_H_
#define SRC_BLE_UART2BLES_COM_H_


#define UART2BLES_BASE_UUID                  {{0x83, 0xE9, 0xE7, 0x69, 0xA2, 0x6C, 0xBE, 0xAD, 0x71, 0x4C, 0x04, 0xDA, 0x00, 0x00, 0x00, 0x20}} /**< Used vendor specific UUID. */

#define BLE_UUID_UART2BLES_SERVICE			0x0001                      /**< The UUID of the Nordic UART Service. */
#define BLE_UUID_UART2BLES_TX_CHARACTERISTIC 0x0002                      /**< The UUID of the TX Characteristic. */
#define BLE_UUID_UART2BLES_RX_CHARACTERISTIC 0x0003                      /**< The UUID of the RX Characteristic. */

#define BLE_UART2BLES_MAX_DATA_LEN (GATT_MTU_SIZE_DEFAULT - 3) /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */

#define BLE_UART2BLES_MAX_RX_CHAR_LEN        BLE_UART2BLES_MAX_DATA_LEN        /**< Maximum length of the RX Characteristic (in bytes). */
#define BLE_UART2BLES_MAX_TX_CHAR_LEN        BLE_UART2BLES_MAX_DATA_LEN        /**< Maximum length of the TX Characteristic (in bytes). */




#endif /* SRC_BLE_UART2BLES_COM_H_ */

/*
 * ble_uart2bles.h
 *
 *  Created on: 2016/11/28
 *      Author: idt12312
 */

#ifndef SRC_BLE_UART2BLES_H_
#define SRC_BLE_UART2BLES_H_

#include "ble.h"
#include "ble_srv_common.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_UUID_UART2BLE_SERVICE 0x0001                      /**< The UUID of the Nordic UART Service. */
#define BLE_UART2BLE_MAX_DATA_LEN (GATT_MTU_SIZE_DEFAULT - 3) /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */


typedef struct ble_uart2bles_s ble_uart2bles_t;

typedef void (*ble_uart2bles_data_handler_t) (ble_uart2bles_t * p_uart2bles, uint8_t * p_data, uint16_t length);


typedef struct
{
	ble_uart2bles_data_handler_t data_handler;
	uint8_t device_id;
} ble_uart2bles_init_t;


struct ble_uart2bles_s
{
    uint8_t                  uuid_type;               /**< UUID type for Nordic UART Service Base UUID. */
    uint16_t                 service_handle;          /**< Handle of Nordic UART Service (as provided by the SoftDevice). */
    ble_gatts_char_handles_t tx_handles;              /**< Handles related to the TX characteristic (as provided by the SoftDevice). */
    ble_gatts_char_handles_t rx_handles;              /**< Handles related to the RX characteristic (as provided by the SoftDevice). */
    uint16_t                 conn_handle;             /**< Handle of the current connection (as provided by the SoftDevice). BLE_CONN_HANDLE_INVALID if not in a connection. */
    bool                     is_notification_enabled; /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
    ble_uart2bles_data_handler_t   data_handler;            /**< Event handler to be called for handling received data. */
};


uint32_t ble_uart2bles_init(ble_uart2bles_t * p_uart2bles, const ble_uart2bles_init_t * p_uart2bles_init);


void ble_uart2bles_on_ble_evt(ble_uart2bles_t * p_uart2bles, ble_evt_t * p_ble_evt);


uint32_t ble_uart2bles_send(ble_uart2bles_t * p_uart2bles, uint8_t * p_data, uint16_t length);


#ifdef __cplusplus
}
#endif


#endif /* SRC_BLE_UART2BLES_H_ */

/*
 * ble_barts.h
 *
 *  Created on: 2016/11/28
 *      Author: idt12312
 */

#ifndef SRC_BLE_BARTS_H_
#define SRC_BLE_BARTS_H_

#include "ble.h"
#include "ble_srv_common.h"
#include <stdint.h>
#include <stdbool.h>

#include "ble_barts_com.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct ble_barts_s ble_barts_t;

typedef void (*ble_barts_data_handler_t) (ble_barts_t * p_barts, uint8_t * p_data, uint16_t length);


typedef struct
{
	ble_barts_data_handler_t data_handler;
	uint8_t device_id;
} ble_barts_init_t;


struct ble_barts_s
{
    uint8_t                  uuid_type;               /**< UUID type for Nordic UART Service Base UUID. */
    uint16_t                 service_handle;          /**< Handle of Nordic UART Service (as provided by the SoftDevice). */
    ble_gatts_char_handles_t tx_handles;              /**< Handles related to the TX characteristic (as provided by the SoftDevice). */
    ble_gatts_char_handles_t rx_handles;              /**< Handles related to the RX characteristic (as provided by the SoftDevice). */
    uint16_t                 conn_handle;             /**< Handle of the current connection (as provided by the SoftDevice). BLE_CONN_HANDLE_INVALID if not in a connection. */
    bool                     is_notification_enabled; /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
    ble_barts_data_handler_t   data_handler;            /**< Event handler to be called for handling received data. */
};


uint32_t ble_barts_init(ble_barts_t * p_barts, const ble_barts_init_t * p_barts_init);


void ble_barts_on_ble_evt(ble_barts_t * p_barts, ble_evt_t * p_ble_evt);


uint32_t ble_barts_send(ble_barts_t * p_barts, uint8_t * p_data, uint16_t length);


#ifdef __cplusplus
}
#endif


#endif /* SRC_BLE_BARTS_H_ */

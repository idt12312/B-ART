/*
 * ble_barts_c.h
 *
 *  Created on: 2017/01/25
 *      Author: idt12312
 */

#ifndef SRC_BLE_BARTS_C_H_
#define SRC_BLE_BARTS_C_H_



#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_gatt.h"
#include "ble_db_discovery.h"

#include "ble_barts_com.h"

typedef enum
{
    BLE_BARTS_C_EVT_DISCOVERY_COMPLETE = 1, /**< Event indicating that the BARTS service and its characteristics was found. */
    BLE_BARTS_C_EVT_BARTS_RX_EVT,             /**< Event indicating that the central has received something from a peer. */
    BLE_BARTS_C_EVT_DISCONNECTED            /**< Event indicating that the BARTS server has disconnected. */
} ble_barts_c_evt_type_t;


typedef struct {
    uint16_t                barts_rx_handle;      /**< Handle of the BARTS RX characteristic as provided by a discovery. */
    uint16_t                barts_rx_cccd_handle; /**< Handle of the CCCD of the BARTS RX characteristic as provided by a discovery. */
    uint16_t                barts_tx_handle;      /**< Handle of the BARTS TX characteristic as provided by a discovery. */
} ble_barts_c_handles_t;


typedef struct {
    ble_barts_c_evt_type_t evt_type;
    uint16_t             conn_handle;
    uint8_t            * p_data;
    uint8_t              data_len;
    ble_barts_c_handles_t  handles;     /**< Handles on which the Nordic Uart service characteristics was discovered on the peer device. This will be filled if the evt_type is @ref BLE_BARTS_C_EVT_DISCOVERY_COMPLETE.*/
} ble_barts_c_evt_t;

typedef struct ble_barts_c_s ble_barts_c_t;

typedef void (* ble_barts_c_evt_handler_t)(ble_barts_c_t * p_ble_barts_c, const ble_barts_c_evt_t * p_evt);

struct ble_barts_c_s
{
    uint8_t                 uuid_type;          /**< UUID type. */
    uint16_t                conn_handle;        /**< Handle of the current connection. Set with @ref ble_barts_c_handles_assign when connected. */
    ble_barts_c_handles_t     handles;            /**< Handles on the connected peer device needed to interact with it. */
    ble_barts_c_evt_handler_t evt_handler;        /**< Application event handler to be called when there is an event related to the BARTS. */
};

typedef struct {
    ble_barts_c_evt_handler_t evt_handler;
	uint8_t device_id;
} ble_barts_c_init_t;


uint32_t ble_barts_c_init(ble_barts_c_t * p_ble_barts_c, ble_barts_c_init_t * p_ble_barts_c_init);

void ble_barts_c_on_db_disc_evt(ble_barts_c_t * p_ble_barts_c, ble_db_discovery_evt_t * p_evt);

void ble_barts_c_on_ble_evt(ble_barts_c_t * p_ble_barts_c, const ble_evt_t * p_ble_evt);

uint32_t ble_barts_c_rx_notif_enable(ble_barts_c_t * p_ble_barts_c);

uint32_t ble_barts_c_string_send(ble_barts_c_t * p_ble_barts_c, uint8_t * p_string, uint16_t length);

uint32_t ble_barts_c_handles_assign(ble_barts_c_t * p_ble_barts_c, const uint16_t conn_handle, const ble_barts_c_handles_t * p_peer_handles);




#endif /* SRC_BLE_BARTS_C_H_ */

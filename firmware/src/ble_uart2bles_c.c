/*
 * ble_uart2bles_c.c
 *
 *  Created on: 2017/01/25
 *      Author: idt12312
 */


#include <stdlib.h>

#include "ble.h"
#include "ble_uart2bles_c.h"
#include "ble_gattc.h"
#include "ble_srv_common.h"
#include "app_error.h"
#include "sdk_common.h"


static void on_hvx(ble_uart2bles_c_t * p_ble_uart2bles_c, const ble_evt_t * p_ble_evt)
{
    // HVX can only occur from client sending.
    if ( (p_ble_uart2bles_c->handles.uart2bles_rx_handle != BLE_GATT_HANDLE_INVALID)
            && (p_ble_evt->evt.gattc_evt.params.hvx.handle == p_ble_uart2bles_c->handles.uart2bles_rx_handle)
            && (p_ble_uart2bles_c->evt_handler != NULL)
        )
    {
        ble_uart2bles_c_evt_t ble_uart2bles_c_evt;

        ble_uart2bles_c_evt.evt_type = BLE_UART2BLES_C_EVT_UART2BLES_RX_EVT;
        ble_uart2bles_c_evt.p_data   = (uint8_t *)p_ble_evt->evt.gattc_evt.params.hvx.data;
        ble_uart2bles_c_evt.data_len = p_ble_evt->evt.gattc_evt.params.hvx.len;

        p_ble_uart2bles_c->evt_handler(p_ble_uart2bles_c, &ble_uart2bles_c_evt);
    }
}


static uint32_t cccd_configure(uint16_t conn_handle, uint16_t cccd_handle, bool enable)
{
    uint8_t buf[BLE_CCCD_VALUE_LEN];

    buf[0] = enable ? BLE_GATT_HVX_NOTIFICATION : 0;
    buf[1] = 0;

    const ble_gattc_write_params_t write_params = {
        .write_op = BLE_GATT_OP_WRITE_REQ,
        .flags    = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_WRITE,
        .handle   = cccd_handle,
        .offset   = 0,
        .len      = sizeof(buf),
        .p_value  = buf
    };

    return sd_ble_gattc_write(conn_handle, &write_params);
}


void ble_uart2bles_c_on_db_disc_evt(ble_uart2bles_c_t * p_ble_uart2bles_c, ble_db_discovery_evt_t * p_evt)
{
    ble_uart2bles_c_evt_t uart2bles_c_evt;
    memset(&uart2bles_c_evt,0,sizeof(ble_uart2bles_c_evt_t));

    ble_gatt_db_char_t * p_chars = p_evt->params.discovered_db.charateristics;

    // Check if the UART2BLES was discovered.
    if (p_evt->evt_type == BLE_DB_DISCOVERY_COMPLETE &&
        p_evt->params.discovered_db.srv_uuid.uuid == BLE_UUID_UART2BLES_SERVICE &&
        p_evt->params.discovered_db.srv_uuid.type == p_ble_uart2bles_c->uuid_type) {

        for (int i=0; i<p_evt->params.discovered_db.char_count; i++) {
            switch (p_chars[i].characteristic.uuid.uuid) {
                case BLE_UUID_UART2BLES_TX_CHARACTERISTIC:
                    uart2bles_c_evt.handles.uart2bles_tx_handle = p_chars[i].characteristic.handle_value;
                    break;

                case BLE_UUID_UART2BLES_RX_CHARACTERISTIC:
                    uart2bles_c_evt.handles.uart2bles_rx_handle = p_chars[i].characteristic.handle_value;
                    uart2bles_c_evt.handles.uart2bles_rx_cccd_handle = p_chars[i].cccd_handle;
                    break;

                default:
                    break;
            }
        }

        if (p_ble_uart2bles_c->evt_handler != NULL) {
            uart2bles_c_evt.conn_handle = p_evt->conn_handle;
            uart2bles_c_evt.evt_type    = BLE_UART2BLES_C_EVT_DISCOVERY_COMPLETE;
            p_ble_uart2bles_c->evt_handler(p_ble_uart2bles_c, &uart2bles_c_evt);
        }
    }
}


uint32_t ble_uart2bles_c_init(ble_uart2bles_c_t * p_ble_uart2bles_c, ble_uart2bles_c_init_t * p_ble_uart2bles_c_init)
{
    uint32_t      err_code;
    ble_uuid_t    uart_uuid;
    ble_uuid128_t uart2bles_base_uuid = UART2BLES_BASE_UUID;

    // device id をUUIDに埋め込み
    uart2bles_base_uuid.uuid128[14] = p_ble_uart2bles_c_init->device_id;

    VERIFY_PARAM_NOT_NULL(p_ble_uart2bles_c);
    VERIFY_PARAM_NOT_NULL(p_ble_uart2bles_c_init);

    err_code = sd_ble_uuid_vs_add(&uart2bles_base_uuid, &p_ble_uart2bles_c->uuid_type);
    VERIFY_SUCCESS(err_code);

    uart_uuid.type = p_ble_uart2bles_c->uuid_type;
    uart_uuid.uuid = BLE_UUID_UART2BLES_SERVICE;

    p_ble_uart2bles_c->conn_handle           = BLE_CONN_HANDLE_INVALID;
    p_ble_uart2bles_c->evt_handler           = p_ble_uart2bles_c_init->evt_handler;
    p_ble_uart2bles_c->handles.uart2bles_rx_handle = BLE_GATT_HANDLE_INVALID;
    p_ble_uart2bles_c->handles.uart2bles_tx_handle = BLE_GATT_HANDLE_INVALID;

    return ble_db_discovery_evt_register(&uart_uuid);
}


void ble_uart2bles_c_on_ble_evt(ble_uart2bles_c_t * p_ble_uart2bles_c, const ble_evt_t * p_ble_evt)
{
    if ((p_ble_uart2bles_c == NULL) || (p_ble_evt == NULL)) return;

    if ( (p_ble_uart2bles_c->conn_handle != BLE_CONN_HANDLE_INVALID)
         && (p_ble_uart2bles_c->conn_handle != p_ble_evt->evt.gap_evt.conn_handle)
       ) return;

    switch (p_ble_evt->header.evt_id) {
        case BLE_GATTC_EVT_HVX:
            on_hvx(p_ble_uart2bles_c, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            if (p_ble_evt->evt.gap_evt.conn_handle == p_ble_uart2bles_c->conn_handle
                    && p_ble_uart2bles_c->evt_handler != NULL) {
                ble_uart2bles_c_evt_t uart2bles_c_evt;

                uart2bles_c_evt.evt_type = BLE_UART2BLES_C_EVT_DISCONNECTED;

                p_ble_uart2bles_c->conn_handle = BLE_CONN_HANDLE_INVALID;
                p_ble_uart2bles_c->evt_handler(p_ble_uart2bles_c, &uart2bles_c_evt);
            }
            break;
    }
}


uint32_t ble_uart2bles_c_rx_notif_enable(ble_uart2bles_c_t * p_ble_uart2bles_c)
{
    VERIFY_PARAM_NOT_NULL(p_ble_uart2bles_c);

    if ( (p_ble_uart2bles_c->conn_handle == BLE_CONN_HANDLE_INVALID)
       ||(p_ble_uart2bles_c->handles.uart2bles_rx_cccd_handle == BLE_GATT_HANDLE_INVALID)
       ) {
        return NRF_ERROR_INVALID_STATE;
    }
    return cccd_configure(p_ble_uart2bles_c->conn_handle,p_ble_uart2bles_c->handles.uart2bles_rx_cccd_handle, true);
}


uint32_t ble_uart2bles_c_send(ble_uart2bles_c_t * p_ble_uart2bles_c, uint8_t * p_data, uint16_t length)
{
    VERIFY_PARAM_NOT_NULL(p_ble_uart2bles_c);

    if (length > BLE_UART2BLES_MAX_DATA_LEN) return NRF_ERROR_INVALID_PARAM;

    if ( p_ble_uart2bles_c->conn_handle == BLE_CONN_HANDLE_INVALID) return NRF_ERROR_INVALID_STATE;

    const ble_gattc_write_params_t write_params = {
        .write_op = BLE_GATT_OP_WRITE_CMD,
        .flags    = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_WRITE,
        .handle   = p_ble_uart2bles_c->handles.uart2bles_tx_handle,
        .offset   = 0,
        .len      = length,
        .p_value  = p_data
    };

    return sd_ble_gattc_write(p_ble_uart2bles_c->conn_handle, &write_params);
}


uint32_t ble_uart2bles_c_handles_assign(ble_uart2bles_c_t * p_ble_uart2bles,
                                  const uint16_t conn_handle,
                                  const ble_uart2bles_c_handles_t * p_peer_handles)
{
    VERIFY_PARAM_NOT_NULL(p_ble_uart2bles);

    p_ble_uart2bles->conn_handle = conn_handle;
    if (p_peer_handles != NULL) {
        p_ble_uart2bles->handles.uart2bles_rx_cccd_handle = p_peer_handles->uart2bles_rx_cccd_handle;
        p_ble_uart2bles->handles.uart2bles_rx_handle      = p_peer_handles->uart2bles_rx_handle;
        p_ble_uart2bles->handles.uart2bles_tx_handle      = p_peer_handles->uart2bles_tx_handle;
    }
    return NRF_SUCCESS;
}



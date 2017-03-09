#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "nordic_common.h"
#include "app_error.h"
#include "app_uart.h"
#include "ble_db_discovery.h"
#include "app_timer.h"
#include "app_util.h"
#include "app_error.h"
#include "ble.h"
#include "ble_gap.h"
#include "ble_hci.h"
#include "softdevice_handler.h"
#include "ble_advdata.h"
#include "ble_uart2bles_c.h"

#include "hardware_conf.h"
#include "led.h"


#ifdef DEBUG
#define DBG(...) printf(__VA_ARGS__)
#else
#define DBG(...)
#endif


#define CENTRAL_LINK_COUNT      1                               /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT   0                               /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define UART2BLES_SERVICE_UUID_TYPE   BLE_UUID_TYPE_VENDOR_BEGIN      /**< UUID type for the Nordic UART Service (vendor specific). */

#define SCAN_INTERVAL           0x00A0                          /**< Determines scan interval in units of 0.625 millisecond. */
#define SCAN_WINDOW             0x0050                          /**< Determines scan window in units of 0.625 millisecond. */
#define SCAN_ACTIVE             1                               /**< If 1, performe active scanning (scan requests). */
#define SCAN_SELECTIVE          0                               /**< If 1, ignore unknown devices (non whitelisted). */
#define SCAN_TIMEOUT            0x0000                          /**< Timout when scanning. 0x0000 disables timeout. */

#define MIN_CONNECTION_INTERVAL MSEC_TO_UNITS(20, UNIT_1_25_MS) /**< Determines minimum connection interval in millisecond. */
#define MAX_CONNECTION_INTERVAL MSEC_TO_UNITS(75, UNIT_1_25_MS) /**< Determines maximum connection interval in millisecond. */
#define SLAVE_LATENCY           0                               /**< Determines slave latency in counts of connection events. */
#define SUPERVISION_TIMEOUT     MSEC_TO_UNITS(4000, UNIT_10_MS) /**< Determines supervision time-out in units of 10 millisecond. */

#define UUID16_SIZE             2                               /**< Size of 16 bit UUID */
#define UUID32_SIZE             4                               /**< Size of 32 bit UUID */
#define UUID128_SIZE            16                              /**< Size of 128 bit UUID */

static ble_uart2bles_c_t              m_ble_uart2bles_c;                    /**< Instance of UART2BLES service. Must be passed to all UART2BLES_C API calls. */
static ble_db_discovery_t       m_ble_db_discovery;             /**< Instance of database discovery module. Must be passed to all db_discovert API calls */


static const ble_gap_conn_params_t m_connection_param =
{
		(uint16_t)MIN_CONNECTION_INTERVAL,  // Minimum connection
		(uint16_t)MAX_CONNECTION_INTERVAL,  // Maximum connection
		(uint16_t)SLAVE_LATENCY,            // Slave latency
		(uint16_t)SUPERVISION_TIMEOUT       // Supervision time-out
};

static const ble_gap_scan_params_t m_scan_params =
{
		.active      = SCAN_ACTIVE,
		.selective   = SCAN_SELECTIVE,
		.p_whitelist = NULL,
		.interval    = SCAN_INTERVAL,
		.window      = SCAN_WINDOW,
		.timeout     = SCAN_TIMEOUT
};

static const ble_uuid_t m_uart2bles_uuid =
{
		.uuid = BLE_UUID_UART2BLES_SERVICE,
		.type = UART2BLES_SERVICE_UUID_TYPE
};



static void scan_start(void)
{
	uint32_t err_code;

	err_code = sd_ble_gap_scan_start(&m_scan_params);
	APP_ERROR_CHECK(err_code);
}


static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
	ble_uart2bles_c_on_db_disc_evt(&m_ble_uart2bles_c, p_evt);
}


void client_uart_event_handle(app_uart_evt_t * p_event)
{
	static uint8_t data_array[BLE_UART2BLES_MAX_DATA_LEN];
	static uint8_t index = 0;
	uint32_t       err_code;

	switch (p_event->evt_type) {
	case APP_UART_DATA_READY:
		UNUSED_VARIABLE(app_uart_get(&data_array[index]));

		// 未接続時はデータを無視
		if (m_ble_uart2bles_c.conn_handle == BLE_CONN_HANDLE_INVALID) break;

		index++;

		if ((data_array[index - 1] == '\n') || (index >= (BLE_UART2BLES_MAX_DATA_LEN))) {
			DBG("[UART receive]");
			for (int i=0;i<index;i++) {
				DBG("%02x",data_array[i]);
			}
			DBG("\n");
			err_code = ble_uart2bles_c_send(&m_ble_uart2bles_c, data_array, index);
			led_blink(TX_LED);
			if (err_code != NRF_ERROR_INVALID_STATE) {
				APP_ERROR_CHECK(err_code);
			}
			index = 0;
		}
		break;

	case APP_UART_COMMUNICATION_ERROR:
		APP_ERROR_HANDLER(p_event->data.error_communication);
		break;

	case APP_UART_FIFO_ERROR:
		APP_ERROR_HANDLER(p_event->data.error_code);
		break;

	default:
		break;
	}
}


static void ble_uart2bles_c_evt_handler(ble_uart2bles_c_t * p_ble_uart2bles_c, const ble_uart2bles_c_evt_t * p_ble_uart2bles_evt)
{
	uint32_t err_code;
	switch (p_ble_uart2bles_evt->evt_type) {
	case BLE_UART2BLES_C_EVT_DISCOVERY_COMPLETE:
		err_code = ble_uart2bles_c_handles_assign(p_ble_uart2bles_c, p_ble_uart2bles_evt->conn_handle, &p_ble_uart2bles_evt->handles);
		APP_ERROR_CHECK(err_code);

		err_code = ble_uart2bles_c_rx_notif_enable(p_ble_uart2bles_c);
		APP_ERROR_CHECK(err_code);
		DBG("The device has the UART2BLE Service\n");
		break;

	case BLE_UART2BLES_C_EVT_UART2BLES_RX_EVT:
		DBG("[UART send]");
		led_blink(RX_LED);
		for (uint32_t i = 0; i < p_ble_uart2bles_evt->data_len; i++) {
			DBG("%c", p_ble_uart2bles_evt->p_data[i]);
			while(app_uart_put( p_ble_uart2bles_evt->p_data[i]) != NRF_SUCCESS);
		}
		DBG("\n");
		break;

	case BLE_UART2BLES_C_EVT_DISCONNECTED:
		DBG("Disconnected\n");
		led_off(TX_LED);
		led_on(RX_LED);
		scan_start();
		break;
	}
}


static bool is_uuid_present(const ble_uuid_t *p_target_uuid,
		const ble_gap_evt_adv_report_t *p_adv_report)
{
	uint32_t err_code;
	uint32_t index = 0;
	uint8_t *p_data = (uint8_t *)p_adv_report->data;
	ble_uuid_t extracted_uuid;

	while (index < p_adv_report->dlen)
	{
		uint8_t field_length = p_data[index];
		uint8_t field_type   = p_data[index+1];

		if ( (field_type == BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE)
				|| (field_type == BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE)
		)
		{
			for (uint32_t u_index = 0; u_index < (field_length/UUID16_SIZE); u_index++) {
				err_code = sd_ble_uuid_decode(  UUID16_SIZE,
						&p_data[u_index * UUID16_SIZE + index + 2],
						&extracted_uuid);
				if (err_code == NRF_SUCCESS)
				{
					if ((extracted_uuid.uuid == p_target_uuid->uuid)
							&& (extracted_uuid.type == p_target_uuid->type))
					{
						return true;
					}
				}
			}
		}

		else if ( (field_type == BLE_GAP_AD_TYPE_32BIT_SERVICE_UUID_MORE_AVAILABLE)
				|| (field_type == BLE_GAP_AD_TYPE_32BIT_SERVICE_UUID_COMPLETE)
		)
		{
			for (uint32_t u_index = 0; u_index < (field_length/UUID32_SIZE); u_index++) {
				err_code = sd_ble_uuid_decode(UUID16_SIZE,
						&p_data[u_index * UUID32_SIZE + index + 2],
						&extracted_uuid);
				if (err_code == NRF_SUCCESS)
				{
					if ((extracted_uuid.uuid == p_target_uuid->uuid)
							&& (extracted_uuid.type == p_target_uuid->type))
					{
						return true;
					}
				}
			}
		}

		else if ( (field_type == BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_MORE_AVAILABLE)
				|| (field_type == BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE)
		)
		{
			err_code = sd_ble_uuid_decode(UUID128_SIZE,
					&p_data[index + 2],
					&extracted_uuid);
			if (err_code == NRF_SUCCESS)
			{
				if ((extracted_uuid.uuid == p_target_uuid->uuid)
						&& (extracted_uuid.type == p_target_uuid->type))
				{
					return true;
				}
			}
		}
		index += field_length + 1;
	}
	return false;
}


static void on_ble_evt(ble_evt_t * p_ble_evt)
{
	uint32_t              err_code;
	const ble_gap_evt_t * p_gap_evt = &p_ble_evt->evt.gap_evt;

	switch (p_ble_evt->header.evt_id) {
	case BLE_GAP_EVT_ADV_REPORT:
	{
		DBG("[BLE event] BLE_GAP_EVT_ADV_REPORT\n");

		const ble_gap_evt_adv_report_t * p_adv_report = &p_gap_evt->params.adv_report;

		if (is_uuid_present(&m_uart2bles_uuid, p_adv_report))
		{

			err_code = sd_ble_gap_connect(&p_adv_report->peer_addr,
					&m_scan_params,
					&m_connection_param);

			if (err_code == NRF_SUCCESS)
			{
				// scan is automatically stopped by the connect
				APP_ERROR_CHECK(err_code);
				DBG("Connecting to target %02x%02x%02x%02x%02x%02x\n",
						p_adv_report->peer_addr.addr[0],
						p_adv_report->peer_addr.addr[1],
						p_adv_report->peer_addr.addr[2],
						p_adv_report->peer_addr.addr[3],
						p_adv_report->peer_addr.addr[4],
						p_adv_report->peer_addr.addr[5]
				);
			}
		}
		break;
	}

	case BLE_GAP_EVT_CONNECTED:
		DBG("[BLE event] BLE_GAP_EVT_CONNECTED\n");
		led_blink(TX_LED);
		led_blink(RX_LED);

		// start discovery of services. The UART2BLES Client waits for a discovery result
		err_code = ble_db_discovery_start(&m_ble_db_discovery, p_ble_evt->evt.gap_evt.conn_handle);
		APP_ERROR_CHECK(err_code);
		break;

	case BLE_GAP_EVT_TIMEOUT:
		DBG("[BLE event] BLE_GAP_EVT_TIMEOUT\n");
		if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_SCAN) {
			DBG("Scan timed out\n");
			scan_start();
		}
		else if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN) {
			DBG("Connection Request timed out\n");
		}
		break;

	case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
		DBG("[BLE event] BLE_GAP_EVT_SEC_PARAMS_REQUEST\n");
		// Pairing not supported
		err_code = sd_ble_gap_sec_params_reply(p_ble_evt->evt.gap_evt.conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
		APP_ERROR_CHECK(err_code);
		break;

	case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
		DBG("[BLE event] BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST\n");
		// Accepting parameters requested by peer.
		err_code = sd_ble_gap_conn_param_update(p_gap_evt->conn_handle,
				&p_gap_evt->params.conn_param_update_request.conn_params);
		APP_ERROR_CHECK(err_code);
		break;

	default:
		break;
	}
}


static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
	on_ble_evt(p_ble_evt);
    ble_db_discovery_on_ble_evt(&m_ble_db_discovery, p_ble_evt);
	ble_uart2bles_c_on_ble_evt(&m_ble_uart2bles_c,p_ble_evt);
}


static void ble_stack_init(void)
{
	uint32_t err_code;

	nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

	// Initialize the SoftDevice handler module.
	SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

	ble_enable_params_t ble_enable_params;
	err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
			PERIPHERAL_LINK_COUNT,
			&ble_enable_params);
	APP_ERROR_CHECK(err_code);

	//Check the ram settings against the used number of links
	CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);

	// Enable BLE stack.
	err_code = softdevice_enable(&ble_enable_params);
	APP_ERROR_CHECK(err_code);

	// Register with the SoftDevice handler module for BLE events.
	err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
	APP_ERROR_CHECK(err_code);
}


static void services_init(uint8_t device_id)
{
	uint32_t         err_code;
	ble_uart2bles_c_init_t uart2bles_c_init_t;

	uart2bles_c_init_t.evt_handler = ble_uart2bles_c_evt_handler;

	uart2bles_c_init_t.device_id = device_id;
	DBG("[uart2bles] Device ID : %u\n", uart2bles_c_init_t.device_id);

	err_code = ble_uart2bles_c_init(&m_ble_uart2bles_c, &uart2bles_c_init_t);
	APP_ERROR_CHECK(err_code);
}


static void db_discovery_init(void)
{
	uint32_t err_code = ble_db_discovery_init(db_disc_handler);
	APP_ERROR_CHECK(err_code);
}


void client_main(uint8_t device_id)
{
	uint32_t err_code;

	db_discovery_init();
	ble_stack_init();
	services_init(device_id);

	// Start scanning for peripherals and initiate connection
	// with devices that advertise UART2BLES UUID.
	scan_start();
	DBG("Scan started\n");

	//送信強度を4dB(最大)に設定
	err_code = sd_ble_gap_tx_power_set(4);
	APP_ERROR_CHECK(err_code);

	led_off(TX_LED);
	led_on(RX_LED);

	while (1) {
		uint32_t err_code = sd_app_evt_wait();
		APP_ERROR_CHECK(err_code);
	}
}

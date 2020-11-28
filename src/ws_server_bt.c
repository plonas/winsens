/*
 * ws_broker_bt.c
 *
 *  Created on: 8 cze 2018
 *      Author: Damian.Plonek
 */

#include "ws_server_bt.h"
#include "ws_ble_wms.h"
#include "ws_ble_cs.h"
#include "ws_task_queue.h"
#include "hwal/ws_button.h"
#include "ws_configuration_write.h"
#define WS_LOG_MODULE_NAME SVBT
#include "ws_log.h"
#include "utils/utils.h"

#include "nordic_common.h"
#include "nrf.h"
#include "nrf_sdm.h"
#include "nrf_delay.h"
#include "fds.h"
#include "nrf_fstorage.h"
#include "app_timer.h"
#include "ble.h"
#include "peer_manager.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_state.h"
#include "ble_srv_common.h"
#include "ble_conn_params.h"
#include "nrf_ble_gatt.h"
#include "nrf_nvic.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"


#define WS_SUBSCRIBERS_NUMBER   1

#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                           /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)            /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds). */

#define DEVICE_NAME                     "WinSensDev"                                /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "DamianPłonek"                              /**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                128                                         /**< The advertising interval (in units of 0.625 ms. This value corresponds to 80 ms). */
#define APP_ADV_DURATION                0                                           /**< The advertising timeout in units of 10ms. */

#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */

#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */
#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */

#define WS_WHITELIST_MAX_LEN            BLE_GAP_WHITELIST_ADDR_MAX_COUNT



BLE_ADVERTISING_DEF(ws_advertising);                                 /**< Advertising module instance. */
NRF_BLE_GATT_DEF(m_gatt);


struct WS_ServerBtState_s;

typedef enum
{
    WS_SERVER_BT_STATE_UNKNOWN,
    WS_SERVER_BT_STATE_CONNECTED,
    WS_SERVER_BT_STATE_CONNECTING,
    WS_SERVER_BT_STATE_DISCONNECTED,
    WS_SERVER_BT_STATE_DISCONNECTING,
    WS_SERVER_BT_STATE_ADVERTISING,
    WS_SERVER_BT_STATE_BONDING,
    WS_SERVER_BT_STATE_UNBONDING,

} WS_ServerBtState_e;

typedef enum
{
    WS_SERVER_BT_EVENT_CONNECT,
    WS_SERVER_BT_EVENT_CONNECTED,
    WS_SERVER_BT_EVENT_DISCONNECT,
    WS_SERVER_BT_EVENT_DISCONNECTED,
    WS_SERVER_BT_EVENT_ADVERTISE,
    WS_SERVER_BT_EVENT_ADVERTISING_STARTED,
    WS_SERVER_BT_EVENT_ADVERTISING_STOPPED,

    WS_SERVER_BT_EVENT_UNBOND,
    WS_SERVER_BT_EVENT_UNBONDED,
    WS_SERVER_BT_EVENT_BOND,
    WS_SERVER_BT_EVENT_BONDED,

} WS_ServerBtEvent_e;

typedef void (*WS_ServerBtEventHandler_f)(WS_Event_t event);
typedef void (*WS_ServerBtStateEntry_f)(void);
typedef void (*WS_ServerBtStateExit_f)(void);

typedef struct WS_ServerBtState_s
{
    uint32_t                        stateId;
    WS_ServerBtEventHandler_f       eventHandler;
    WS_ServerBtStateEntry_f         entry;
    WS_ServerBtStateExit_f          exit;

} WS_ServerBtState_t;

static WINSENS_Status_e WS_ServerBtDisconnect(void);
static WINSENS_Status_e WS_ServerBtDeletePeers(void);
static void ws_ServerBtDeinit(
    WS_Server_t *server);
static void ws_ServerBtUpdateWindowState(
    WS_Server_t *server,
    WS_Window_e windowId,
    WS_WindowState_e state);
static WINSENS_Status_e ws_ServerSubscribe(
    WS_Server_t *server,
    WS_ServerCallback_f callback);
static void ws_ServerUnsubscribe(
    WS_Server_t *server,
    WS_ServerCallback_f callback);
static void ws_ServerBtReset(
    WS_Server_t *server,
    const WS_Configuration_t *config);
static ws_ble_wms_state_e ws_convertWindowState(
    WS_WindowState_e state);
static void ws_update_subscribers(WS_Window_e window, WS_ServerEvent_t event);
static void ws_on_threshold_write(WS_Window_e window, uint16_t value);
static void ws_on_enabled_write(WS_Window_e window, bool value);
static void ws_on_apply_write(void);

static void ws_timers_init(void);
static void ws_ble_stack_init(void);
static void ws_peer_manager_init(
    bool erase_bonds);
static void ws_pm_evt_handler(
    pm_evt_t const * p_evt);
static void ws_gap_params_init(void);
static void ws_advertising_init(void);
static void ws_advertising_start(
    ble_gap_addr_t* addr);
static void ws_on_adv_evt(
    ble_adv_evt_t ble_adv_evt);
static void ws_services_init(
    const WS_Configuration_t *config);
static void ws_conn_params_init(void);
static void ws_on_conn_params_evt(
    ble_conn_params_evt_t * p_evt);
static void ws_conn_params_error_handler(
    uint32_t nrf_error);
//static void ws_sleep_mode_enter(void);
static void ws_on_ble_evt(
    ble_evt_t const * p_ble_evt,
    void * p_context);
static void ws_ServerBtResetHandler(
    void *p_event_data,
    uint16_t event_size);

static bool ws_addToWhitelist(
    pm_peer_id_t peerId);
static bool ws_bond(
    uint16_t connHandle);
static void ws_removeAllBondings(void);
static bool ws_disconnect(
    uint16_t connHandle);

static void WS_EventHandler(
    WS_Event_t event);

static void WS_EmptyEventHandler(
    WS_Event_t event) {}
static void WS_EmptyOnExit(void) {}
static void WS_EmptyOnEnter(void) {}

static void WS_ConnectedEventHandler(
    WS_Event_t event);
//static void WS_ConnectingEventHandler(
//    WS_Event_t event);
static void WS_DisconnectedEventHandler(
    WS_Event_t event);
static void WS_DisconnectingEventHandler(
    WS_Event_t event);
static void WS_AdvertisingEventHandler(
    WS_Event_t event);
static void WS_BondingEventHandler(
    WS_Event_t event);
static void WS_UnbondingEventHandler(
    WS_Event_t event);

//static void WS_HandleStateEvent(
//    WS_Event_t event);
static void WS_ChangeState(
    const WS_ServerBtState_t *newState);
static void WS_ConnectedOnExit(void);
//static void WS_ConnectingOnExit(void);
static void WS_DisconnectedOnExit(void);
static void WS_DisconnectingOnExit(void);
static void WS_AdvertisingOnExit(void);
static void WS_BondingOnExit(void);
static void WS_UnbondingOnExit(void);
static void WS_ConnectedOnEnter(void);
//static void WS_ConnectingOnEnter(void);
static void WS_DisconnectedOnEnter(void);
static void WS_DisconnectingOnEnter(void);
static void WS_AdvertisingOnEnter(void);
static void WS_BondingOnEnter(void);
static void WS_UnbondingOnEnter(void);

static char const * WS_ConvertState(
    WS_ServerBtState_e e);

const static WS_ServerBtState_t unknownConnState = { WS_SERVER_BT_STATE_UNKNOWN, WS_EmptyEventHandler, WS_EmptyOnEnter, WS_EmptyOnExit };
const static WS_ServerBtState_t connectedState = { WS_SERVER_BT_STATE_CONNECTED, WS_ConnectedEventHandler, WS_ConnectedOnEnter, WS_ConnectedOnExit };
//const static WS_ServerBtState_t connectingState = { WS_SERVER_BT_STATE_CONNECTING, WS_ConnectingEventHandler, WS_ConnectingOnEnter, WS_ConnectingOnExit };
const static WS_ServerBtState_t disconnectedState = { WS_SERVER_BT_STATE_DISCONNECTED, WS_DisconnectedEventHandler, WS_DisconnectedOnEnter, WS_DisconnectedOnExit };
const static WS_ServerBtState_t disconnectingState = { WS_SERVER_BT_STATE_DISCONNECTING, WS_DisconnectingEventHandler, WS_DisconnectingOnEnter, WS_DisconnectingOnExit };
const static WS_ServerBtState_t advertisingState = { WS_SERVER_BT_STATE_ADVERTISING, WS_AdvertisingEventHandler, WS_AdvertisingOnEnter, WS_AdvertisingOnExit };
const static WS_ServerBtState_t bondingState = { WS_SERVER_BT_STATE_BONDING, WS_BondingEventHandler, WS_BondingOnEnter, WS_BondingOnExit };
const static WS_ServerBtState_t unbondingState = { WS_SERVER_BT_STATE_UNBONDING, WS_UnbondingEventHandler, WS_UnbondingOnEnter, WS_UnbondingOnExit };

static ble_uuid_t ws_adv_uuids[] = {{BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}}; /**< Universally unique service identifiers. */
//static ble_uuid_t ws_sr_uuids[] = {{BLE_UUID_WMS_SERVICE_UUID, BLE_UUID_TYPE_VENDOR_BEGIN}}; /**< Universally unique service identifiers. */

static uint16_t ws_conn_handle = BLE_CONN_HANDLE_INVALID;                           /**< Handle of the current connection. */
static pm_peer_id_t ws_peer_id = PM_PEER_ID_INVALID;
static bool ws_connectable = false;
static ws_ble_wms_t ws_wms[WS_WINDOWS_NUMBER] = {WS_BLE_WMS_INIT};
static ws_ble_cs_t ws_cs;
static WS_ServerCallback_f ws_callbacks[WS_WINDOWS_NUMBER] = {NULL};
static WS_Configuration_t ws_config = { 0 };
static const WS_ServerBtState_t *ws_currentState = &unknownConnState;
static pm_peer_id_t ws_whitelist[WS_WHITELIST_MAX_LEN] = {PM_PEER_ID_INVALID};


/**@brief GATT module event handler.
 */
static void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    if (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)
    {
        NRF_LOG_INFO("GATT ATT MTU on connection 0x%x changed to %d.",
                     p_evt->conn_handle,
                     p_evt->params.att_mtu_effective);
    }
}

WINSENS_Status_e WS_ServerBtInit(
    WS_Server_t *server,
    const WS_Configuration_t *config)
{
    uint32_t i = 0;
    ret_code_t err_code;

    for (i = 0; i < WS_WINDOWS_NUMBER; ++i)
    {
        ws_wms[i] = (ws_ble_wms_t)WS_BLE_WMS_INIT;
    }
    memset(ws_callbacks, 0, sizeof(WS_ServerCallback_f) * WS_SUBSCRIBERS_NUMBER);

    if (config)
    {
        ws_config = *config;
    }

    ws_conn_handle = BLE_CONN_HANDLE_INVALID;
    ws_connectable = false;

    ws_timers_init();
    ws_ble_stack_init();
    ws_gap_params_init();
    err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    WS_APP_ERROR(err_code);

    ws_conn_params_init();
    ws_peer_manager_init(!config->bonded);
    err_code = pm_register(ws_pm_evt_handler);
    WS_LOG_DEBUG("pm_register: %lu\r\n", err_code);
    WS_APP_ERROR(err_code);

    ws_services_init(&ws_config);
    ws_advertising_init();

    server->updateWindowState = ws_ServerBtUpdateWindowState;
    server->subscribe = ws_ServerSubscribe;
    server->unsubscribe = ws_ServerUnsubscribe;
    server->reset = ws_ServerBtReset;
    server->disconnect = WS_ServerBtDisconnect;
    server->deletePeers = WS_ServerBtDeletePeers;
    server->deinit = ws_ServerBtDeinit;

    WINSENS_Status_e status = WS_ButtonRegisterCallback(WS_DIGITAL_INPUT_PAIR_BTN, WS_EventHandler);
    WS_LOG_ERROR_CHECK(status);

    WS_ChangeState(&advertisingState);

    return status;
}

static WINSENS_Status_e ws_ServerSubscribe(
    WS_Server_t *server,
    WS_ServerCallback_f callback)
{
    uint8_t i;

    for (i = 0; i < WS_SUBSCRIBERS_NUMBER; ++i)
    {
        if (NULL == ws_callbacks[i])
        {
            ws_callbacks[i] = callback;
            return WINSENS_OK;
        }
    }

    return WINSENS_NO_RESOURCES;
}

static void ws_ServerUnsubscribe(
    WS_Server_t *server,
    WS_ServerCallback_f callback)
{
    uint8_t i;

    for (i = 0; i < WS_SUBSCRIBERS_NUMBER; ++i)
    {
        if (callback == ws_callbacks[i])
        {
            ws_callbacks[i] = NULL;
            break;
        }
    }
}

static void ws_ServerBtReset(
    WS_Server_t *server,
    const WS_Configuration_t *config)
{
    WINSENS_Status_e status = WINSENS_ERROR;

    WS_LOG_DEBUG("ws_ServerBtReset\r\n");

    status = WS_TaskQueueAdd(NULL, 0, ws_ServerBtResetHandler);
    if (WINSENS_OK != status)
    {
        WS_LOG_ERROR("WS_TaskQueueAdd failed\r\n");
    }
}

static WINSENS_Status_e WS_ServerBtDisconnect(void)
{
    WS_LOG_DEBUG("WS_ServerBtDisconnect\r\n");

    WS_ChangeState(&disconnectingState);

    return WINSENS_OK;
}

static WINSENS_Status_e WS_ServerBtDeletePeers(void)
{
    WS_LOG_DEBUG("WS_ServerBtDeletePeers\r\n");

    WS_ChangeState(&unbondingState);
    return WINSENS_OK;
}

static void ws_ServerBtDeinit(
    WS_Server_t *server)
{
    WS_DigitalInputUnregisterCallback(WS_DIGITAL_INPUT_PAIR_BTN);

    server->updateWindowState = NULL;
    server->subscribe = NULL;
    server->unsubscribe = NULL;
    server->deinit = NULL;
}

static void ws_ServerBtUpdateWindowState(
    WS_Server_t *server,
    WS_Window_e windowId,
    WS_WindowState_e state)
{
    if (WS_SERVER_BT_STATE_CONNECTED != ws_currentState->stateId)
    {
        return;
    }

    if (WS_WINDOWS_NUMBER > windowId)
    {
        const ws_ble_wms_state_e wmsState = ws_convertWindowState(state);
        ws_ble_wms_window_state_update(&ws_wms[windowId], wmsState);
    }
}

static ws_ble_wms_state_e ws_convertWindowState(
    WS_WindowState_e state)
{
    return (ws_ble_wms_state_e) state;
}

static void ws_update_subscribers(WS_Window_e window, WS_ServerEvent_t event)
{
    uint_fast8_t i;

    for (i = 0; i < WS_SUBSCRIBERS_NUMBER; ++i)
    {
        ws_callbacks[i](window, event);
    }
}

static void ws_on_threshold_write(WS_Window_e window, uint16_t value)
{
    WS_ServerEvent_t e;
    e.eventType = WS_SERVER_EVENT_TYPE_THRESHOLD_UPDATE;
    e.value.threshold = value;
    ws_update_subscribers(window, e);
}

static void ws_on_enabled_write(WS_Window_e window, bool value)
{
    WS_ServerEvent_t e;
    e.eventType = WS_SERVER_EVENT_TYPE_ENABLED_UPDATE;
    e.value.enabled = value;
    WS_LOG_INFO("ws_on_enabled_write %u\r\n", value);
    ws_update_subscribers(window, e);
}

static void ws_on_apply_write(void)
{
    WS_ServerEvent_t e = {WS_SERVER_EVENT_TYPE_APPLY, {0}};
    ws_update_subscribers(WS_WINDOWS_NUMBER, e);
}

static void ws_timers_init(void)
{
}

static void ws_ble_stack_init(void)
{
    ret_code_t err_code;

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    WS_APP_ERROR(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    WS_APP_ERROR(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ws_on_ble_evt, NULL);
}

static void ws_peer_manager_init(
    bool erase_bonds)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    ble_conn_state_init();
    err_code = pm_init();
    WS_LOG_DEBUG("pm_init: %lu\r\n", err_code);
    WS_APP_ERROR(err_code);

    if (erase_bonds)
    {
        err_code = pm_peers_delete();
        WS_LOG_DEBUG("pm_peers_delete: %lu\r\n", err_code);
        WS_APP_ERROR(err_code);
    }

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_param);
    WS_LOG_DEBUG("pm_sec_params_set: %lu\r\n", err_code);
    WS_APP_ERROR(err_code);
}

static void ws_pm_evt_handler(
    pm_evt_t const * p_evt)
{
    ret_code_t err_code;

    switch (p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
        {
            WS_LOG_INFO("Connected to a previously bonded device.\r\n");
            ws_peer_id = p_evt->peer_id;
        } break;

        case PM_EVT_CONN_SEC_SUCCEEDED:
        {
            ws_peer_id = p_evt->peer_id;
            WS_LOG_INFO("Connection secured. Role: %d. conn_handle: %d, Procedure: %d, peerId: %d\r\n",
                         ble_conn_state_role(p_evt->conn_handle),
                         p_evt->conn_handle,
                         p_evt->params.conn_sec_succeeded.procedure,
                         p_evt->peer_id);
            WS_Event_t e = { .id = WS_SERVER_BT_EVENT_BONDED, .data = 0 };
            WS_EventHandler(e);
        } break;

        case PM_EVT_CONN_SEC_FAILED:
        {
            /* Often, when securing fails, it shouldn't be restarted, for security reasons.
             * Other times, it can be restarted directly.
             * Sometimes it can be restarted, but only after changing some Security Parameters.
             * Sometimes, it cannot be restarted until the link is disconnected and reconnected.
             * Sometimes it is impossible, to secure the link, or the peer device does not support it.
             * How to handle this error is highly application dependent. */
            WS_Event_t e = { .id = WS_SERVER_BT_EVENT_UNBONDED, .data = true };
            WS_EventHandler(e);
        } break;

        case PM_EVT_CONN_SEC_CONFIG_REQ:
        {
            // Don't reject pairing request from an already bonded peer.
            pm_conn_sec_config_t conn_sec_config = {.allow_repairing = true};
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
        } break;

        case PM_EVT_STORAGE_FULL:
        {
            // Run garbage collection on the flash.
            err_code = fds_gc();
            if (err_code == FDS_ERR_BUSY || err_code == FDS_ERR_NO_SPACE_IN_QUEUES)
            {
                // Retry.
            }
            else
            {
                WS_APP_ERROR(err_code);
            }
        } break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
        {
            WS_Event_t e = { .id = WS_SERVER_BT_EVENT_UNBONDED, .data = true };
            WS_EventHandler(e);
        } break;

        case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
        {
            // The local database has likely changed, send service changed indications.
            pm_local_database_has_changed();
        } break;

        case PM_EVT_PEER_DATA_UPDATE_FAILED:
        {
            // Assert.
            WS_APP_ERROR(p_evt->params.peer_data_update_failed.error);
        } break;

        case PM_EVT_PEER_DELETE_FAILED:
        {
            // Assert.
            WS_APP_ERROR(p_evt->params.peer_delete_failed.error);
        } break;

        case PM_EVT_PEERS_DELETE_FAILED:
        {
            // Assert.
            WS_APP_ERROR(p_evt->params.peers_delete_failed_evt.error);
            WS_Event_t e = { .id = WS_SERVER_BT_EVENT_UNBONDED, .data = false };
            WS_EventHandler(e);
        } break;

        case PM_EVT_ERROR_UNEXPECTED:
        {
            // Assert.
            WS_APP_ERROR(p_evt->params.error_unexpected.error);
        } break;

        case PM_EVT_CONN_SEC_START:
        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
        case PM_EVT_PEER_DELETE_SUCCEEDED:
        case PM_EVT_LOCAL_DB_CACHE_APPLIED:
        case PM_EVT_SERVICE_CHANGED_IND_SENT:
        case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
        default:
            break;
    }
}

static void ws_gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    WS_APP_ERROR(err_code);

    /* YOUR_JOB: Use an appearance value matching the application's use case.
       err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
       WS_APP_ERROR(err_code); */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    WS_LOG_INFO("sd_ble_gap_ppcp_set: %lu\r\n", err_code);
    WS_APP_ERROR(err_code);
}

static void ws_advertising_init(void)
{
    ret_code_t             err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(ws_adv_uuids) / sizeof(ws_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = ws_adv_uuids;

    init.config.ble_adv_on_disconnect_disabled  = true;
    init.config.ble_adv_fast_enabled            = true;
    init.config.ble_adv_fast_interval           = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout            = APP_ADV_DURATION;

    init.evt_handler = ws_on_adv_evt;

    err_code = ble_advertising_init(&ws_advertising, &init);
    WS_APP_ERROR(err_code);

    ble_advertising_conn_cfg_tag_set(&ws_advertising, APP_BLE_CONN_CFG_TAG);
}

static void ws_advertising_start(
    ble_gap_addr_t* addr)
{
    ble_advertising_start(&ws_advertising, BLE_ADV_MODE_FAST);
}

static void ws_on_adv_evt(
    ble_adv_evt_t ble_adv_evt)
{
    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
        {
//            NRF_LOG_INFO("Fast advertising.");
            WS_Event_t e = { .id = WS_SERVER_BT_EVENT_ADVERTISING_STARTED, .data = 0 };
            WS_EventHandler(e);
        } break;

        case BLE_ADV_EVT_IDLE:
        {
//            sleep_mode_enter();
            WS_Event_t e = { .id = WS_SERVER_BT_EVENT_ADVERTISING_STOPPED, .data = 0 };
            WS_EventHandler(e);
        } break;

        default:
            break;
    }
}

static void ws_services_init(
    const WS_Configuration_t *config)
{
    uint32_t err_code;

    // Initialize CS Service
    ws_ble_cs_init(&ws_cs, config, ws_on_threshold_write, ws_on_enabled_write, ws_on_apply_write);

    // Initialize WMS Service.
    if (config->windowEnabled[WS_WINDOW_1])
    {
        err_code = ws_ble_wms_init(&ws_wms[WS_WINDOW_1]);
        WS_APP_ERROR(err_code);
    }
    if (config->windowEnabled[WS_WINDOW_2])
    {
        err_code = ws_ble_wms_init(&ws_wms[WS_WINDOW_2]);
        WS_APP_ERROR(err_code);
    }
}

static void ws_conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = ws_on_conn_params_evt;
    cp_init.error_handler                  = ws_conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    WS_LOG_INFO("ble_conn_params_init: %lu\r\n", err_code);
    WS_APP_ERROR(err_code);
}

static void ws_on_conn_params_evt(
    ble_conn_params_evt_t * p_evt)
{
    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        uint32_t err_code = sd_ble_gap_disconnect(ws_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        WS_LOG_INFO("BLE_CONN_PARAMS_EVT_FAILED");
        WS_APP_ERROR(err_code);
    }
}

static void ws_conn_params_error_handler(
    uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

//static void ws_sleep_mode_enter(void)
//{
//    uint32_t err_code;
//
//    WS_LOG_INFO("Going into sleep mode\r\n");
//    // Go to system-off mode (this function will not return; wakeup will cause a reset).
//    err_code = sd_power_system_off();
//    WS_APP_ERROR(err_code);
//}

static void ws_on_ble_evt(
    ble_evt_t const * p_ble_evt,
    void * p_context)
{
    uint32_t err_code = NRF_SUCCESS;

    WS_LOG_DEBUG("ws_on_ble_evt event %u\r\n", p_ble_evt->header.evt_id);

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
        {
            WS_LOG_INFO("BLE_GAP_EVT_DISCONNECTED\r\n");
            WS_Event_t e = { .id = WS_SERVER_BT_EVENT_DISCONNECTED, .data = 0 };
            WS_EventHandler(e);
            break; // BLE_GAP_EVT_DISCONNECTED
        }

        case BLE_GAP_EVT_CONNECTED:
        {
            WS_LOG_INFO("BLE_GAP_EVT_CONNECTED\r\n");
            WS_Event_t e = { .id = WS_SERVER_BT_EVENT_CONNECTED, .data = p_ble_evt->evt.gap_evt.conn_handle };
            WS_EventHandler(e);
            break; // BLE_GAP_EVT_CONNECTED
        }

        case BLE_GAP_EVT_TIMEOUT:
            WS_LOG_DEBUG("BLE_GAP_EVT_TIMEOUT %u.\r\n", p_ble_evt->evt.gap_evt.params.timeout.src);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
        {
            // Disconnect on GATT Client timeout event.
            WS_LOG_DEBUG("GATT Client Timeout.\r\n");
            WS_Event_t e = { .id = WS_SERVER_BT_EVENT_DISCONNECT, .data = p_ble_evt->evt.gattc_evt.conn_handle };
            WS_EventHandler(e);
            break; // BLE_GATTC_EVT_TIMEOUT
        }

        case BLE_GATTS_EVT_TIMEOUT:
        {
            // Disconnect on GATT Server timeout event.
            WS_LOG_DEBUG("GATT Server Timeout.\r\n");
            WS_Event_t e = { .id = WS_SERVER_BT_EVENT_DISCONNECT, .data = p_ble_evt->evt.gatts_evt.conn_handle };
            WS_EventHandler(e);
            break; // BLE_GATTS_EVT_TIMEOUT
        }

        case BLE_EVT_USER_MEM_REQUEST:
            err_code = sd_ble_user_mem_reply(p_ble_evt->evt.gattc_evt.conn_handle, NULL);
            WS_APP_ERROR(err_code);
            break; // BLE_EVT_USER_MEM_REQUEST

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t  req;
            ble_gatts_rw_authorize_reply_params_t auth_reply;

            req = p_ble_evt->evt.gatts_evt.params.authorize_request;

            if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
            {
                if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ)     ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
                {
                    if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
                    }
                    else
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
                    }
                    auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
                    err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                               &auth_reply);
                    WS_APP_ERROR(err_code);
                }
            }
        } break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            WS_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            WS_APP_ERROR(err_code);
        } break;

#if (NRF_SD_BLE_API_VERSION == 3)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                       NRF_BLE_MAX_MTU_SIZE);
            WS_APP_ERROR(err_code);
            break; // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST
#endif

        default:
            WS_LOG_DEBUG("ws_on_ble_evt event %u not handled\r\n", p_ble_evt->header.evt_id);
            // No implementation needed.
            break;
    }

    ws_ble_wms_on_ble_evt(&ws_wms[WS_WINDOW_1], p_ble_evt);
    ws_ble_cs_on_ble_evt(&ws_cs, p_ble_evt);
}

static void ws_ServerBtResetHandler(
    void *p_event_data,
    uint16_t event_size)
{
    //todo move it to the system module
    nrf_delay_ms(500);
    sd_nvic_SystemReset();
}

static bool ws_addToWhitelist(
    pm_peer_id_t peerId)
{
    for (uint8_t i = 0; i < WS_WHITELIST_MAX_LEN; ++i)
    {
        if (PM_PEER_ID_INVALID == ws_whitelist[i])
        {
            ws_whitelist[i] = peerId;
            return true;
        }
    }
    return false;
}

static bool ws_bond(
    uint16_t connHandle)
{
    pm_peer_id_t peerId = PM_PEER_ID_INVALID;
    ret_code_t ret = pm_conn_secure(connHandle, false);
    WS_APP_ERROR(ret);
    ret = pm_peer_id_get(connHandle, &peerId);
    WS_APP_ERROR(ret);
    return ws_addToWhitelist(peerId);
}

static void ws_removeAllBondings(void)
{
    if (0 < pm_peer_count())
    {
        uint32_t err_code = pm_peers_delete();
        if (NRF_SUCCESS != err_code)
        {
            WS_LOG_DEBUG("pm_peers_delete: %lu\r\n", err_code);
        }
    }
    else
    {
        WS_ChangeState(&disconnectedState);
    }
}

static bool ws_disconnect(
    uint16_t connHandle)
{
    uint32_t err_code = sd_ble_gap_disconnect(connHandle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    WS_APP_ERROR(err_code);
    return NRF_SUCCESS == err_code;
}

static void WS_EventHandler(
    WS_Event_t event)
{
    WS_LOG_WARNING("WS_EventHandler %d\r\n", event.id);
    ws_currentState->eventHandler(event);
}

static void WS_ConnectedEventHandler(
    WS_Event_t event)
{
    if (WS_SERVER_BT_EVENT_DISCONNECT == event.id)
    {

    }
    else if (WS_SERVER_BT_EVENT_DISCONNECTED == event.id)
    {
        WS_ChangeState(&advertisingState);
    }
    else if (WS_BUTTON_EVENT_NORMAL == event.id)
    {
        WS_ChangeState(&bondingState);
    }
    else if (WS_BUTTON_EVENT_VERY_LONG == event.id)
    {
        WS_ChangeState(&unbondingState);
    }
    else
    {
        WS_LOG_WARNING("Event %d in WS_ConnectedEventHandler not handled\r\n", event.id);
    }
}

//static void WS_ConnectingEventHandler(
//    WS_Event_t event)
//{
//    if (WS_SERVER_BT_EVENT_CONNECTED == event.id)
//    {
//        ws_conn_handle = event.data16a;
//        WS_ChangeState(&connectedState);
//    }
//    else if (WS_SERVER_BT_EVENT_DISCONNECTED == event.id)
//    {
//        WS_ChangeState(&disconnectedState);
//    }
//    else
//    {
//        WS_LOG_WARNING("Event %d in WS_ConnectingEventHandler not handled\r\n", event.id);
//    }
//}

static void WS_DisconnectedEventHandler(
    WS_Event_t event)
{
    if (WS_SERVER_BT_EVENT_CONNECTED == event.id)
    {
        ws_conn_handle = event.data16a;
        WS_ChangeState(&connectedState);
    }
    else if (WS_BUTTON_EVENT_NORMAL == event.id)
    {
        WS_ChangeState(&advertisingState);
    }
    else if (WS_BUTTON_EVENT_VERY_LONG == event.id)
    {
        WS_ChangeState(&unbondingState);
    }
    else
    {
        WS_LOG_WARNING("Event %d in WS_DisconnectedEventHandler not handled\r\n", event.id);
    }
}

static void WS_DisconnectingEventHandler(
    WS_Event_t event)
{
    if (WS_SERVER_BT_EVENT_DISCONNECTED == event.id)
    {
        WS_ChangeState(&disconnectedState);
    }
    else
    {
        WS_LOG_WARNING("Event %d in WS_DisconnectingEventHandler not handled\r\n", event.id);
    }
}

static void WS_AdvertisingEventHandler(
    WS_Event_t event)
{
    if (WS_SERVER_BT_EVENT_ADVERTISING_STOPPED == event.id)
    {
        WS_ChangeState(&disconnectedState);
    }
    else if (WS_SERVER_BT_EVENT_CONNECTED == event.id)
    {
        ws_conn_handle = event.data16a;
        WS_ChangeState(&connectedState);
    }
    else
    {
        WS_LOG_WARNING("Event %d in WS_AdvertisingEventHandler not handled\r\n", event.id);
    }
}

static void WS_BondingEventHandler(
    WS_Event_t event)
{
    if (WS_SERVER_BT_EVENT_BONDED == event.id)
    {
        WS_ChangeState(&connectedState);
    }
    else if (WS_SERVER_BT_EVENT_UNBONDED == event.id)
    {
        WS_ChangeState(&connectedState);
    }
    else
    {
        WS_LOG_WARNING("Event %d in WS_BondingEventHandler not handled\r\n", event.id);
    }
}

static void WS_UnbondingEventHandler(
    WS_Event_t event)
{
    if (WS_SERVER_BT_EVENT_DISCONNECTED == event.id)
    {
        ws_removeAllBondings();
    }
    else if (WS_SERVER_BT_EVENT_UNBONDED == event.id)
    {
        WS_ChangeState(&disconnectedState);
    }
    else
    {
        WS_LOG_WARNING("Event %d in WS_UnbondingEventHandler not handled\r\n", event.id);
    }
}

static void WS_ChangeState(
    const WS_ServerBtState_t *newState)
{
    WS_LOG_DEBUG("%s -> %s\r\n", (uint32_t) WS_ConvertState(ws_currentState->stateId), (uint32_t) WS_ConvertState(newState->stateId));

    ws_currentState->exit();
    ws_currentState = newState;
    ws_currentState->entry();
}

static void WS_ConnectedOnExit(void)
{
}

//static void WS_ConnectingOnExit(void)
//{
//}

static void WS_DisconnectedOnExit(void)
{
}

static void WS_DisconnectingOnExit(void)
{
}

static void WS_AdvertisingOnExit(void)
{
    ws_connectable = false;
}

static void WS_BondingOnExit(void)
{
}

static void WS_UnbondingOnExit(void)
{
}

static void WS_ConnectedOnEnter(void)
{
}

//static void WS_ConnectingOnEnter(void)
//{
//}

static void WS_DisconnectedOnEnter(void)
{
    ws_conn_handle = BLE_CONN_HANDLE_INVALID;
}

static void WS_DisconnectingOnEnter(void)
{
    ws_disconnect(ws_conn_handle);
}

static void WS_AdvertisingOnEnter(void)
{
    ws_connectable = true;
    ws_advertising_start(NULL);
}

static void WS_BondingOnEnter(void)
{
    ws_bond(ws_conn_handle);
}

static void WS_UnbondingOnEnter(void)
{
    //todo stop advertising
    if (!ws_disconnect(ws_conn_handle))
    {
        ws_removeAllBondings();
    }
}

static char const * WS_ConvertState(
    WS_ServerBtState_e e)
{
    switch (e)
    {
    case WS_SERVER_BT_STATE_ADVERTISING: return WS_STRING(WS_SERVER_BT_STATE_ADVERTISING);
    case WS_SERVER_BT_STATE_BONDING: return WS_STRING(WS_SERVER_BT_STATE_BONDING);
    case WS_SERVER_BT_STATE_CONNECTED: return WS_STRING(WS_SERVER_BT_STATE_CONNECTED);
    case WS_SERVER_BT_STATE_CONNECTING: return WS_STRING(WS_SERVER_BT_STATE_CONNECTING);
    case WS_SERVER_BT_STATE_DISCONNECTED: return WS_STRING(WS_SERVER_BT_STATE_DISCONNECTED);
    case WS_SERVER_BT_STATE_DISCONNECTING: return WS_STRING(WS_SERVER_BT_STATE_DISCONNECTING);
    case WS_SERVER_BT_STATE_UNBONDING: return WS_STRING(WS_SERVER_BT_STATE_UNBONDING);
    case WS_SERVER_BT_STATE_UNKNOWN: return WS_STRING(WS_SERVER_BT_STATE_UNKNOWN);
    }

    return "unhandled state";
}


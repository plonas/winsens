/*
 * ble_peripheral.c
 *
 *  Created on: 20.02.2021
 *      Author: Damian Plonek
 */

#include "ble_peripheral.h"
#include "ble_peripheral_cfg.h"
#include "config.h"
#include "config_cfg.h"
#define ILOG_MODULE_NAME SVBT
#include "log.h"
#include "log_internal_nrf52.h"
#include "utils.h"
#include "winsens_types.h"
#include "window_state_cfg.h"

#include "app_timer.h"
#include "app_util.h"
#include "ble.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"
#include "fds.h"
#include "nrf_ble_gatt.h"
#include "nrf_sdh_ble.h"
#include "peer_manager.h"


#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */
#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define DEVICE_NAME                     "WinSensDev"                                /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "DamianPłonek"                              /**< Manufacturer. Will be passed to Device Information Service. */

#define APP_ADV_INTERVAL                128                                         /**< The advertising interval (in units of 0.625 ms. This value corresponds to 80 ms). */
#define APP_ADV_DURATION                0                                           /**< The advertising timeout in units of 10ms. */
#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)            /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                      /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                           /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define SERVICES_NUMBER                 (sizeof(g_services)/sizeof(ble_peripheral_svc_t))
#define CHARACTERISTICS_NUMBER          (sizeof(g_characteristics)/sizeof(ble_peripheral_char_t))
#define WHITELIST_LEN                   BLE_GAP_WHITELIST_ADDR_MAX_COUNT


typedef enum
{
    BLE_PERIPHERAL_EVT_CONNECT,
    BLE_PERIPHERAL_EVT_CONNECTED,
    BLE_PERIPHERAL_EVT_DISCONNECT,
    BLE_PERIPHERAL_EVT_DISCONNECTED,
    BLE_PERIPHERAL_EVT_ADVERTISE,
    BLE_PERIPHERAL_EVT_ADVERTISING_STARTED,
    BLE_PERIPHERAL_EVT_ADVERTISING_STOPPED,

    BLE_PERIPHERAL_EVT_UNBOND,
    BLE_PERIPHERAL_EVT_UNBONDED,
    BLE_PERIPHERAL_EVT_BOND,
    BLE_PERIPHERAL_EVT_BONDED,

} ble_peripheral_evt_enum_t;

typedef void (*ble_peripheral_evt_handler_t)(winsens_event_t event);
typedef void (*ble_peripheral_state_entry_t)(void);
typedef void (*ble_peripheral_state_exit_t)(void);

typedef struct WS_ServerBtState_s
{
    ble_peripheral_state_enum_t     state_id;
    ble_peripheral_evt_handler_t    evt_handler;
    ble_peripheral_state_entry_t    enter_state;
    ble_peripheral_state_exit_t     exit_state;

} ble_peripheral_state_t;


static void ble_stack_init(void);
static void gap_params_init(void);
static void conn_params_init(void);
static void peer_manager_init(bool erase_bonds);
static void services_init(void);
static void advertising_init(void);

static void on_adv_evt(ble_adv_evt_t ble_adv_evt);
static void on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);
static void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt);
static void pm_evt_handler(pm_evt_t const * p_evt);
static void on_conn_params_evt_handler(ble_conn_params_evt_t * p_evt);
static void conn_params_error_handler(uint32_t nrf_error);
static void evt_handler(winsens_event_t event);

static void change_state(const ble_peripheral_state_t *new_state);

static void empty_evt_handler(winsens_event_t event) {}
static void empty_on_enter(void) {}
static void empty_on_exit(void) {}

static void connected_evt_handler(winsens_event_t event);
//static void connecting_evt_handler(winsens_event_t event);
static void disconnected_evt_handler(winsens_event_t event);
static void disconnecting_evt_handler(winsens_event_t event);
static void advertising_evt_handler(winsens_event_t event);
static void bonding_evt_handler(winsens_event_t event);
static void unbonding_evt_handler(winsens_event_t event);

static void disconnected_on_enter(void);
static void disconnecting_on_enter(void);
static void advertising_on_enter(void);
static void bonding_on_enter(void);
static void unbonding_on_enter(void);

static void advertising_on_exit(void);

static void add_service(ble_peripheral_svc_t *svc);
static void add_characteristic(ble_peripheral_char_t *charact);
static void remove_all_bondings(void);
static bool disconnect(uint16_t connHandle);
static void start_advertising(ble_gap_addr_t* addr);
static bool bond(uint16_t connHandle);
static bool add_to_whitelist(pm_peer_id_t peerId);
static char const * convert_state_to_str(ble_peripheral_state_enum_t state);


const static ble_peripheral_state_t UNKNOWN_STATE           = { BLE_PERIPHERAL_STATE_UNKNOWN, empty_evt_handler, empty_on_enter, empty_on_exit };
const static ble_peripheral_state_t CONNECTED_STATE         = { BLE_PERIPHERAL_STATE_CONNECTED, connected_evt_handler, empty_on_enter, empty_on_exit };
const static ble_peripheral_state_t DISCONNECTED_STATE      = { BLE_PERIPHERAL_STATE_DISCONNECTED, disconnected_evt_handler, disconnected_on_enter, empty_on_exit };
const static ble_peripheral_state_t DISCONNECTING_STATE     = { BLE_PERIPHERAL_STATE_DISCONNECTING, disconnecting_evt_handler, disconnecting_on_enter, disconnecting_on_enter };
const static ble_peripheral_state_t ADVERTISING_STATE       = { BLE_PERIPHERAL_STATE_ADVERTISING, advertising_evt_handler, advertising_on_enter, advertising_on_exit };
const static ble_peripheral_state_t BONDING_STATE           = { BLE_PERIPHERAL_STATE_BONDING, bonding_evt_handler, bonding_on_enter, empty_on_exit };
const static ble_peripheral_state_t UNBONDING_STATE         = { BLE_PERIPHERAL_STATE_UNBONDING, unbonding_evt_handler, unbonding_on_enter, empty_on_exit };


static bool g_ble_peripheral_init                           = false;
static bool g_connectable                                   = false;
static uint16_t g_conn_handle                               = BLE_CONN_HANDLE_INVALID;                                      /**< Handle of the current connection. */
static const ble_peripheral_state_t *g_current_state        = &UNKNOWN_STATE;
//static ble_uuid_t g_adv_uuids[]                             = {{BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}};   /**< Universally unique service identifiers. */

static ble_peripheral_svc_t g_services[] = BLE_PERIPHERAL_SERVICES_INIT;
static ble_peripheral_char_t g_characteristics[] = BLE_PERIPHERAL_CHARS_INIT;
static ble_uuid_t g_adv_uuids[] = {{BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}}; /**< Universally unique service identifiers. */
static pm_peer_id_t g_whitelist[WHITELIST_LEN] = {PM_PEER_ID_INVALID};
static ble_peripheral_cb_t g_callbacks[BLE_PERIPERAL_MAX_CALLBACKS] = {NULL};

BLE_ADVERTISING_DEF(g_advertising);
NRF_BLE_GATT_DEF(g_gatt);


winsens_status_t ble_peripheral_init(void)
{
    winsens_status_t status = WINSENS_OK;

    if (false == g_ble_peripheral_init)
    {
        g_ble_peripheral_init = true;

        ble_stack_init();
        gap_params_init();

        ret_code_t err_code = nrf_ble_gatt_init(&g_gatt, gatt_evt_handler);
        LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);

        conn_params_init();

        peer_manager_init(false); //todo get param from config

        err_code = pm_register(pm_evt_handler);
        LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);

        services_init();
        advertising_init();

        change_state(&ADVERTISING_STATE);
    }

    (void)BONDING_STATE; //todo change to BONDING_STATE when button is pressed

    return status;
}

winsens_status_t ble_peripheral_disconnect()
{
    change_state(&DISCONNECTING_STATE);
    return WINSENS_OK;
}

winsens_status_t ble_peripheral_start_advertising(void)
{
    change_state(&ADVERTISING_STATE);
    return WINSENS_OK;
}

winsens_status_t ble_peripheral_delete_all_peers()
{
    change_state(&UNBONDING_STATE);
    return WINSENS_OK;
}

winsens_status_t ble_peripheral_bond(void)
{
    change_state(&BONDING_STATE);
    return WINSENS_OK;
}

winsens_status_t ble_peripheral_unbond(void)
{
    change_state(&UNBONDING_STATE);
    return WINSENS_OK;
}

winsens_status_t ble_peripheral_subscribe(ble_peripheral_cb_t callback)
{
    for (uint32_t i = 0; i < BLE_PERIPERAL_MAX_CALLBACKS; ++i)
    {
        if (NULL == g_callbacks[i])
        {
            g_callbacks[i] = callback;
            return WINSENS_OK;
        }
    }

    return WINSENS_NO_RESOURCES;
}

winsens_status_t ble_peripheral_update(ble_peripheral_svc_id_t server_id, ble_peripheral_char_id_t char_id, uint16_t value_len, uint8_t *value)
{
    //todo implement ble_peripheral_update
    return WINSENS_OK;
}

ble_peripheral_state_enum_t ble_peripheral_get_state(void)
{
    return g_current_state->state_id;
}

static void ble_stack_init(void)
{
    ret_code_t err_code;

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    LOG_NRF_ERROR_RETURN(err_code, ;);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    LOG_NRF_ERROR_RETURN(err_code, ;);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, on_ble_evt, NULL);
}

static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    LOG_NRF_ERROR_RETURN(err_code, ;);

    /* YOUR_JOB: Use an appearance value matching the application's use case.
       err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
       WS_LOG_NRF_ERROR_CHECK(err_code); */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    LOG_NRF_ERROR_RETURN(err_code, ;);
}

static void conn_params_init(void)
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
    cp_init.evt_handler                    = on_conn_params_evt_handler;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    LOG_NRF_ERROR_RETURN(err_code, ;);
}

static void peer_manager_init(bool erase_bonds)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    ble_conn_state_init();
    err_code = pm_init();
    LOG_NRF_ERROR_RETURN(err_code, ;);

    if (erase_bonds)
    {
        err_code = pm_peers_delete();
        LOG_NRF_ERROR_RETURN(err_code, ;);
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
    LOG_NRF_ERROR_RETURN(err_code, ;);
}

static void services_init(void)
{
    for (uint8_t i = 0; i < SERVICES_NUMBER; ++i)
    {
        add_service(&g_services[i]);
    }

    for (uint8_t i = 0; i < CHARACTERISTICS_NUMBER; ++i)
    {
        add_characteristic(&g_characteristics[i]);
    }
}

static void advertising_init(void)
{
    ret_code_t             err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(g_adv_uuids) / sizeof(g_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = g_adv_uuids;

    init.config.ble_adv_on_disconnect_disabled  = true;
    init.config.ble_adv_fast_enabled            = true;
    init.config.ble_adv_fast_interval           = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout            = APP_ADV_DURATION;

    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&g_advertising, &init);
    LOG_NRF_ERROR_RETURN(err_code, ;);

    ble_advertising_conn_cfg_tag_set(&g_advertising, APP_BLE_CONN_CFG_TAG);
}

static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
        {
//            NRF_LOG_INFO("Fast advertising.");
            winsens_event_t e = { .id = BLE_PERIPHERAL_EVT_ADVERTISING_STARTED, .data = 0 };
            evt_handler(e);
        } break;

        case BLE_ADV_EVT_IDLE:
        {
//            sleep_mode_enter();
            winsens_event_t e = { .id = BLE_PERIPHERAL_EVT_ADVERTISING_STOPPED, .data = 0 };
            evt_handler(e);
        } break;

        default:
            break;
    }
}

static void on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    uint32_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
        {
            LOG_INFO("BLE_GAP_EVT_DISCONNECTED");
            winsens_event_t e = { .id = BLE_PERIPHERAL_EVT_DISCONNECTED, .data = 0 };
            evt_handler(e);
            break; // BLE_GAP_EVT_DISCONNECTED
        }

        case BLE_GAP_EVT_CONNECTED:
        {
            LOG_INFO("BLE_GAP_EVT_CONNECTED");
            winsens_event_t e = { .id = BLE_PERIPHERAL_EVT_CONNECTED, .data = p_ble_evt->evt.gap_evt.conn_handle };
            evt_handler(e);
            break; // BLE_GAP_EVT_CONNECTED
        }

        case BLE_GAP_EVT_TIMEOUT:
            LOG_DEBUG("BLE_GAP_EVT_TIMEOUT %u.", p_ble_evt->evt.gap_evt.params.timeout.src);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
        {
            // Disconnect on GATT Client timeout event.
            LOG_DEBUG("GATT Client Timeout.");
            winsens_event_t e = { .id = BLE_PERIPHERAL_EVT_DISCONNECT, .data = p_ble_evt->evt.gattc_evt.conn_handle };
            evt_handler(e);
            break; // BLE_GATTC_EVT_TIMEOUT
        }

        case BLE_GATTS_EVT_TIMEOUT:
        {
            // Disconnect on GATT Server timeout event.
            LOG_DEBUG("GATT Server Timeout.");
            winsens_event_t e = { .id = BLE_PERIPHERAL_EVT_DISCONNECT, .data = p_ble_evt->evt.gatts_evt.conn_handle };
            evt_handler(e);
            break; // BLE_GATTS_EVT_TIMEOUT
        }

        case BLE_EVT_USER_MEM_REQUEST:
            err_code = sd_ble_user_mem_reply(p_ble_evt->evt.gattc_evt.conn_handle, NULL);
            LOG_NRF_ERROR_CHECK(err_code);
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
                    LOG_NRF_WARNING_CHECK(err_code);
                }
            }
        } break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            LOG_NRF_ERROR_CHECK(err_code);
        } break;

#if (NRF_SD_BLE_API_VERSION == 3)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                       NRF_BLE_MAX_MTU_SIZE);
            WS_LOG_NRF_WARNING_CHECK(err_code);
            break; // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST
#endif

        case BLE_GATTS_EVT_WRITE:
            //todo handle BLE_GATTS_EVT_WRITE
            break;

        default:
            LOG_DEBUG("ws_on_ble_evt event %u not handled", p_ble_evt->header.evt_id);
            // No implementation needed.
            break;
    }
}

static void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    if (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)
    {
        NRF_LOG_INFO("GATT ATT MTU on connection 0x%x changed to %d.",
                     p_evt->conn_handle,
                     p_evt->params.att_mtu_effective);
    }
}

static void pm_evt_handler(pm_evt_t const * p_evt)
{
    ret_code_t err_code;

    switch (p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
        {
            LOG_INFO("Connected to a previously bonded device.");
        } break;

        case PM_EVT_CONN_SEC_SUCCEEDED:
        {
            LOG_INFO("Connection secured. Role: %d. conn_handle: %d, Procedure: %d, peerId: %d",
                         ble_conn_state_role(p_evt->conn_handle),
                         p_evt->conn_handle,
                         p_evt->params.conn_sec_succeeded.procedure,
                         p_evt->peer_id);
            winsens_event_t e = { .id = BLE_PERIPHERAL_EVT_BONDED, .data = 0 };
            evt_handler(e);
        } break;

        case PM_EVT_CONN_SEC_FAILED:
        {
            /* Often, when securing fails, it shouldn't be restarted, for security reasons.
             * Other times, it can be restarted directly.
             * Sometimes it can be restarted, but only after changing some Security Parameters.
             * Sometimes, it cannot be restarted until the link is disconnected and reconnected.
             * Sometimes it is impossible, to secure the link, or the peer device does not support it.
             * How to handle this error is highly application dependent. */
            winsens_event_t e = { .id = BLE_PERIPHERAL_EVT_UNBONDED, .data = true };
            evt_handler(e);
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
                LOG_NRF_ERROR_CHECK(err_code);
            }
        } break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
        {
            winsens_event_t e = { .id = BLE_PERIPHERAL_EVT_UNBONDED, .data = true };
            evt_handler(e);
        } break;

        case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
        {
            // The local database has likely changed, send service changed indications.
            pm_local_database_has_changed();
        } break;

        case PM_EVT_PEER_DATA_UPDATE_FAILED:
        {
            // Assert.
            LOG_NRF_ERROR_CHECK(p_evt->params.peer_data_update_failed.error);
        } break;

        case PM_EVT_PEER_DELETE_FAILED:
        {
            // Assert.
            LOG_NRF_ERROR_CHECK(p_evt->params.peer_delete_failed.error);
        } break;

        case PM_EVT_PEERS_DELETE_FAILED:
        {
            // Assert.
            LOG_NRF_ERROR_CHECK(p_evt->params.peers_delete_failed_evt.error);
            winsens_event_t e = { .id = BLE_PERIPHERAL_EVT_UNBONDED, .data = false };
            evt_handler(e);
        } break;

        case PM_EVT_ERROR_UNEXPECTED:
        {
            // Assert.
            LOG_NRF_ERROR_CHECK(p_evt->params.error_unexpected.error);
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

static void on_conn_params_evt_handler(ble_conn_params_evt_t * p_evt)
{
    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        uint32_t err_code = sd_ble_gap_disconnect(g_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        LOG_NRF_WARNING_CHECK(err_code);
    }
}

static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

static void evt_handler(winsens_event_t event)
{
    LOG_DEBUG("evt_handler %d", event.id);
    g_current_state->evt_handler(event);
}

static void change_state(const ble_peripheral_state_t *new_state)
{
    LOG_DEBUG("%s -> %s", (uint32_t) convert_state_to_str(g_current_state->state_id), (uint32_t) convert_state_to_str(new_state->state_id));

    g_current_state->exit_state();
    g_current_state = new_state;
    g_current_state->enter_state();
}

static void connected_evt_handler(
    winsens_event_t event)
{
    if (BLE_PERIPHERAL_EVT_DISCONNECT == event.id)
    {

    }
    else if (BLE_PERIPHERAL_EVT_DISCONNECTED == event.id)
    {
        change_state(&ADVERTISING_STATE);
    }
    else
    {
        LOG_INFO("Event %d in WS_Connected_evt_handler not handled", event.id);
    }
}

//static void connecting_evt_handler(
//    WS_Event_t event)
//{
//    if (BLE_PERIPHERAL_EVT_CONNECTED == event.id)
//    {
//        ws_conn_handle = event.data16a;
//        change_state(&connectedState);
//    }
//    else if (BLE_PERIPHERAL_EVT_DISCONNECTED == event.id)
//    {
//        change_state(&disconnectedState);
//    }
//    else
//    {
//        WS_LOG_WARNING("Event %d in WS_Connecting_evt_handler not handled", event.id);
//    }
//}

static void disconnected_evt_handler(
    winsens_event_t event)
{
    if (BLE_PERIPHERAL_EVT_CONNECTED == event.id)
    {
        g_conn_handle = event.data16a;
        change_state(&CONNECTED_STATE);
    }
    else
    {
        LOG_INFO("Event %d in WS_Disconnected_evt_handler not handled", event.id);
    }
}

static void disconnecting_evt_handler(
    winsens_event_t event)
{
    if (BLE_PERIPHERAL_EVT_DISCONNECTED == event.id)
    {
        change_state(&DISCONNECTED_STATE);
    }
    else
    {
        LOG_INFO("Event %d in WS_Disconnecting_evt_handler not handled", event.id);
    }
}

static void advertising_evt_handler(
    winsens_event_t event)
{
    if (BLE_PERIPHERAL_EVT_ADVERTISING_STOPPED == event.id)
    {
        change_state(&DISCONNECTED_STATE);
    }
    else if (BLE_PERIPHERAL_EVT_CONNECTED == event.id)
    {
        g_conn_handle = event.data16a;
        change_state(&CONNECTED_STATE);
    }
    else
    {
        LOG_INFO("Event %d in WS_Advertising_evt_handler not handled", event.id);
    }
}

static void bonding_evt_handler(
    winsens_event_t event)
{
    if (BLE_PERIPHERAL_EVT_BONDED == event.id)
    {
        change_state(&CONNECTED_STATE);
    }
    else if (BLE_PERIPHERAL_EVT_UNBONDED == event.id)
    {
        change_state(&CONNECTED_STATE);
    }
    else
    {
        LOG_INFO("Event %d in WS_Bonding_evt_handler not handled", event.id);
    }
}

static void unbonding_evt_handler(
    winsens_event_t event)
{
    if (BLE_PERIPHERAL_EVT_DISCONNECTED == event.id)
    {
        remove_all_bondings();
    }
    else if (BLE_PERIPHERAL_EVT_UNBONDED == event.id)
    {
        change_state(&DISCONNECTED_STATE);
    }
    else
    {
        LOG_INFO("Event %d in WS_Unbonding_evt_handler not handled", event.id);
    }
}

static void disconnected_on_enter(void)
{
    g_conn_handle = BLE_CONN_HANDLE_INVALID;
}

static void disconnecting_on_enter(void)
{
    disconnect(g_conn_handle);
}

static void advertising_on_enter(void)
{
    g_connectable = true;
    start_advertising(NULL);
}

static void bonding_on_enter(void)
{
    bond(g_conn_handle);
}

static void unbonding_on_enter(void)
{
    //todo stop advertising
    if (!disconnect(g_conn_handle))
    {
        remove_all_bondings();
    }
}

static void advertising_on_exit(void)
{
    g_connectable = false;
}

static void add_service(ble_peripheral_svc_t *svc)
{
    uint32_t err_code = sd_ble_uuid_vs_add(&svc->service_base_uuid, &svc->service_uuid.type);
    LOG_NRF_ERROR_CHECK(err_code);
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &svc->service_uuid,
                                        &svc->service_handle);
    LOG_NRF_ERROR_CHECK(err_code);
}

static void add_characteristic(ble_peripheral_char_t *charact)
{
    const ble_peripheral_svc_t *svc = &g_services[charact->service_id];

    //Add a custom characteristic UUID
    uint32_t            err_code;
    ble_uuid_t          char_uuid;

    err_code = sd_ble_uuid_vs_add(&svc->service_base_uuid, &charact->char_uuid.type);
    LOG_NRF_ERROR_CHECK(err_code);

    //Add read/write properties to our characteristic
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = charact->read_enabled;
    char_md.char_props.write = charact->write_enabled;

    //Configuring Client Characteristic Configuration Descriptor metadata and add to char_md structure
    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = charact->notification_enabled;

    //Configure the attribute metadata
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_STACK;

    //Set read/write security levels to our characteristic
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    //Configure the characteristic value attribute
    ble_gatts_attr_t attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;

    //Set characteristic length in number of bytes
    attr_char_value.max_len     = sizeof(bool);
    attr_char_value.init_len    = sizeof(bool);
//    attr_char_value.p_value     = charact->value;

    //Add our new characteristic to the service
    err_code = sd_ble_gatts_characteristic_add(svc->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &charact->char_handle);
    LOG_NRF_ERROR_CHECK(err_code);
}

static void remove_all_bondings(void)
{
    if (0 < pm_peer_count())
    {
        uint32_t err_code = pm_peers_delete();
        LOG_NRF_WARNING_CHECK(err_code);
    }
    else
    {
        change_state(&DISCONNECTED_STATE);
    }
}

static bool disconnect(uint16_t connHandle)
{
    uint32_t err_code = sd_ble_gap_disconnect(connHandle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    LOG_NRF_WARNING_RETURN(err_code, false);
    return true;
}

static void start_advertising(ble_gap_addr_t* addr)
{
    ret_code_t err_code = ble_advertising_start(&g_advertising, BLE_ADV_MODE_FAST);
    LOG_NRF_ERROR_CHECK(err_code);
}

static bool bond(uint16_t connHandle)
{
    pm_peer_id_t peerId = PM_PEER_ID_INVALID;
    ret_code_t ret = pm_conn_secure(connHandle, false);
    LOG_NRF_WARNING_RETURN(ret, false);
    ret = pm_peer_id_get(connHandle, &peerId);
    LOG_NRF_WARNING_RETURN(ret, false);
    return add_to_whitelist(peerId);
}

static bool add_to_whitelist(pm_peer_id_t peerId)
{
    for (uint8_t i = 0; i < WHITELIST_LEN; ++i)
    {
        if (PM_PEER_ID_INVALID == g_whitelist[i])
        {
            g_whitelist[i] = peerId;
            return true;
        }
    }
    return false;
}

static char const * convert_state_to_str(ble_peripheral_state_enum_t state)
{
    switch (state)
    {
    case BLE_PERIPHERAL_STATE_ADVERTISING:      return UTILS_STRING(BLE_PERIPHERAL_STATE_ADVERTISING);
    case BLE_PERIPHERAL_STATE_BONDING:          return UTILS_STRING(BLE_PERIPHERAL_STATE_BONDING);
    case BLE_PERIPHERAL_STATE_CONNECTED:        return UTILS_STRING(BLE_PERIPHERAL_STATE_CONNECTED);
    case BLE_PERIPHERAL_STATE_CONNECTING:       return UTILS_STRING(BLE_PERIPHERAL_STATE_CONNECTING);
    case BLE_PERIPHERAL_STATE_DISCONNECTED:     return UTILS_STRING(BLE_PERIPHERAL_STATE_DISCONNECTED);
    case BLE_PERIPHERAL_STATE_DISCONNECTING:    return UTILS_STRING(BLE_PERIPHERAL_STATE_DISCONNECTING);
    case BLE_PERIPHERAL_STATE_UNBONDING:        return UTILS_STRING(BLE_PERIPHERAL_STATE_UNBONDING);
    case BLE_PERIPHERAL_STATE_UNKNOWN:          return UTILS_STRING(BLE_PERIPHERAL_STATE_UNKNOWN);
    }

    return "unknown state";
}

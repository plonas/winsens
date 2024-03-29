PROJECT_NAME     := winsens
TARGETS          := nrf52840_xxaa
OUTPUT_DIRECTORY := ../../../$(PROJECT_NAME)_build
PROJ_DIR         := ../..

include $(PROJ_DIR)/Makefile.cfg


$(OUTPUT_DIRECTORY)/nrf52840_xxaa.out: \
  LINKER_SCRIPT := nrf52_memory.ld

# Source files common to all targets
SRC_FILES += \
  $(PROJ_DIR)/projects/winsens/main.c \
  $(PROJ_DIR)/projects/winsens/app/app.c \
  $(PROJ_DIR)/src/hal/adc/nrf52/adc.c \
  $(PROJ_DIR)/src/hal/adc/cli/adc_cli.c \
  $(PROJ_DIR)/src/hal/pwr_mgr/nrf52/pwr_mgr.c \
  $(PROJ_DIR)/src/os/task_queue/nrf52/task_queue.c \
  $(PROJ_DIR)/src/hardware/storage/nrf52/storage.c \
  $(PROJ_DIR)/src/hal/i2c/nrf52/i2c.c \
  $(PROJ_DIR)/src/os/system/nrf52/system.c \
  $(PROJ_DIR)/src/os/critical_region/nrf52/critical_region.c \
  $(PROJ_DIR)/src/hal/digital_io/nrf52/digital_io.c \
  $(PROJ_DIR)/src/hardware/button/common/button.c \
  $(PROJ_DIR)/src/os/timer/nrf52/timer.c \
  $(PROJ_DIR)/src/os/log/nrf52/log_internal.c \
  $(PROJ_DIR)/src/firmware/window_state/dist/window_state.c \
  $(PROJ_DIR)/src/firmware/window_state/observer/ble/window_state_observer.c \
  $(PROJ_DIR)/src/firmware/battery/cr2032/battery.c \
  $(PROJ_DIR)/src/firmware/battery/observer/ble/battery_observer.c \
  $(PROJ_DIR)/src/firmware/distance/cny70/distance.c \
  $(PROJ_DIR)/src/firmware/distance/observer/ble/distance_observer.c \
  $(PROJ_DIR)/src/firmware/winsens/impl/winsens.c \
  $(PROJ_DIR)/src/firmware/config/storage/config.c \
  $(PROJ_DIR)/src/hardware/acc/lis3dh/acc.c \
  $(PROJ_DIR)/src/hardware/acc/ctrl/bt/acc_ctrl.c \
  $(PROJ_DIR)/src/hardware/ble_peripheral/nrf52/ble_peripheral.c \
  $(PROJ_DIR)/src/hardware/ble_peripheral_ctrl/nrf52/ble_peripheral_ctrl.c \
  $(PROJ_DIR)/src/app/hmi/buttons/hmi.c \
  $(PROJ_DIR)/src/utils/circular_buf/circular_buf.c \
  $(PROJ_DIR)/src/utils/circular_buf/safe/circular_buf_safe.c \
  $(PROJ_DIR)/src/utils/circular_buf/nonsafe/circular_buf_nonsafe.c \
  $(PROJ_DIR)/src/utils/subscribers/impl/subscribers.c \
  $(PROJ_DIR)/src/app/cli/nrf/cli.c \

SRC_FILES += \
  $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52840.S \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52840.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_saadc.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_timer.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_ppi.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twi.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uart.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uarte.c \
  $(SDK_ROOT)/modules/nrfx/soc/nrfx_atomic.c \
  $(SDK_ROOT)/modules/nrfx/hal/nrf_nvmc.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/external/fnmatch/fnmatch.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_uart.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_flash.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
  $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_sd.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_nvmc.c \
  $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
  $(SDK_ROOT)/components/libraries/atomic_flags/nrf_atflags.c \
  $(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
  $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
  $(SDK_ROOT)/components/libraries/bsp/bsp.c \
  $(SDK_ROOT)/components/libraries/bsp/bsp_btn_ble.c \
  $(SDK_ROOT)/components/libraries/button/app_button.c \
  $(SDK_ROOT)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
  $(SDK_ROOT)/components/boards/boards.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_ppi.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_uart.c \
  $(SDK_ROOT)/components/libraries/cli/nrf_cli.c \
  $(SDK_ROOT)/components/libraries/cli/uart/nrf_cli_uart.c \

# Source files for BLE
SRC_FILES += \
  $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_params.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_state.c \
  $(SDK_ROOT)/components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatt_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatts_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/id_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_data_storage.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_database.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_id.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/pm_buffer.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_dispatcher.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/auth_status_tracker.c \
  $(SDK_ROOT)/components/libraries/fds/fds.c \
  $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_params.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_state.c \
  $(SDK_ROOT)/components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer.c \
  $(SDK_ROOT)/components/libraries/util/sdk_mapped_flags.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c \
  $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
  $(SDK_ROOT)/components/libraries/queue/nrf_queue.c \

INC_FOLDERS += \
  ./config \
  ./app \
  $(PROJ_DIR)/src \
  $(PROJ_DIR)/src/utils \
  $(PROJ_DIR)/src/utils/circular_buf \
  $(PROJ_DIR)/src/utils/circular_buf/safe \
  $(PROJ_DIR)/src/utils/circular_buf/nonsafe \
  $(PROJ_DIR)/src/utils/command \
  $(PROJ_DIR)/src/utils/subscribers \
  $(PROJ_DIR)/src/hardware \
  $(PROJ_DIR)/src/firmware/window_state \
  $(PROJ_DIR)/src/firmware/battery \
  $(PROJ_DIR)/src/firmware/battery/observer \
  $(PROJ_DIR)/src/firmware/window_state/observer \
  $(PROJ_DIR)/src/firmware/distance \
  $(PROJ_DIR)/src/firmware/distance/observer \
  $(PROJ_DIR)/src/firmware/winsens \
  $(PROJ_DIR)/src/firmware/config \
  $(PROJ_DIR)/src/hardware/acc \
  $(PROJ_DIR)/src/hardware/acc/ctrl \
  $(PROJ_DIR)/src/os/log \
  $(PROJ_DIR)/src/os/log/nrf52 \
  $(PROJ_DIR)/src/hardware/button \
  $(PROJ_DIR)/src/hardware/storage \
  $(PROJ_DIR)/src/hal/adc \
  $(PROJ_DIR)/src/hal/pwr_mgr \
  $(PROJ_DIR)/src/hal/digital_io \
  $(PROJ_DIR)/src/hardware/ble_peripheral \
  $(PROJ_DIR)/src/hardware/ble_peripheral_ctrl \
  $(PROJ_DIR)/src/hal/i2c \
  $(PROJ_DIR)/src/os/timer \
  $(PROJ_DIR)/src/os/task_queue \
  $(PROJ_DIR)/src/os/system \
  $(PROJ_DIR)/src/os/critical_region \
  $(PROJ_DIR)/src/app/hmi \
  $(PROJ_DIR)/src/app/cli \
  $(PROJ_DIR)/src/app/cli/nrf \

INC_FOLDERS += \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/softdevice/s113/headers \
  $(SDK_ROOT)/components/softdevice/s113/headers/nrf52 \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/toolchain/gcc \
  $(SDK_ROOT)/components/toolchain \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/libraries/fds \
  $(SDK_ROOT)/components/libraries/strerror \
  $(SDK_ROOT)/components/libraries/memobj \
  $(SDK_ROOT)/components/libraries/balloc \
  $(SDK_ROOT)/components/libraries/atomic \
  $(SDK_ROOT)/components/libraries/atomic_fifo \
  $(SDK_ROOT)/components/libraries/atomic_flags \
  $(SDK_ROOT)/components/libraries/delay \
  $(SDK_ROOT)/components/libraries/ringbuf \
  $(SDK_ROOT)/components/libraries/mutex \
  $(SDK_ROOT)/components/libraries/bsp \
  $(SDK_ROOT)/components/libraries/pwr_mgmt \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/external/fprintf \
  $(SDK_ROOT)/external/fnmatch \
  $(SDK_ROOT)/modules/nrfx \
  $(SDK_ROOT)/modules/nrfx/mdk \
  $(SDK_ROOT)/modules/nrfx/hal \
  $(SDK_ROOT)/modules/nrfx/drivers/include \
  $(SDK_ROOT)/integration/nrfx \
  $(SDK_ROOT)/integration/nrfx/legacy \
  $(SDK_ROOT)/components/libraries/cli \
  $(SDK_ROOT)/components/libraries/cli/uart \
  $(SDK_ROOT)/components/libraries/queue \

INC_FOLDERS += \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/ble/peer_manager \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/ble/ble_advertising \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/components/softdevice/common \

# Optimization flags
OPT = -O3 -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DNRF_LOG_ENABLED=1
CFLAGS += -DNRF_LOG_BACKEND_SERIAL_USES_UART=0
CFLAGS += -DNRF_LOG_BACKEND_SERIAL_USES_RTT=0
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DSWI_DISABLE0
CFLAGS += -DNRFX_SAADC_API_V2
#CFLAGS += -D__HEAP_SIZE=0
#CFLAGS += -DBSP_DEFINES_ONLY
CFLAGS += -DBOARD_PCA10059
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DNRF_SD_BLE_API_VERSION=7
CFLAGS += -DS113
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DNRF52840_XXAA
CFLAGS += -DNRF52_SERIES
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS +=  -Wall -Werror
CFLAGS +=  -O0 -g3 -ggdb
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums
CFLAGS += -DDEBUG
CFLAGS += $(WINSENSIF)

# C++ flags common to all targets
CXXFLAGS += $(OPT)

# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
#ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DBOARD_PCA10059
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DNRF52840_XXAA
ASMFLAGS += -DNRF52_SERIES
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=7
ASMFLAGS += -DS113
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DDEBUG

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

nrf52840_xxaa: CFLAGS += -D__HEAP_SIZE=8192
nrf52840_xxaa: CFLAGS += -D__STACK_SIZE=8192
nrf52840_xxaa: ASMFLAGS += -D__HEAP_SIZE=8192
nrf52840_xxaa: ASMFLAGS += -D__STACK_SIZE=8192

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm


.PHONY:  default help

# Default target - first one defined
default: nrf52840_xxaa

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		nrf52840_xxaa
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc

include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY: flash erase

# Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/nrf52840_xxaa.hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/nrf52840_xxaa.hex --sectorerase
	nrfjprog -f nrf52 --reset

erase:
	nrfjprog -f nrf52 --eraseall

SDK_CONFIG_FILE := config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)

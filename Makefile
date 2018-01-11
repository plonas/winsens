PROJECT_NAME     := winSens
TARGETS          := nrf51422_xxac
OUTPUT_DIRECTORY := ../$(PROJECT_NAME)_build

SDK_ROOT := C:/dev/nordic/nRF5_SDK_12.2.0_f012efa
PROJ_DIR := .

$(OUTPUT_DIRECTORY)/nrf51422_xxac.out: \
  LINKER_SCRIPT  := nrf51_memory.ld

# Source files common to all targets
SRC_FILES += \
  $(PROJ_DIR)/src/main.c \
  $(PROJ_DIR)/src/winsens.c \
  $(PROJ_DIR)/src/sensors/ws_distance.c \
  $(SDK_ROOT)/components/drivers_nrf/hal/nrf_adc.c \
  $(SDK_ROOT)/components/drivers_nrf/adc/nrf_drv_adc.c \
  $(SDK_ROOT)/components/drivers_nrf/timer/nrf_drv_timer.c \
  $(SDK_ROOT)/components/drivers_nrf/ppi/nrf_drv_ppi.c \

INC_FOLDERS += \
  $(PROJ_DIR)/src \
  $(PROJ_DIR)/config \
  $(SDK_ROOT)/components/device \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/toolchain \
  $(SDK_ROOT)/components/drivers_nrf/adc \
  $(SDK_ROOT)/components/drivers_nrf/timer \
  $(SDK_ROOT)/components/drivers_nrf/ppi \
  $(SDK_ROOT)/components/drivers_nrf/hal \
  $(SDK_ROOT)/components/drivers_nrf/common \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/softdevice/s130/headers \

# C flags common to all targets
CFLAGS += -D"A"
CFLAGS += -DNRF_LOG_ENABLED=1
CFLAGS += -DNRF_LOG_BACKEND_SERIAL_USES_UART=0
CFLAGS += -DNRF_LOG_BACKEND_SERIAL_USES_RTT=1
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DSWI_DISABLE0
CFLAGS += -D__HEAP_SIZE=0
CFLAGS += -DNRF51
CFLAGS += -DS130
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DNRF51422
CFLAGS += -DNRF_SD_BLE_API_VERSION=2
CFLAGS += -mcpu=cortex-m0
CFLAGS += -mthumb -mabi=aapcs
CFLAGS +=  -Wall -Werror -O3 -g3
CFLAGS += -mfloat-abi=soft
# keep every function in separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums 

# C++ flags common to all targets
CXXFLAGS += \

# Assembler flags common to all targets
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -D__HEAP_SIZE=0
ASMFLAGS += -DNRF51
ASMFLAGS += -DS130
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DNRF51422
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=2

# Linker flags
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m0
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys


.PHONY: $(TARGETS) default all clean help flash flash_softdevice

# Default target - first one defined
default: nrf51422_xxac

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo 	nrf51422_xxac

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc

include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

# Flash the program
flash: $(OUTPUT_DIRECTORY)/nrf51422_xxac.hex
	@echo Flashing: $<
	nrfjprog --program $< -f nrf51 --sectorerase
	nrfjprog --reset -f nrf51

# Flash softdevice
flash_softdevice:
	@echo Flashing: s130_nrf51_2.0.1_softdevice.hex
	nrfjprog --program $(SDK_ROOT)/components/softdevice/s130/hex/s130_nrf51_2.0.1_softdevice.hex -f nrf51 --sectorerase 
	nrfjprog --reset -f nrf51

erase:
	nrfjprog --eraseall -f nrf52

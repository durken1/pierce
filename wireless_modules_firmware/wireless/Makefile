PROJECT_NAME     := Testgzll
TARGETS          := nrf51822_xxac
OUTPUT_DIRECTORY := _build

SDK_ROOT := ../../../nRF5_SDK_12.3.0_d7731ad
PROJ_DIR := ./

$(OUTPUT_DIRECTORY)/nrf51822_xxac.out: \
  LINKER_SCRIPT  := pierce.ld

# Source files common to all targets
SRC_FILES += \
  $(PROJ_DIR)/main.c \
  $(SDK_ROOT)/external/segger_rtt/RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c \
  $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf51.S \
  $(SDK_ROOT)/components/toolchain/system_nrf51.c \
  $(SDK_ROOT)/components/drivers_nrf/clock/nrf_drv_clock.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/drivers_nrf/rtc/nrf_drv_rtc.c \

# Include folders common to all targets
INC_FOLDERS += \
  ./config \
  $(PROJ_DIR) \
  $(SDK_ROOT)/components/proprietary_rf/gzll \
  $(SDK_ROOT)/components/toolchain/gcc \
  $(SDK_ROOT)/components/device \
  $(SDK_ROOT)/components/drivers_nrf/hal \
  $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd \
  $(SDK_ROOT)/components/toolchain \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/components/drivers_nrf/delay \
  $(SDK_ROOT)/components/drivers_nrf/clock \
  $(SDK_ROOT)/components/drivers_nrf/common \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/drivers_nrf/rtc \

# Libraries common to all targets
LIB_FILES += \
  $(SDK_ROOT)/components/proprietary_rf/gzll/gcc/gzll_nrf51_sd_resources_gcc.a \

# C flags common to all targets
CFLAGS += -DGAZELL_PRESENT
CFLAGS += -DBOARD_CUSTOM
CFLAGS += -DNRF51
CFLAGS += -DUSE_SD_HW_RESOURCES
CFLAGS += -DGAZELL_ALTERNATIVE_RESOURCES
CFLAGS += -DSWI_DISABLE0
CFLAGS += -DNRF51822
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
ASMFLAGS += -DGAZELL_PRESENT
ASMFLAGS += -DBOARD_CUSTOM
ASMFLAGS += -DNRF51
ASMFLAGS += -DUSE_SD_HW_RESOURCES
ASMFLAGS += -DGAZELL_ALTERNATIVE_RESOURCES
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DNRF51822

# Linker flags
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m0
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys


.PHONY: $(TARGETS) default all clean help flash 

# Default target - first one defined
default: nrf51822_xxac

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo 	nrf51822_xxac

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc

include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

# Flash the program
flash: $(OUTPUT_DIRECTORY)/nrf51822_xxac.hex
	@echo Flashing: $<
	nrfjprog --program $< -f nrf51 --sectorerase
	nrfjprog --reset -f nrf51

erase:
	nrfjprog --eraseall -f nrf52

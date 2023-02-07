PROJECT = com-monolithic
BUILD_DIR = build

SUPPORT_DIR = ./support
TAB_DIR = ../../c-implementation
CFILES = com_monolithic.c
CFILES += com.c
CFILES += tab.c

# Edit these two lines as needed
DEVICE=nrf51822aa
OOCD_FILE = board/common.cfg

# All lines below probably should not be edited
VPATH += $(SUPPORT_DIR)
VPATH += $(TAB_DIR)
INCLUDES += $(patsubst %,-I%, . $(SUPPORT_DIR))
INCLUDES += $(patsubst %,-I%, . $(TAB_DIR))
OPENCM3_DIR=../libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk

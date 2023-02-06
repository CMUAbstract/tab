PROJECT = cdh-monolithic
BUILD_DIR = build

SUPPORT_DIR = ./support
TAB_DIR = ../../c-implementation
CFILES = cdh_monolithic.c
CFILES += cdh.c
CFILES += tab.c

# Edit these two lines as needed
DEVICE=stm32l496rgt3
OOCD_FILE = board/stm32l4-generic.cfg

# All lines below probably should not be edited
VPATH += $(SUPPORT_DIR)
VPATH += $(TAB_DIR)
INCLUDES += $(patsubst %,-I%, . $(SUPPORT_DIR))
INCLUDES += $(patsubst %,-I%, . $(TAB_DIR))
OPENCM3_DIR=../libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk

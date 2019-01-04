PROJECT = stm32-msc-bootloader
BUILD_DIR = ../bin
INCLUDES = -I../inc
CFILES = stm32-msc-bootloader.c pseudo_fat.c

# TODO - you will need to edit these two lines!
DEVICE=stm32f103c8t6
OOCD_FILE = board/bluepill.cfg
LDSCRIPT=./stm32f103c8t6.ld

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk

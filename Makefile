SCETOOL_FLAGS := -0 SELF
SCETOOL_FLAGS += -1 TRUE
SCETOOL_FLAGS += -s FALSE
SCETOOL_FLAGS += -2 04
SCETOOL_FLAGS += -3 1070000052000001
SCETOOL_FLAGS += -4 01000002
SCETOOL_FLAGS += -5 APP
SCETOOL_FLAGS += -A 0001000000000000
SCETOOL_FLAGS += -6 0003004000000000

CELL_SDK ?= /usr/local/cell
CELL_MK_DIR ?= $(CELL_SDK)/samples/mk
include $(CELL_MK_DIR)/sdk.makedef.mk

OBJS_DIR = .

PPU_SRCS = $(wildcard *.c)
PPU_PRX_TARGET = prxmb.prx
PPU_CFLAGS += -Wno-unused-parameter -fno-builtin-printf -nodefaultlibs

PPU_PRX_LDFLAGS += $(PPU_CFLAGS)
PPU_PRX_LDLIBDIR += -L./vsh/lib

PPU_PRX_LDLIBS += -lallocator_vsh -lstdc_vsh -lsysPrxForUser_vsh

PPU_OPTIMIZE_LV = -Os
PPU_INCDIRS += -I./vsh/include

all: $(PPU_PRX_TARGET)
	$(PPU_PRX_STRIP) --strip-debug --strip-section-header $<
	scetool $(SCETOOL_FLAGS) -e $< $(PPU_SPRX_TARGET)

include $(CELL_MK_DIR)/sdk.target.mk

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

PPU_SRCS = $(wildcard *.c) $(wildcard ../prxmb_modules/*/prxmb_module.c)
PPU_PRX_TARGET = prxmb.prx
PPU_CFLAGS += -Wno-unused-parameter -ffunction-sections -fdata-sections -fno-builtin-printf

PPU_PRX_LDFLAGS += -Wl,--strip-unused-data -nodefaultlibs
PPU_PRX_LDLIBDIR += -L../common/vsh_exports/lib

PPU_PRX_LDLIBS += -lfs_stub
PPU_PRX_LDLIBS += -lallocator_export_stub -lstdc_export_stub -lsysPrxForUser_export_stub \
			-lvshtask_export_stub -lsys_net_export_stub

PPU_OPTIMIZE_LV = -Os
PPU_INCDIRS += -I../

all: $(PPU_PRX_TARGET)
	$(PPU_PRX_STRIP) --strip-debug --strip-section-header $<
	scetool $(SCETOOL_FLAGS) -e $< $(PPU_SPRX_TARGET)

include $(CELL_MK_DIR)/sdk.target.mk

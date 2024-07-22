#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

export TARGET := ttdldi
export TOPDIR := $(CURDIR)

export YSMENU_MAJOR	:= 1
export YSMENU_PATCH	:= 0

VERSION	:=	$(YSMENU_MAJOR).$(YSMENU_PATCH)

include $(DEVKITARM)/ds_rules

.PHONY: data ndsbootloader bootstub clean

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: ndsbootloader bootstub $(TARGET).nds

#---------------------------------------------------------------------------------
checkarm7:
	$(MAKE) -C arm7

#---------------------------------------------------------------------------------
checkarm9:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
$(TARGET).nds : $(NITRO_FILES) arm7/$(TARGET).elf arm9/$(TARGET).elf
	ndstool	-c $(TARGET).nds -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf \
			-b $(CURDIR)/icon.bmp "YSMenu HB Bootstrap;$(VERSION)" \
			-h 0x200
#	dlditool nrio.dldi $(TARGET).nds
	cp $(TARGET).nds ttdldi.dat
	cp $(TARGET).nds ttpatch.dat
	
data:
	@mkdir -p data

ndsbootloader: data
	$(MAKE) -C ndsbootloader LOADBIN=$(CURDIR)/data/load.bin
	

bootstub: data
	$(MAKE) -C bootstub
	
#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7

#---------------------------------------------------------------------------------
arm9/$(TARGET).elf: ndsbootloader
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	$(MAKE) -C ndsbootloader clean
	$(MAKE) -C bootstub clean
	rm -rf data
	rm -rf hbmenu
	rm -f $(TARGET).nds
	rm -f $(TARGET).dat
	rm -f ttpatch.dat


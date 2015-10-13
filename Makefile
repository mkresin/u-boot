include ./rg/rg_config.mk

ifdef CONFIG_BOOT_FROM_SPI
BOOT_FLASH_TYPE=spi
endif
ifdef CONFIG_BOOT_FROM_NOR
BOOT_FLASH_TYPE=nor
endif
ifdef CONFIG_BOOT_FROM_NAND
BOOT_FLASH_TYPE=nand
endif

ifdef CONFIG_VR9
BOARD_CONFIG=vr9_config
else
ifdef CONFIG_AR10
BOARD_CONFIG=ar10_config
else
$(error "*** Error: Currently only u-boot compilation for vr9/ar10 board is supported! ***")
endif
endif

ifeq ($(CONFIG_RG_HW),VRX288)
  BOARD_TYPE=vr9
else
  ifeq ($(CONFIG_RG_HW),SAGEM_HH5_VRX268)
    BOARD_TYPE=vr9
  else
    ifeq ($(CONFIG_RG_HW),GRX288)
      BOARD_TYPE=gr9
    else
      ifeq ($(CONFIG_RG_HW),XRX300)
        BOARD_TYPE=ar10
      else
        ifeq ($(CONFIG_RG_HW),SAGEM_HH4_ARX300)
          BOARD_TYPE=ar10
        else
          $(error "*** Error: Currently only VRX200/GRX200/XRX300/SAGEM_HH4_ARX300/SAGEM_HH5_VRX268 boards are supported! ***")
        endif
      endif
    endif
  endif
endif

GE_REV2=$(shell [ $(CONFIG_RG_HW_SAGEM_HH4_ARX300_REV) -ge 2 ] && echo true)
RMT_UPD_CRT=$(shell ls -L ./rg/rmt_upd*.crt)
RMT_UPD_SYMKEY=./rg/rmt_upd_sym.key
ENV_CRT=./rmt_upd.env
SYMKEY_C=./common/symkey.c

ARCHCONFIG_FIRST_TASKS+=uboot_make_config
uboot_make_config:
	# Assembler compiler cannot handle "extern vcCONFIG.." and we dont need
	# them - so remove them
	sed -e's/extern.*vc.*//' ./rg/rg_config.h > ./include/configs/rg_config.h
ifeq ($(CONFIG_BOOT_FROM_SPI)$(CONFIG_BOOT_FROM_NOR)$(CONFIG_BOOT_FROM_NAND),)
	@echo -e "\n*** Error: Please specify type of flash to boot u-boot from! ***\n"; false
else
	@$(MAKE) -f Makefile.u-boot $(BOARD_CONFIG)
	@cp ./defconfig.common ./.config
	@cat ./defconfig.$(BOOT_FLASH_TYPE) >> ./.config
	@cat ./defconfig.$(BOARD_TYPE) >> ./.config
ifeq ($(CONFIG_BOOTLDR_UBOOT_SECURED_BOOT)-$(CONFIG_RG_GPL),y-)
	@cat ./defconfig.secured >> ./.config
else
	@cat ./defconfig.non_secured >> ./.config
endif
ifdef CONFIG_AR10
  ifeq ($(GE_REV2),true) # Spin2B and up #
	@cat ./defconfig.ar10_cs1 >> ./.config
  else
	@cat ./defconfig.ar10_cs0 >> ./.config
  endif
endif
ifdef CONFIG_RG_GPL
	@cat ./defconfig.gpl >> ./.config
else
	@cat ./defconfig.openrg >> ./.config
endif
	@$(MAKE) -f Makefile.u-boot oldconfig
endif

OTHER_TASKS+=uboot_make

create_env_crt= \
	echo -n \"cert=\" >> $(ENV_CRT); \
	for i in $(RMT_UPD_CRT); do \
	  sed 's/\(.*\)/"\1\\n"/' $${i} >> $(ENV_CRT); \
	done; \
	echo -n \"\\0\" >> $(ENV_CRT)

create_symkey_c = \
	echo -n "u8 sym_key[] = {" >> $(SYMKEY_C); \
	sed 's/\(..\)/0x\1, /g' $(RMT_UPD_SYMKEY) >> $(SYMKEY_C); \
	echo "};" >> $(SYMKEY_C)

$(ENV_CRT): $(RMT_UPD_CRT)
	echo > $(ENV_CRT)
	$(call create_env_crt)

$(SYMKEY_C): $(if $(CONFIG_RG_GPL),,$(RMT_UPD_SYMKEY))
	echo > $(SYMKEY_C)
	$(call create_symkey_c)

uboot_make: $(ENV_CRT) $(SYMKEY_C)
	$(MAKE) V=99 POLARSSL_TINY=$(CONFIG_RG_POLARSSL) \
	SIGNED_IMAGE=$(CONFIG_BOOTLDR_UBOOT_SIGNED_IMAGES) \
	SECURED_ENV=$(CONFIG_BOOTLDR_UBOOT_SECURED_ENV) \
	-f Makefile.u-boot all

ifdef IS_BUILDDIR
clean:: uboot_clean

uboot_clean:
	$(MAKE) -f Makefile.u-boot clean
endif

include $(RGMK)

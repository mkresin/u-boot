RGSRC=../../
include $(RGSRC)/envir.mak

SUBDIRS+=include/polarssl
		
OTHER_TASKS+=polar_make
polar_make: dummy
	$(MAKE) -f Makefile.polar

ifdef IS_BUILDDIR
clean:: uboot_clean

uboot_clean:
	$(MAKE) -f Makefile.polar clean
endif

include $(RGMK)

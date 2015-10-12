```shell
export PATH=/your/path/to/openwrt/toolchain-mips_34kc+dsp_gcc-4.8-linaro_musl-1.1.11/bin/:$PATH

make uboot_make_config uboot_make

awk -f ./tools/lantiq_ram_init_uart.awk -v soc=vr9 board/vr9/ddr2/xrx200_ddr2_250M_settings.h > ./ddr
perl ./scripts_platform/gct ./ddr_settings ./u-boot.srec u-boot.asc
```

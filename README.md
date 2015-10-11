```shell
export PATH=/your/path/to/openwrt/toolchain-mips_34kc+dsp_gcc-4.8-linaro_musl-1.1.11/bin/:$PATH

make distclean vr9_config ARCH=mips CROSS_COMPILE=mips-openwrt-linux-musl-
cp my.config .config
make oldconfig u-boot.srec ARCH=mips CROSS_COMPILE=mips-openwrt-linux-musl-

awk -f ./tools/lantiq_ram_init_uart.awk -v soc=vr9 board/vr9/ddr2/xrx200_ddr2_250M_settings.h > ./ddr_settings
perl ./scripts_platform/gct ./ddr_settings ./u-boot.srec u-boot.asc
```

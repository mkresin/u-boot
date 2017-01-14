/*
 * Copyright (C) 2015 Martin Blumenstingl <martin.blumenstingl@googlemail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/gpio.h>
#include <asm/lantiq/eth.h>
#include <asm/lantiq/chipid.h>
#include <asm/lantiq/cpu.h>
#include <asm/arch/gphy.h>

#if defined(CONFIG_SYS_BOOT_RAM)
#define do_gpio_init	1
#define do_pll_init	0
#define do_dcdc_init	1
#elif defined(CONFIG_SYS_BOOT_NOR)
#define do_gpio_init	1
#define do_pll_init	1
#define do_dcdc_init	1
#elif defined(CONFIG_SYS_BOOT_NORSPL)
#define do_gpio_init	1
#define do_pll_init	1
#define do_dcdc_init	0
#else
#define do_gpio_init	0
#define do_pll_init	0
#define do_dcdc_init	1
#endif

#define GPIO_POWER_GREEN	14
#define GPIO_POWER_RED		28

static void gpio_init(void)
{
	/* Turn on the green power LED */
	gpio_direction_output(GPIO_POWER_GREEN, 0);

	/* Turn off the red power LED */
	gpio_direction_output(GPIO_POWER_RED, 1);
}

int board_early_init_f(void)
{
	if (do_gpio_init)
		gpio_init();

	if (do_pll_init)
		ltq_pll_init();

	if (do_dcdc_init)
		ltq_dcdc_init(0x7F);

	return 0;
}

int checkboard(void)
{
	puts("Board: " CONFIG_BOARD_NAME "\n");
	ltq_chip_print_info();

	return 0;
}

#if !defined(CONFIG_SPL_BUILD)
void show_boot_progress(int arg)
{
	if (!do_gpio_init)
		return;

	if (arg >= 0) {
		/* Success - turn off the red power LED and turn on the green power LED */
		gpio_set_value(GPIO_POWER_RED, 1);
		gpio_set_value(GPIO_POWER_GREEN, 0);
	} else {
		/* Failure - turn off green power LED and turn on red power LED */
		gpio_set_value(GPIO_POWER_GREEN, 1);
		gpio_set_value(GPIO_POWER_RED, 0);
	}
}
#endif

static const struct ltq_eth_port_config eth_port_config[] = {
	/* unused */
	{ 0, 0x0, LTQ_ETH_PORT_NONE, PHY_INTERFACE_MODE_NONE },
	/* unused */
	{ 1, 0x0, LTQ_ETH_PORT_NONE, PHY_INTERFACE_MODE_NONE },
	/* Internal GPHY0 with 10/100 firmware for LAN port 2 */
	{ 2, 0x11, LTQ_ETH_PORT_PHY, PHY_INTERFACE_MODE_MII },
	/* Internal GPHY0 with 10/100 firmware for LAN port 1 */
	{ 3, 0x12, LTQ_ETH_PORT_PHY, PHY_INTERFACE_MODE_MII },
	/* Internal GPHY1 with 10/100 firmware for LAN port 4 */
	{ 4, 0x13, LTQ_ETH_PORT_PHY, PHY_INTERFACE_MODE_MII },
	/* Internal GPHY1 with 10/100 firmware for LAN port 3 */
	{ 5, 0x14, LTQ_ETH_PORT_PHY, PHY_INTERFACE_MODE_MII },
};

static const struct ltq_eth_board_config eth_board_config = {
	.ports = eth_port_config,
	.num_ports = ARRAY_SIZE(eth_port_config),
};

int board_eth_init(bd_t * bis)
{
	const enum ltq_gphy_clk clk = LTQ_GPHY_CLK_25MHZ_PLL0;
	const ulong fw_addr = 0x80FF0000;

	switch (ltq_chip_version_get()) {
	case 1:
		ltq_gphy_phy22f_a1x_load(fw_addr);
		break;
	case 2:
		ltq_gphy_phy22f_a2x_load(fw_addr);
		break;
	}

	ltq_cgu_gphy_clk_src(clk);

	ltq_rcu_gphy_boot(0, fw_addr);
	ltq_rcu_gphy_boot(1, fw_addr);

	return ltq_eth_initialize(&eth_board_config);
}

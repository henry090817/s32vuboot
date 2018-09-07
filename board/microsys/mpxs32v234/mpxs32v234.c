/*
 * (C) Copyright 2013-2016 Freescale Semiconductor, Inc.
 * (C) Copyright 2017 MicroSys Electronics GmbH
 * Copyright 2018 NXP
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/siul.h>
#include <asm/arch/clock.h>
#include <asm/arch/xrdc.h>
#include <asm/arch/cse.h>
#include <fdt_support.h>
#include <libfdt.h>
#include <miiphy.h>
#include <netdev.h>
#include <net.h>
#include <i2c.h>
#include "s32v_ocotp.h"

#undef	CONFIG_MPXS32V234_R1

DECLARE_GLOBAL_DATA_PTR;

void rtc_init(void);

static void setup_iomux_uart(void)
{
	/* Muxing for linflex0 and linflex1 */

	/* set PA12 - MSCR[12] - for UART0 TXD */
	writel(SIUL2_MSCR_PORT_CTRL_UART_TXD, SIUL2_MSCRn(SIUL2_MSCR_PA12));

	/* set PA11 - MSCR[11] - for UART0 RXD */
	writel(SIUL2_MSCR_PORT_CTRL_UART_RXD, SIUL2_MSCRn(SIUL2_MSCR_PA11));
	/* set UART0 RXD - IMCR[200] - to link to PA11 */
	writel(SIUL2_IMCR_UART_RXD_to_pad, SIUL2_IMCRn(SIUL2_IMCR_UART0_RXD));

	/* set PA14 - MSCR[14] - for UART1 TXD*/
	writel(SIUL2_MSCR_PORT_CTRL_UART_TXD, SIUL2_MSCRn(SIUL2_MSCR_PA14));

	/* set PA13 - MSCR[13] - for UART1 RXD */
	writel(SIUL2_MSCR_PORT_CTRL_UART_RXD, SIUL2_MSCRn(SIUL2_MSCR_PA13));
	/* set UART1 RXD - IMCR[202] - to link to PA13 */
	writel(SIUL2_IMCR_UART_RXD_to_pad, SIUL2_IMCRn(SIUL2_IMCR_UART1_RXD));
}

static void setup_iomux_enet(void)
{
#ifndef CONFIG_PHY_RGMII_DIRECT_CONNECTED
	/* MDC - PC13 */
	writel(0x0020c701, SIUL2_MSCRn(45));
	/* MDIO - PC14 */
	writel(0x0028c701, SIUL2_MSCRn(46));
	writel(0x2, SIUL2_MSCRn(981));
#endif

#ifdef CONFIG_PHY_RGMII_DIRECT_CONNECTED
	/* TX_CLK - PC15 */
	writel(0x0020c701, SIUL2_MSCRn(47));
#else
	/* TX_CLK - PC15 */
	writel(0x00203701, SIUL2_MSCRn(47));
#endif
	writel(0x2, SIUL2_MSCRn(978));

	/* RX_CLK - PD0 */
	writel(0x0008c700, SIUL2_MSCRn(48));
	writel(0x2, SIUL2_MSCRn(979));

	/* RX_D0 - PD1 */
	writel(0x0008c700, SIUL2_MSCRn(49));
	writel(0x2, SIUL2_MSCRn(974));

	/* RX_D1 - PD2 */
	writel(0x0008c700, SIUL2_MSCRn(50));
	writel(0x2, SIUL2_MSCRn(975));

	/* RX_D2 - PD3 */
	writel(0x0008c700, SIUL2_MSCRn(51));
	writel(0x2, SIUL2_MSCRn(976));

	/* RX_D3 - PD4 */
	writel(0x0008c700, SIUL2_MSCRn(52));
	writel(0x2, SIUL2_MSCRn(977));

	/* RX_DV - PD5 */
	writel(0x0008c700, SIUL2_MSCRn(53));
	writel(0x2, SIUL2_MSCRn(973));

	/* TX_D0 - PD7 */
	writel(0x0020c701, SIUL2_MSCRn(55));
	/* TX_D1 - PD8 */
	writel(0x0020c701, SIUL2_MSCRn(56));
	/* TX_D2 - PD9 */
	writel(0x0020c701, SIUL2_MSCRn(57));
	/* TX_D3 - PD10 */
	writel(0x0020c701, SIUL2_MSCRn(58));
	/* TX_EN - PD11 */
	writel(0x0020c701, SIUL2_MSCRn(59));
}

static void setup_iomux_i2c(void)
{
#ifdef	CONFIG_MPXS32V234_R1
	/* MPXS32V234-R1 */
	/* I2C0 - Serial Data Input */
	writel(SIUL2_PAD_CTRL_I2C0_MSCR1_SDA, SIUL2_MSCRn(15));
	writel(SIUL2_PAD_CTRL_I2C0_IMCR1_SDA, SIUL2_IMCRn(269));

	/* I2C0 - Serial Clock Input */
	writel(SIUL2_PAD_CTRL_I2C0_MSCR1_SCLK, SIUL2_MSCRn(16));
	writel(SIUL2_PAD_CTRL_I2C0_IMCR1_SCLK, SIUL2_IMCRn(268));

	/* I2C1 - Serial Data Input */
	writel(SIUL2_PAD_CTRL_I2C1_MSCR1_SDA, SIUL2_MSCRn(17));
	writel(SIUL2_PAD_CTRL_I2C1_IMCR1_SDA, SIUL2_IMCRn(271));

	/* I2C1 - Serial Clock Input */
	writel(SIUL2_PAD_CTRL_I2C1_MSCR1_SCLK, SIUL2_MSCRn(18));
	writel(SIUL2_PAD_CTRL_I2C1_IMCR1_SCLK, SIUL2_IMCRn(270));

#elif defined(CONFIG_MPXS32V234_R2)
	/* MPXS32V234-R2 */
	/* I2C0 - Serial Data Input */
	writel(SIUL2_PAD_CTRL_I2C0_MSCR_SDA, SIUL2_MSCRn(99));
	writel(SIUL2_PAD_CTRL_I2C0_IMCR_SDA, SIUL2_IMCRn(269));

	/* I2C0 - Serial Clock Input */
	writel(SIUL2_PAD_CTRL_I2C0_MSCR_SCLK, SIUL2_MSCRn(100));
	writel(SIUL2_PAD_CTRL_I2C0_IMCR_SCLK, SIUL2_IMCRn(268));

	/* I2C1 - Serial Data Input */
	writel(SIUL2_PAD_CTRL_I2C1_MSCR_SDA, SIUL2_MSCRn(101));
	writel(SIUL2_PAD_CTRL_I2C1_IMCR_SDA, SIUL2_IMCRn(271));

	/* I2C1 - Serial Clock Input */
	writel(SIUL2_PAD_CTRL_I2C1_MSCR_SCLK, SIUL2_MSCRn(102));
	writel(SIUL2_PAD_CTRL_I2C1_IMCR_SCLK, SIUL2_IMCRn(270));
#else
#error	Please define CONFIG_MPXS32V234_R1 or CONFIG_MPXS32V234_R2
#endif
	/* I2C2 - Serial Data Input */
	writel(SIUL2_PAD_CTRL_I2C2_MSCR_SDA, SIUL2_MSCRn(19));
	writel(SIUL2_PAD_CTRL_I2C2_IMCR_SDA, SIUL2_IMCRn(273));

	/* I2C2 - Serial Clock Input */
	writel(SIUL2_PAD_CTRL_I2C2_MSCR_SCLK, SIUL2_MSCRn(20));
	writel(SIUL2_PAD_CTRL_I2C2_IMCR_SCLK, SIUL2_IMCRn(272));
}

static void mscm_init(void)
{
	struct mscm_ir *mscmir = (struct mscm_ir *)MSCM_BASE_ADDR;
	int i;

	for (i = 0; i < MSCM_IRSPRC_NUM; i++)
		writew(MSCM_IRSPRC_CPn_EN, &mscmir->irsprc[i]);
}

int board_phy_config(struct phy_device *phydev)
{
	if (phydev->drv->config)
		phydev->drv->config(phydev);

	return 0;
}

void setup_xrdc(void)
{
	writel(XRDC_ADDR_MIN, XRDC_MRGD_W0_16);
	writel(XRDC_ADDR_MAX, XRDC_MRGD_W1_16);
	writel(XRDC_VALID, XRDC_MRGD_W3_16);

	writel(XRDC_ADDR_MIN, XRDC_MRGD_W0_17);
	writel(XRDC_ADDR_MAX, XRDC_MRGD_W1_17);
	writel(XRDC_VALID, XRDC_MRGD_W3_17);

	writel(XRDC_ADDR_MIN, XRDC_MRGD_W0_18);
	writel(XRDC_ADDR_MAX, XRDC_MRGD_W1_18);
	writel(XRDC_VALID, XRDC_MRGD_W3_18);

	writel(XRDC_ADDR_MIN, XRDC_MRGD_W0_19);
	writel(XRDC_ADDR_MAX, XRDC_MRGD_W1_19);
	writel(XRDC_VALID, XRDC_MRGD_W3_19);
}

#ifdef CONFIG_DCU_QOS_FIX
int board_dcu_qos(void)
{
	/* m_fastdma1_ib */
	writel(0x0, 0x40012380);
	writel(0x0, 0x40012384);

	/* m_gpu0 */
	writel(0x0, 0x40012480);
	writel(0x0, 0x40012484);

	/* m_h264dec */
	writel(0x0, 0x40012580);
	writel(0x0, 0x40012584);

	/* m_gpu1 */
	writel(0x0, 0x40012680);
	writel(0x0, 0x40012684);

	/* m_cores_cci1_ib */
	writel(0x0, 0x40012780);
	writel(0x0, 0x40012784);

	/* m_apex0_dma */
	writel(0x0, 0x40012880);
	writel(0x0, 0x40012884);

	/* m_apex0_blkdm a */
	writel(0x0, 0x40012980);
	writel(0x0, 0x40012984);

	/* m_apex1_dma */
	writel(0x0, 0x40012A80);
	writel(0x0, 0x40012A84);

	/* m_apex1_blkdma */
	writel(0x0, 0x40012B80);
	writel(0x0, 0x40012B84);

	/* m_pcie */
	writel(0x0, 0x40012C80);
	writel(0x0, 0x40012C84);

	/* m_enet0 */
	writel(0x0, 0x40012D80);
	writel(0x0, 0x40012D84);

	/* m_enet1 */
	writel(0x0, 0x40012E80);
	writel(0x0, 0x40012E84);

	/* m_cores_cci0 */
	writel(0x0, 0x40012F80);
	writel(0x0, 0x40012F84);

	/* m_axbs */
	writel(0x0, 0x40013080);
	writel(0x0, 0x40013084);

	/* m_pdi0 */
	writel(0x0, 0x40013180);
	writel(0x0, 0x40013184);

	/* m_pdi1 */
	writel(0x0, 0x40013280);
	writel(0x0, 0x40013284);

	/* m_axbs_ram */
	writel(0x0, 0x40013380);
	writel(0x0, 0x40013384);

	/* m_fastdma0 */
	writel(0x0, 0x40013480);
	writel(0x0, 0x40013484);

	return 0;
}
#endif

int board_early_init_f(void)
{
	clock_init();
	mscm_init();

	setup_iomux_uart();
	setup_iomux_enet();
	setup_iomux_i2c();

	setup_iomux_dcu();
#ifdef CONFIG_DCU_QOS_FIX
	board_dcu_qos();

#endif

	setup_xrdc();
	return 0;
}

int board_get_mac(struct eth_device *dev, unsigned char *mac)
{
	if (!dev || !mac)
		return CMD_RET_FAILURE;

	u32 ocotp_mac0 = readl(OCOTP_MAC0_REG);
	u32 ocotp_mac1 = readl(OCOTP_MAC1_REG);

	mac[0] = (ocotp_mac1 >> 8);
	mac[1] = ocotp_mac1;

	mac[2] = (ocotp_mac0 >> 24);
	mac[3] = (ocotp_mac0 >> 16);
	mac[4] = (ocotp_mac0 >> 8);
	mac[5] = ocotp_mac0;

	return CMD_RET_SUCCESS;
}

int board_eth_init(bd_t *bis)
{
	/*
	 * cpu_eth_init() is implemented in arch/arm/cpu/armv8/s23v234/soc.c
	 * It calls fecmxc_initialize(bis).
	 */

	if (cpu_eth_init(bis) < 0)
		printf("CPU Net Initialization Failed\n");

	/*
	 * After the call above the MAC can now be manipulated ...
	 */

	struct eth_device *dev = eth_get_dev_by_name("FEC");

	if (dev) {
		unsigned char mac[6];

		board_get_mac(dev, mac);

		if (!is_zero_ethaddr(mac)) {
			if (!getenv("ethaddr")) {
				char buf[20];
				sprintf(buf, "%pM", mac);
				setenv("ethaddr", buf);
			}
		} else {
			printf("Note: no MAC for %s in SROM programmed!\n"
				, dev->name);
		}

		memcpy(dev->enetaddr, mac, 6);
	}

	return 0;
}

int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

	rtc_init();

	/*
	 * Enable HDMI output
	 */

	i2c_set_bus_num(2);
	i2c_reg_write(0x3f, 0x8, 0x35);

	return 0;
}

int checkboard(void)
{
#ifdef	CONFIG_MPXS32V234_R1
	puts("Board: mpxs32v234-R1\n");
#elif	defined(CONFIG_MPXS32V234_R2)
	puts("Board: mpxs32v234-R2\n");
#else
	puts("Board: Unknown board\n");
#endif
	return 0;
}


#if defined(CONFIG_OF_FDT) && defined(CONFIG_OF_BOARD_SETUP)
int ft_board_setup(void *blob, bd_t *bd)
{
	ft_cpu_setup(blob, bd);
	return 0;
}
#endif /* defined(CONFIG_OF_FDT) && defined(CONFIG_OF_BOARD_SETUP) */

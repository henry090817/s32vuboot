/*
 * Copyright 2015 Freescale Semiconductor
 * (C) Copyright 2017 NXP
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/imx-regs.h>
#include <asm/arch/siul.h>
#include <asm/arch/clock.h>
#include <asm/arch/xrdc.h>
#include <asm/arch/mc_cgm_regs.h>
#include <asm/arch/mc_me_regs.h>
#include <asm/arch/mc_rgm_regs.h>
#include <asm/arch/src.h>
#include <asm/arch/mmdc.h>
#include <asm/arch/ddr.h>
#if defined(CONFIG_S32_LPDDR2)
#include <asm/arch/lpddr2.h>
#elif defined(CONFIG_S32_DDR3)
#include <asm/arch/ddr3.h>
#elif defined(CONFIG_S32_LPDDR4)
#include <asm/arch/lpddr4.h>
#else
#error "Please define the DDR type!"
#endif


void setup_iomux_enet(void);

#ifdef CONFIG_FSL_DCU_FB
void setup_iomux_dcu(void);
#else
static inline void setup_iomux_dcu(void)
{
}
#endif

void cpu_pci_clock_init(const int clockexternal);

/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * (C) Copyright 2017-2018 NXP
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/arch/mc_me_regs.h>
#include <asm/arch/mc_rgm_regs.h>
#include "mp.h"

DECLARE_GLOBAL_DATA_PTR;

void *get_spin_tbl_addr(void)
{
	void *ptr = (void *)SECONDARY_CPU_BOOT_PAGE;

	/*
	 * Spin table is at the beginning of secondary boot page. It is
	 * copied to SECONDARY_CPU_BOOT_PAGE.
	 */
	ptr += (u64)&__spin_table - (u64)&secondary_boot_page;

	return ptr;
}

phys_addr_t determine_mp_bootpg(void)
{
	return (phys_addr_t)SECONDARY_CPU_BOOT_PAGE;
}

#ifdef CONFIG_S32V234
int fsl_s32_wake_seconday_cores(void)
{
	void *boot_loc = (void *)SECONDARY_CPU_BOOT_PAGE;
	size_t *boot_page_size = &(__secondary_boot_page_size);
	u64 *table = get_spin_tbl_addr();

	/* Clear spin table so that secondary processors
	 * observe the correct value after waking up from wfe.
	 */
	memset(table, 0, CONFIG_MAX_CPUS * ENTRY_SIZE);
	flush_dcache_range((unsigned long)boot_loc,
			   (unsigned long)boot_loc + *boot_page_size);

	/* program the cores possible running modes */
	writew(MC_ME_CCTL_DEASSERT_CORE, MC_ME_CCTL2);
	writew(MC_ME_CCTL_DEASSERT_CORE, MC_ME_CCTL3);
	writew(MC_ME_CCTL_DEASSERT_CORE, MC_ME_CCTL4);

	/* write the cores' start address */
	writel(CONFIG_SYS_TEXT_BASE | MC_ME_CADDRn_ADDR_EN, MC_ME_CADDR2);
	writel(CONFIG_SYS_TEXT_BASE | MC_ME_CADDRn_ADDR_EN, MC_ME_CADDR3);
	writel(CONFIG_SYS_TEXT_BASE | MC_ME_CADDRn_ADDR_EN, MC_ME_CADDR4);

	writel( MC_ME_MCTL_RUN0 | MC_ME_MCTL_KEY, MC_ME_MCTL );
	writel( MC_ME_MCTL_RUN0 | MC_ME_MCTL_INVERTEDKEY, MC_ME_MCTL );

	while( (readl(MC_ME_GS) & MC_ME_GS_S_MTRANS) != 0x00000000 );

	smp_kick_all_cpus();

//	printf("All (%d) cores are up.\n", cpu_numcores());

	return 0;
}
#elif defined(CONFIG_S32_GEN1)
void fsl_s32_wake_seconday_core(int prtn, int core)
{
	/* Set core clock enable bit. */
	writel(MC_ME_PRTN_N_CORE_M_PCONF_CCE,
	       MC_ME_PRTN_N_CORE_M_PCONF(prtn, core));

	/* Enable core clock triggering to update on writing CTRL key
	 * sequence.
	 */
	writel(MC_ME_PRTN_N_CORE_M_PUPD_CCUPD,
	       MC_ME_PRTN_N_CORE_M_PUPD(prtn, core));

	/* Write start_address in MC_ME_PRTN_N_CORE_M_ADDR register. */
	writel(CONFIG_SYS_TEXT_BASE,
	       MC_ME_PRTN_N_CORE_M_ADDR(prtn, core));

	/* Write valid key sequence to trigger the update. */
	writel(MC_ME_CTL_KEY_KEY, MC_ME_CTL_KEY);
	writel(MC_ME_CTL_KEY_INVERTEDKEY, MC_ME_CTL_KEY);

	/* Wait until hardware process to enable core is completed. */
	while (readl(MC_ME_PRTN_N_CORE_M_PUPD(prtn, core)))
		;
}

int fsl_s32_wake_seconday_cores(void)
{
	void *boot_loc = (void *)SECONDARY_CPU_BOOT_PAGE;
	size_t *boot_page_size = &(__secondary_boot_page_size);
	u64 *table = get_spin_tbl_addr();
	u32 reset;

	/* Clear spin table so that secondary processors
	 * observe the correct value after waking up from wfe.
	 */
	memset(table, 0, CONFIG_MAX_CPUS * ENTRY_SIZE);
	flush_dcache_range((unsigned long)boot_loc,
			   (unsigned long)boot_loc + *boot_page_size);

	/* Cluster 0, core 0 is already enabled by BootROM.
	 * We should enable core 1 from partion 0 and
	 * core 0 and 1 from partition 1.
	 * The procedure can be found in
	 * "MC_ME application core enable", S32R RM Rev1 DraftC.
	 */
	fsl_s32_wake_seconday_core(0, 1);
	fsl_s32_wake_seconday_core(1, 0);
	fsl_s32_wake_seconday_core(1, 1);

	/* Releasing reset of A53 cores(1,2,3) using peripherals
	 * reset in RGM.
	 */
	reset = readl(RGM_PRST(RGM_CORES_RESET_GROUP));

	/* Clear the bits corresponding to cores 1,2,3. */
	reset &= ~0x1C;
	writel(reset, RGM_PRST(RGM_CORES_RESET_GROUP));

	/* Wait until the bits corresponding to the cores in the RGM_PSTATn
	 * are cleared (see "Individual Peripheral Resets",
	 * S32R RM Rev1 DraftC).
	 */
	while (readl(RGM_PSTAT(RGM_CORES_RESET_GROUP)) != reset)
		;

	smp_kick_all_cpus();

//	printf("All (%d) cores are up.\n", cpu_numcores());

	return 0;
}
#else
#error "Incomplete platform definition"
#endif

int is_core_valid(unsigned int core)
{
	if (core == 0)
		return 0;

	return !!((1 << core) & cpu_mask());
}

int cpu_reset(int nr)
{
	puts("Feature is not implemented.\n");

	return 0;
}

int cpu_disable(int nr)
{
	puts("Feature is not implemented.\n");

	return 0;
}

int core_to_pos(int nr)
{
	u32 cores = cpu_mask();
	int i, count = 0;

	if (nr == 0) {
		return 0;
	} else if (nr >= hweight32(cores)) {
		puts("Not a valid core number.\n");
		return -1;
	}

	for (i = 1; i < 32; i++) {
		if (is_core_valid(i)) {
			count++;
			if (count == nr)
				break;
		}
	}

	return count;
}

int cpu_status(int nr)
{
	u64 *table;
	int valid;

	if (nr == 0) {
		table = (u64 *)get_spin_tbl_addr();
		printf("table base @ 0x%p\n", table);
	} else {
		valid = is_core_valid(nr);
		if (!valid)
			return -1;
		table = (u64 *)get_spin_tbl_addr() + nr * NUM_BOOT_ENTRY;
		printf("table @ 0x%p\n", table);
		printf("   addr - 0x%016llx\n", table[BOOT_ENTRY_ADDR]);
		printf("   r3   - 0x%016llx\n", table[BOOT_ENTRY_R3]);
		printf("   pir  - 0x%016llx\n", table[BOOT_ENTRY_PIR]);
	}

	return 0;
}

int cpu_release(int nr, int argc, char * const argv[])
{
	u64 boot_addr;
	u64 *table = (u64 *)get_spin_tbl_addr();
#ifndef CONFIG_FSL_SMP_RELEASE_ALL
	int valid;

	valid = is_core_valid(nr);
	if (!valid)
		return 0;

	table += nr * NUM_BOOT_ENTRY;
#endif
	boot_addr = simple_strtoull(argv[0], NULL, 16);
	table[BOOT_ENTRY_ADDR] = boot_addr;
	flush_dcache_range((unsigned long)table,
			   (unsigned long)table + SIZE_BOOT_ENTRY);
	asm volatile("dsb st");
	smp_kick_all_cpus();	/* only those with entry addr set will run */

	return 0;
}

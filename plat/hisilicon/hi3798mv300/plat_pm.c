/* SPDX-License-Identifier: BSD-3-Clause */
#include <assert.h>

#include <arch_helpers.h>
#include <bl31/bl31.h>
#include <common/debug.h>
#include <drivers/console.h>
#include <drivers/delay_timer.h>
#include <lib/mmio.h>
#include <lib/psci/psci.h>
#include <lib/spinlock.h>
#include <plat/common/platform.h>

#include <plat_private.h>
#include <platform_def.h>

/*
 * MMU/cache-on, coherent Normal memory. Protect shared CRG registers across
 * CPU_ON requests for different targets; per-target state belongs to TF-A.
 */
static spinlock_t crg_lock __aligned(CACHE_WRITEBACK_GRANULE);

static int mv300_cpu_on(u_register_t mpidr)
{
	unsigned int cpu = (unsigned int)plat_core_pos_by_mpidr(mpidr);
	uint32_t lp, slow, reset, por, core;

	if (cpu == 0U || cpu >= PLATFORM_CORE_COUNT) {
		return PSCI_E_INVALID_PARAMS;
	}
	por = MV300_CPU_POR_RESET(cpu);
	core = MV300_CPU_CORE_RESET(cpu);
	spin_lock(&crg_lock);
	reset = mmio_read_32(MV300_CPU_RST);
	if ((reset & (por | core | MV300_CPU_POR_RESET(0) |
		      MV300_CPU_CORE_RESET(0))) != (por | core)) {
		spin_unlock(&crg_lock);
		return PSCI_E_INTERN_FAIL;
	}
	/*
	 * RVBAR stays on the aligned TF-A warm entry. No per-request vector
	 * restoration or wait for the PSCI on-finisher: CPU_ON is asynchronous.
	 */
	mmio_setbits_32(MV300_CPU_MODE, BIT(cpu));
	dsbsy();
	lp = mmio_read_32(MV300_CPU_LP);
	slow = (lp & ~MV300_CPU_LP_UPDATE_MASK) | MV300_CPU_LP_RELEASE_VALUE;
	mmio_write_32(MV300_CPU_LP, slow);
	mmio_write_32(MV300_CPU_LP, slow | MV300_CPU_LP_LATCH);
	dsbsy();
	mmio_clrbits_32(MV300_CPU_RST, por);
	(void)mmio_read_32(MV300_CPU_RST);
	dsbsy();
	mmio_clrbits_32(MV300_CPU_RST, core);
	(void)mmio_read_32(MV300_CPU_RST);
	dsbsy();
	mmio_write_32(MV300_CPU_LP, lp & ~MV300_CPU_LP_LATCH);
	mmio_write_32(MV300_CPU_LP, lp);
	dsbsy();
	spin_unlock(&crg_lock);
	return PSCI_E_SUCCESS;
}

static void mv300_cpu_on_finish(const psci_power_state_t *state)
{
	assert(state->pwr_domain_state[0] == PLAT_MAX_OFF_STATE);
	mv300_gic_pcpu_setup();
	INFO("MV300: CPU%u TF-A on_finish\n", plat_my_core_pos());
}

static int mv300_validate_ns_entrypoint(uintptr_t entry)
{
	if (!is_aligned(entry, 4U) || entry < MV300_NS_ENTRY_MIN ||
	    entry >= MV300_NS_DRAM_LIMIT) {
		return PSCI_E_INVALID_ADDRESS;
	}
	return PSCI_E_SUCCESS;
}

/* MV300 datasheet: SYS_CTRL base, SC_SYSRES p80, SC_LOCKEN p96. */
static void __dead2 mv300_system_reset(void)
{
	NOTICE("MV300: PSCI SYSTEM_RESET via SYS_CTRL\n");
	console_flush();
	mmio_write_32(MV300_SC_LOCKEN, MV300_SC_UNLOCK);
	dsbsy();
	mmio_write_32(MV300_SC_SYSRES, 0xfee1deadU);
	dsbsy();
	mdelay(100);
	ERROR("MV300: SYS_CTRL reset did not complete\n");
	panic();
}

/* Original PSCI encoding: CPU-level standby with StateID zero only. */
static int mv300_validate_power_state(unsigned int power_state,
				      psci_power_state_t *state)
{
	unsigned int level;

	if (power_state != 0U) {
		return PSCI_E_INVALID_PARAMS;
	}
	for (level = 0U; level <= PLAT_MAX_PWR_LVL; level++) {
		state->pwr_domain_state[level] = PSCI_LOCAL_STATE_RUN;
	}
	state->pwr_domain_state[0] = PLAT_MAX_RET_STATE;
	return PSCI_E_SUCCESS;
}

static void mv300_cpu_standby(plat_local_state_t state)
{
	u_register_t scr = read_scr_el3();

	assert(state == PLAT_MAX_RET_STATE);
	/* Allow physical interrupts to wake WFI while executing at EL3. */
	write_scr_el3(scr | SCR_IRQ_BIT | SCR_FIQ_BIT);
	isb();
	dsbsy();
	wfi();
	/* Keep DAIF masked; the OS handles the pending interrupt after return. */
	write_scr_el3(scr);
	isb();
}

/* CPU_OFF and SYSTEM_OFF remain unsupported. */
static const plat_psci_ops_t ops = {
	.cpu_standby = mv300_cpu_standby,
	.validate_power_state = mv300_validate_power_state,
	.system_reset = mv300_system_reset,
	.pwr_domain_on = mv300_cpu_on,
	.pwr_domain_on_finish = mv300_cpu_on_finish,
	.validate_ns_entrypoint = mv300_validate_ns_entrypoint,
};

int plat_setup_psci_ops(uintptr_t sec_entrypoint, const plat_psci_ops_t **out)
{
	assert(sec_entrypoint == (uintptr_t)bl31_warm_entrypoint);
	assert(is_aligned((uintptr_t)mv300_warm_entry, MV300_RVBAR_ALIGNMENT));
	mmio_write_32(MV300_CPU_RVBAR, (uintptr_t)mv300_warm_entry);
	dsbsy();
	*out = &ops;
	return 0;
}

/* SPDX-License-Identifier: BSD-3-Clause */
#include <assert.h>

#include <arch_helpers.h>
#include <common/bl_common.h>
#include <common/debug.h>
#include <drivers/arm/pl011.h>
#include <drivers/console.h>
#include <drivers/generic_delay_timer.h>
#include <lib/xlat_tables/xlat_tables_v2.h>
#include <plat/common/platform.h>

#include <plat_private.h>
#include <platform_def.h>

static console_t console;
static entry_point_info_t bl33;

unsigned int plat_get_syscnt_freq2(void)
{
	return MV300_COUNTER_FREQ;
}

void bl31_early_platform_setup2(u_register_t a0, u_register_t a1,
			      u_register_t a2, u_register_t a3)
{
	console_pl011_register(MV300_UART_BASE, MV300_UART_CLK,
			      MV300_UART_BAUD, &console);
	console_set_scope(&console, CONSOLE_FLAG_BOOT | CONSOLE_FLAG_RUNTIME |
			  CONSOLE_FLAG_CRASH);
	SET_PARAM_HEAD(&bl33, PARAM_EP, VERSION_1, NON_SECURE);
	bl33.pc = PRELOADED_BL33_BASE;
	bl33.spsr = SPSR_64(MODE_EL2, MODE_SP_ELX, DISABLE_ALL_EXCEPTIONS);
}

void bl31_plat_arch_setup(void)
{
	mmap_add_region(BL31_BASE, BL31_BASE, BL31_LIMIT - BL31_BASE,
			MT_MEMORY | MT_RW | MT_SECURE);
	mmap_add_region(BL_CODE_BASE, BL_CODE_BASE, BL_CODE_END - BL_CODE_BASE,
			MT_CODE | MT_SECURE);
	mmap_add_region(BL_RO_DATA_BASE, BL_RO_DATA_BASE,
			BL_RO_DATA_END - BL_RO_DATA_BASE, MT_RO_DATA | MT_SECURE);
	mmap_add_region(MV300_DEVICE_BASE, MV300_DEVICE_BASE, MV300_DEVICE_SIZE,
			MT_DEVICE | MT_RW | MT_SECURE);
	init_xlat_tables();
	enable_mmu_el3(0);
}

void bl31_platform_setup(void)
{
	generic_delay_timer_init();
	mv300_gic_init();
	NOTICE("MV300: BL31 [0x%x,0x%x), BL33 0x%x at NS EL2\n",
	       BL31_BASE, BL31_LIMIT, PRELOADED_BL33_BASE);
}

entry_point_info_t *bl31_plat_get_next_image_ep_info(uint32_t type)
{
	assert(sec_state_is_valid(type));
	return type == NON_SECURE ? &bl33 : NULL;
}

/* SPDX-License-Identifier: BSD-3-Clause */
#include <arch_helpers.h>
#include <common/debug.h>
#include <drivers/arm/gicv2.h>
#include <lib/mmio.h>

#include <plat_private.h>
#include <platform_def.h>

static const gicv2_driver_data_t gic_data = {
	.gicd_base = MV300_GICD_BASE,
	.gicc_base = MV300_GICC_BASE,
};

/*
 * On the tested vendor-AUX boot path, NS accesses expose the Secure GIC
 * register view. Give the OS all interrupts as Group0, signalled as IRQ.
 * BL31 owns no interrupts; this policy cannot be combined with a Secure
 * payload or Secure interrupt handlers. It does not establish isolation.
 */
void mv300_gic_pcpu_setup(void)
{
	gicv2_pcpu_distif_init();
	/* IGROUPR0 is banked: configure SGIs/PPIs on every starting CPU. */
	mmio_write_32(MV300_GICD_BASE + GICD_IGROUPR, 0U);
	mmio_write_32(MV300_GICC_BASE + GICC_PMR, GIC_PRI_MASK);
	/* FIQEn=0 and AckCtl=0: Group0 uses the ordinary IAR/EOIR path. */
	mmio_write_32(MV300_GICC_BASE + GICC_CTLR, CTLR_ENABLE_G0_BIT);
	dsbsy();
	isb();
}

void mv300_gic_init(void)
{
	unsigned int banks;

	gicv2_driver_init(&gic_data);
	gicv2_distif_init();
	mmio_write_32(MV300_GICD_BASE + GICD_CTLR, 0U);
	dsbsy();
	banks = (mmio_read_32(MV300_GICD_BASE + GICD_TYPER) &
		 TYPER_IT_LINES_NO_MASK) + 1U;
	/* Configure SPI groups once; bank zero is handled per CPU below. */
	for (unsigned int i = 1U; i < banks; i++) {
		mmio_write_32(MV300_GICD_BASE + GICD_IGROUPR + 4U * i, 0U);
	}
	mv300_gic_pcpu_setup();
	mmio_write_32(MV300_GICD_BASE + GICD_CTLR, CTLR_ENABLE_G0_BIT);
	dsbsy();
	NOTICE("MV300: GIC Group0 IRQ policy (vendor-AUX boot path)\n");
}

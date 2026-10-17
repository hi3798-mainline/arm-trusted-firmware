/* SPDX-License-Identifier: BSD-3-Clause */
#include <arch.h>
#include <asm_macros.S>
#include <platform_def.h>

	.global plat_my_core_pos
	.global plat_reset_handler
	.global platform_mem_init
	.global plat_crash_console_init
	.global plat_crash_console_putc
	.global plat_crash_console_flush
	.global mv300_warm_entry

func plat_my_core_pos
	mrs x0, mpidr_el1
	and x0, x0, #MPIDR_CPU_MASK
	ret
endfunc plat_my_core_pos

func plat_reset_handler
	/* Cortex-A53 cpu_ops also enables SMPEN before caches are enabled. */
	mov_imm x0, MV300_COUNTER_FREQ
	msr cntfrq_el0, x0
	isb
	ret
endfunc plat_reset_handler

func platform_mem_init
	/* Vendor AUX has already initialized DDR. */
	ret
endfunc platform_mem_init

func plat_crash_console_init
	mov_imm x0, MV300_UART_BASE
	mov_imm x1, MV300_UART_CLK
	mov_imm x2, MV300_UART_BAUD
	b console_pl011_core_init
endfunc plat_crash_console_init

func plat_crash_console_putc
	mov_imm x1, MV300_UART_BASE
	b console_pl011_core_putc
endfunc plat_crash_console_putc

func plat_crash_console_flush
	mov_imm x0, MV300_UART_BASE
	b console_pl011_core_flush
endfunc plat_crash_console_flush

/* Preserve the 2 KiB RVBAR alignment used by the tested MV300 entry code. */
func mv300_warm_entry, 11
	b bl31_warm_entrypoint
endfunc mv300_warm_entry

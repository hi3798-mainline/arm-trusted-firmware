/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef HI3798MV300_H
#define HI3798MV300_H

#include <lib/utils_def.h>

#define MV300_SC_SYSRES U(0xf8000004)
#define MV300_SC_LOCKEN U(0xf800020c)
#define MV300_SC_UNLOCK U(0x4f50454e)

#define MV300_UART_BASE U(0xf8b00000)
#define MV300_UART_CLK U(75000000)
#define MV300_UART_BAUD U(115200)
#define MV300_COUNTER_FREQ U(24000000)
#define MV300_GICD_BASE U(0xf1001000)
#define MV300_GICC_BASE U(0xf1002000)
#define MV300_CPU_LP U(0xf8a22048)
#define MV300_CPU_RST U(0xf8a22050)
#define MV300_CPU_MODE U(0xf8a80030)
#define MV300_CPU_RVBAR U(0xf8a80034)

/*
 * Preserve the hardware-tested low-power control sequence. Field meanings
 * beyond the tested masks still need confirmation against MV300 documentation.
 */
#define MV300_CPU_LP_UPDATE_MASK U(0x407)
#define MV300_CPU_LP_RELEASE_VALUE U(0x206)
#define MV300_CPU_LP_LATCH BIT(10)
#define MV300_CPU_CORE_RESET(cpu) BIT(8 + (cpu))
#define MV300_CPU_POR_RESET(cpu) BIT(12 + (cpu))
#define MV300_RVBAR_ALIGNMENT U(0x800)
#define MV300_DEVICE_BASE U(0xf0000000)
#define MV300_DEVICE_SIZE U(0x10000000)
#define MV300_NS_ENTRY_MIN U(0x1000)

#endif /* HI3798MV300_H */

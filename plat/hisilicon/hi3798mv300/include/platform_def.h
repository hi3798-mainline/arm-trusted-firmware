/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef MV300_PLATFORM_DEF_H
#define MV300_PLATFORM_DEF_H

#include <lib/utils_def.h>
#include <plat/common/common_def.h>

#include <hi3798mv300.h>

#define PLATFORM_LINKER_FORMAT "elf64-littleaarch64"
#define PLATFORM_LINKER_ARCH aarch64
#define PLATFORM_CORE_COUNT U(4)
#define PLATFORM_CLUSTER_COUNT U(1)
#define PLATFORM_MAX_CPUS_PER_CLUSTER U(4)
#define PLATFORM_STACK_SIZE U(0x1000)
#define CACHE_WRITEBACK_SHIFT U(6)
#define CACHE_WRITEBACK_GRANULE (1 << CACHE_WRITEBACK_SHIFT)
#define PLAT_MAX_PWR_LVL U(1)
#define PLAT_NUM_PWR_DOMAINS (PLATFORM_CORE_COUNT + PLATFORM_CLUSTER_COUNT)
#define PLAT_MAX_RET_STATE U(1)
#define PLAT_MAX_OFF_STATE U(2)

/* X96Q 1 GiB development layout; reserved by U-Boot and the Linux DT. */
#define BL31_BASE U(0x3fe00000)
#define BL31_LIMIT U(0x40000000)
#define MV300_NS_DRAM_LIMIT BL31_BASE
#define PLAT_PHY_ADDR_SPACE_SIZE (ULL(1) << 32)
#define PLAT_VIRT_ADDR_SPACE_SIZE (ULL(1) << 32)
#define MAX_MMAP_REGIONS U(8)
#define MAX_XLAT_TABLES U(4)

#endif /* MV300_PLATFORM_DEF_H */

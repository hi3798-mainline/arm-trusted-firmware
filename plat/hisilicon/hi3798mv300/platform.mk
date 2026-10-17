# SPDX-License-Identifier: BSD-3-Clause
# The vendor-AUX boot path assigns all GIC interrupts to the NS OS.
# Secure payloads/EL3 interrupt services require a different ownership model.
ifneq (${SPD},none)
$(error hi3798mv300 requires SPD=none: Secure payloads are unsupported)
endif

MV300_UNSUPPORTED_OPTIONS := SPM_MM SPMC_AT_EL3 SDEI_SUPPORT \
	EL3_EXCEPTION_HANDLING ENABLE_RME PSCI_OS_INIT_MODE
$(foreach opt,${MV300_UNSUPPORTED_OPTIONS},\
	$(if $(filter-out 0,$(strip $(${opt}))),\
		$(error hi3798mv300 requires ${opt}=0)))

ifneq ($(strip ${BL32} ${PRELOADED_BL32_BASE}),)
$(error hi3798mv300 does not support BL32 or PRELOADED_BL32_BASE)
endif

MV300_PLAT := plat/hisilicon/hi3798mv300
PLAT_INCLUDES += -I${MV300_PLAT}/include

include lib/xlat_tables_v2/xlat_tables.mk
include drivers/arm/gic/v2/gicv2.mk

BL31_SOURCES += ${XLAT_TABLES_LIB_SRCS} ${GICV2_SOURCES} \
	drivers/arm/pl011/aarch64/pl011_console.S \
	drivers/delay_timer/delay_timer.c \
	drivers/delay_timer/generic_delay_timer.c \
	lib/cpus/aarch64/cortex_a53.S \
	plat/common/plat_gicv2.c plat/common/plat_psci_common.c \
	${MV300_PLAT}/aarch64/mv300_helpers.S \
	${MV300_PLAT}/bl31_plat_setup.c \
	${MV300_PLAT}/mv300_gicv2.c \
	${MV300_PLAT}/plat_topology.c \
	${MV300_PLAT}/plat_pm.c

RESET_TO_BL31 := 1
PROGRAMMABLE_RESET_ADDRESS := 1
COLD_BOOT_SINGLE_CPU := 1
PRELOADED_BL33_BASE := 0x02000000
SEPARATE_CODE_AND_RODATA := 1
USE_COHERENT_MEM := 0
# Four coherent Cortex-A53 cores in one cluster, as verified on this board.
WARMBOOT_ENABLE_DCACHE_EARLY := 1
ENABLE_SVE_FOR_NS := 0
ENABLE_SPE_FOR_NS := 0
ENABLE_FEAT_MPAM := 0
ERRATA_A53_835769 := 1
ERRATA_A53_843419 := 1
ERRATA_A53_855873 := 1
ERRATA_A53_1530924 := 1

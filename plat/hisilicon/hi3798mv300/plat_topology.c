/* SPDX-License-Identifier: BSD-3-Clause */
#include <arch.h>
#include <plat/common/platform.h>
#include <platform_def.h>

static const unsigned char topology[] = {
	PLATFORM_CLUSTER_COUNT,
	PLATFORM_MAX_CPUS_PER_CLUSTER,
};

const unsigned char *plat_get_power_domain_tree_desc(void)
{
	return topology;
}

int plat_core_pos_by_mpidr(u_register_t mpidr)
{
	u_register_t affinity = mpidr & MPIDR_AFFINITY_MASK;

	return affinity < PLATFORM_CORE_COUNT ? (int)affinity : -1;
}

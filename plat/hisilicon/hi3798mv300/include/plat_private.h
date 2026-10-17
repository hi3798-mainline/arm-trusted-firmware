/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef MV300_PLAT_PRIVATE_H
#define MV300_PLAT_PRIVATE_H

void mv300_gic_init(void);
void mv300_gic_pcpu_setup(void);
void mv300_warm_entry(void);

#endif /* MV300_PLAT_PRIVATE_H */

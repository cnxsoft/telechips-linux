/* arch/arm/plat-tcc/nand.c
 *
 * Copyright (C) 2010 Telechips, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#include <linux/mutex.h>
#include <linux/hardirq.h>
#include <asm/mach/flash.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <plat/nand.h>

#include <asm/setup.h>

#define ATAG_TCC_PARTITION 0x54434370 /* TCCp*/
#define TCC_PART_NAME_LEN 16

#define TCC_MAX_PARTITIONS 16

struct tcc_ptbl_entry {
	char name[TCC_PART_NAME_LEN];
	__u64 offset;
	__u64 size;
	__u32 flags;
};

static int tcc_nand_num_partitions;
static struct mtd_partition tcc_nand_partitions[TCC_MAX_PARTITIONS];
static char tcc_nand_names[TCC_MAX_PARTITIONS * TCC_PART_NAME_LEN];

static int __init parse_tag_tcc_partition(const struct tag *tag)
{
	struct mtd_partition *ptn = tcc_nand_partitions;
	char *name = tcc_nand_names;
	struct tcc_ptbl_entry *entry = (void *) &tag->u;
	unsigned count, n;

	count = (tag->hdr.size - 2) /
		(sizeof(struct tcc_ptbl_entry) / sizeof(__u32));

	if (count > TCC_MAX_PARTITIONS)
		count = TCC_MAX_PARTITIONS;

	for (n = 0; n < count; n++) {
		memcpy(name, entry->name, 15);
		name[15] = 0;

		ptn->name = name;
		ptn->offset = (unsigned long long)entry->offset;
		ptn->size = (unsigned long long)entry->size;

		name += 16;
		entry++;
		ptn++;
	}
	tcc_nand_num_partitions = count;

	return 0;
}
__tagtable(ATAG_TCC_PARTITION, parse_tag_tcc_partition);

void tcc_get_partition_info(struct mtd_partition **parts, int *nr_parts)
{
	*parts = tcc_nand_partitions;
	*nr_parts = tcc_nand_num_partitions;
}

/* Protect critical section between NAND block and MTD (or direct access) */
static DEFINE_MUTEX(tcc_nand_mutex);

void tcc_nand_lock()
{
	if (in_atomic())
		return;
	mutex_lock(&tcc_nand_mutex);
}
EXPORT_SYMBOL(tcc_nand_lock);

void tcc_nand_unlock()
{
	if (in_atomic())
		return;
	mutex_unlock(&tcc_nand_mutex);
}
EXPORT_SYMBOL(tcc_nand_unlock);

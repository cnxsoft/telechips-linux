/* linux/arch/arm/plat-tcc/pmap.c
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

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <asm/setup.h>

#include <plat/pmap.h>

#define ATAG_TCC_PMAP	0x5443436d	/* TCCm */

#define MAX_PMAPS		32

static int num_pmaps = 0;
static pmap_t pmap_table[MAX_PMAPS];

static struct proc_dir_entry *pmap_proc_entry;

int pmap_get_info(const char *name, pmap_t *mem)
{
	int i;

	for (i = 0; i < num_pmaps; i++) {
		if (strcmp(name, pmap_table[i].name) == 0) {
			memcpy(mem, &pmap_table[i], sizeof(pmap_t));
			return 1;
		}
	}
	return 0;
}
EXPORT_SYMBOL(pmap_get_info);

static int __init parse_tag_tcc_pmap(const struct tag *tag)
{
	pmap_t *pmap = pmap_table;
	pmap_t *entry = (void *) &tag->u;
	unsigned count, n;

	count = (tag->hdr.size - 2) / ((sizeof(pmap_t) / sizeof(__u32)));
	if (count > MAX_PMAPS)
		count = MAX_PMAPS;

	for (n = 0; n < count; n++) {
		memcpy(pmap->name, entry->name, TCC_PMAP_NAME_LEN - 1);
		pmap->name[TCC_PMAP_NAME_LEN - 1] = 0;
		pmap->base = entry->base;
		pmap->size = entry->size;
		entry++;
		pmap++;
	}
	num_pmaps = count;
	return 0;
}
__tagtable(ATAG_TCC_PMAP, parse_tag_tcc_pmap);

static inline int pmap_proc_info(char *buf, const char *name,
			    unsigned int base_addr, unsigned int size)
{
	return sprintf(buf, "0x%8.8x 0x%8.8x %s\n", base_addr, size, name);
}

static int pmap_read_proc(char *page, char **start, off_t off,
			      int count, int *eof, void *data)
{
	int i, len;

	len = sprintf(page, "%-10s %-10s %s\n", "base_addr", "size", "name");
	for (i = 0; i < num_pmaps; i++) {
		len += pmap_proc_info(page + len, pmap_table[i].name,
				      pmap_table[i].base, pmap_table[i].size);
	}
	return len;
}

static int __init tcc_pmap_init(void)
{
	pmap_proc_entry = create_proc_entry("pmap", 0444, NULL);
	if (pmap_proc_entry) {
		pmap_proc_entry->read_proc = pmap_read_proc;
	}
	return 0;
}
postcore_initcall(tcc_pmap_init);

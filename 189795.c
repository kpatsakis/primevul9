static int pagemap_pte_hole(unsigned long start, unsigned long end,
				void *private)
{
	struct pagemapread *pm = private;
	unsigned long addr;
	int err = 0;
	for (addr = start; addr < end; addr += PAGE_SIZE) {
		err = add_to_pagemap(addr, PM_NOT_PRESENT, pm);
		if (err)
			break;
	}
	return err;
}
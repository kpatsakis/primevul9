static int __gup_device_huge_pud(pud_t orig, pud_t *pudp, unsigned long addr,
				 unsigned long end, unsigned int flags,
				 struct page **pages, int *nr)
{
	unsigned long fault_pfn;
	int nr_start = *nr;

	fault_pfn = pud_pfn(orig) + ((addr & ~PUD_MASK) >> PAGE_SHIFT);
	if (!__gup_device_huge(fault_pfn, addr, end, flags, pages, nr))
		return 0;

	if (unlikely(pud_val(orig) != pud_val(*pudp))) {
		undo_dev_pagemap(nr, nr_start, flags, pages);
		return 0;
	}
	return 1;
}
static int gup_p4d_range(pgd_t pgd, unsigned long addr, unsigned long end,
			 unsigned int flags, struct page **pages, int *nr)
{
	unsigned long next;
	p4d_t *p4dp;

	p4dp = p4d_offset(&pgd, addr);
	do {
		p4d_t p4d = READ_ONCE(*p4dp);

		next = p4d_addr_end(addr, end);
		if (p4d_none(p4d))
			return 0;
		BUILD_BUG_ON(p4d_huge(p4d));
		if (unlikely(is_hugepd(__hugepd(p4d_val(p4d))))) {
			if (!gup_huge_pd(__hugepd(p4d_val(p4d)), addr,
					 P4D_SHIFT, next, flags, pages, nr))
				return 0;
		} else if (!gup_pud_range(p4d, addr, next, flags, pages, nr))
			return 0;
	} while (p4dp++, addr = next, addr != end);

	return 1;
}
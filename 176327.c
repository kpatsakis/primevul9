static int gup_huge_pd(hugepd_t hugepd, unsigned long addr,
		unsigned int pdshift, unsigned long end, unsigned int flags,
		struct page **pages, int *nr)
{
	pte_t *ptep;
	unsigned long sz = 1UL << hugepd_shift(hugepd);
	unsigned long next;

	ptep = hugepte_offset(hugepd, addr, pdshift);
	do {
		next = hugepte_addr_end(addr, end, sz);
		if (!gup_hugepte(ptep, sz, addr, end, flags, pages, nr))
			return 0;
	} while (ptep++, addr = next, addr != end);

	return 1;
}
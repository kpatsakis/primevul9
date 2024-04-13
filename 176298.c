static int gup_pud_range(p4d_t p4d, unsigned long addr, unsigned long end,
			 unsigned int flags, struct page **pages, int *nr)
{
	unsigned long next;
	pud_t *pudp;

	pudp = pud_offset(&p4d, addr);
	do {
		pud_t pud = READ_ONCE(*pudp);

		next = pud_addr_end(addr, end);
		if (unlikely(!pud_present(pud)))
			return 0;
		if (unlikely(pud_huge(pud))) {
			if (!gup_huge_pud(pud, pudp, addr, next, flags,
					  pages, nr))
				return 0;
		} else if (unlikely(is_hugepd(__hugepd(pud_val(pud))))) {
			if (!gup_huge_pd(__hugepd(pud_val(pud)), addr,
					 PUD_SHIFT, next, flags, pages, nr))
				return 0;
		} else if (!gup_pmd_range(pud, addr, next, flags, pages, nr))
			return 0;
	} while (pudp++, addr = next, addr != end);

	return 1;
}
static int gup_huge_pgd(pgd_t orig, pgd_t *pgdp, unsigned long addr,
			unsigned long end, unsigned int flags,
			struct page **pages, int *nr)
{
	int refs;
	struct page *head, *page;

	if (!pgd_access_permitted(orig, flags & FOLL_WRITE))
		return 0;

	BUILD_BUG_ON(pgd_devmap(orig));

	page = pgd_page(orig) + ((addr & ~PGDIR_MASK) >> PAGE_SHIFT);
	refs = record_subpages(page, addr, end, pages + *nr);

	head = try_grab_compound_head(pgd_page(orig), refs, flags);
	if (!head)
		return 0;

	if (unlikely(pgd_val(orig) != pgd_val(*pgdp))) {
		put_compound_head(head, refs, flags);
		return 0;
	}

	*nr += refs;
	SetPageReferenced(head);
	return 1;
}
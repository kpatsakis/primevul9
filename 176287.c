static struct page *follow_page_mask(struct vm_area_struct *vma,
			      unsigned long address, unsigned int flags,
			      struct follow_page_context *ctx)
{
	pgd_t *pgd;
	struct page *page;
	struct mm_struct *mm = vma->vm_mm;

	ctx->page_mask = 0;

	/* make this handle hugepd */
	page = follow_huge_addr(mm, address, flags & FOLL_WRITE);
	if (!IS_ERR(page)) {
		WARN_ON_ONCE(flags & (FOLL_GET | FOLL_PIN));
		return page;
	}

	pgd = pgd_offset(mm, address);

	if (pgd_none(*pgd) || unlikely(pgd_bad(*pgd)))
		return no_page_table(vma, flags);

	if (pgd_huge(*pgd)) {
		page = follow_huge_pgd(mm, address, pgd, flags);
		if (page)
			return page;
		return no_page_table(vma, flags);
	}
	if (is_hugepd(__hugepd(pgd_val(*pgd)))) {
		page = follow_huge_pd(vma, address,
				      __hugepd(pgd_val(*pgd)), flags,
				      PGDIR_SHIFT);
		if (page)
			return page;
		return no_page_table(vma, flags);
	}

	return follow_p4d_mask(vma, address, pgd, flags, ctx);
}
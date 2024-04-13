static int record_subpages(struct page *page, unsigned long addr,
			   unsigned long end, struct page **pages)
{
	int nr;

	for (nr = 0; addr != end; addr += PAGE_SIZE)
		pages[nr++] = page++;

	return nr;
}
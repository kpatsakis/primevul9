void unpin_user_pages(struct page **pages, unsigned long npages)
{
	unsigned long index;

	/*
	 * TODO: this can be optimized for huge pages: if a series of pages is
	 * physically contiguous and part of the same compound page, then a
	 * single operation to the head page should suffice.
	 */
	for (index = 0; index < npages; index++)
		unpin_user_page(pages[index]);
}
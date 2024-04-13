static void dma_free_pagelist(struct page *freelist)
{
	struct page *pg;

	while ((pg = freelist)) {
		freelist = pg->freelist;
		free_pgtable_page(page_address(pg));
	}
}
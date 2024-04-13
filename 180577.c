static void iova_entry_free(unsigned long data)
{
	struct page *freelist = (struct page *)data;

	dma_free_pagelist(freelist);
}
void free_highmem_page(struct page *page)
{
	__free_reserved_page(page);
	totalram_pages++;
	page_zone(page)->managed_pages++;
	totalhigh_pages++;
}
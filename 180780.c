void adjust_managed_page_count(struct page *page, long count)
{
	spin_lock(&managed_page_count_lock);
	page_zone(page)->managed_pages += count;
	totalram_pages += count;
#ifdef CONFIG_HIGHMEM
	if (PageHighMem(page))
		totalhigh_pages += count;
#endif
	spin_unlock(&managed_page_count_lock);
}
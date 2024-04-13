static void change_pageblock_range(struct page *pageblock_page,
					int start_order, int migratetype)
{
	int nr_pageblocks = 1 << (start_order - pageblock_order);

	while (nr_pageblocks--) {
		set_pageblock_migratetype(pageblock_page, migratetype);
		pageblock_page += pageblock_nr_pages;
	}
}
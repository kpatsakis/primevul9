static void soft_offline_free_page(struct page *page)
{
	if (PageHuge(page)) {
		struct page *hpage = compound_head(page);

		set_page_hwpoison_huge_page(hpage);
		if (!dequeue_hwpoisoned_huge_page(hpage))
			num_poisoned_pages_add(1 << compound_order(hpage));
	} else {
		if (!TestSetPageHWPoison(page))
			num_poisoned_pages_inc();
	}
}
static void clear_page_hwpoison_huge_page(struct page *hpage)
{
	int i;
	int nr_pages = 1 << compound_order(hpage);
	for (i = 0; i < nr_pages; i++)
		ClearPageHWPoison(hpage + i);
}
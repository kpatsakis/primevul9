static int soft_offline_in_use_page(struct page *page, int flags)
{
	int ret;
	struct page *hpage = compound_head(page);

	if (!PageHuge(page) && PageTransHuge(hpage)) {
		lock_page(hpage);
		if (!PageAnon(hpage) || unlikely(split_huge_page(hpage))) {
			unlock_page(hpage);
			if (!PageAnon(hpage))
				pr_info("soft offline: %#lx: non anonymous thp\n", page_to_pfn(page));
			else
				pr_info("soft offline: %#lx: thp split failed\n", page_to_pfn(page));
			put_hwpoison_page(hpage);
			return -EBUSY;
		}
		unlock_page(hpage);
		get_hwpoison_page(page);
		put_hwpoison_page(hpage);
	}

	if (PageHuge(page))
		ret = soft_offline_huge_page(page, flags);
	else
		ret = __soft_offline_page(page, flags);

	return ret;
}
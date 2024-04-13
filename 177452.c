int soft_offline_page(struct page *page, int flags)
{
	int ret;
	unsigned long pfn = page_to_pfn(page);

	if (is_zone_device_page(page)) {
		pr_debug_ratelimited("soft_offline: %#lx page is device page\n",
				pfn);
		if (flags & MF_COUNT_INCREASED)
			put_page(page);
		return -EIO;
	}

	if (PageHWPoison(page)) {
		pr_info("soft offline: %#lx page already poisoned\n", pfn);
		if (flags & MF_COUNT_INCREASED)
			put_hwpoison_page(page);
		return -EBUSY;
	}

	get_online_mems();
	ret = get_any_page(page, pfn, flags);
	put_online_mems();

	if (ret > 0)
		ret = soft_offline_in_use_page(page, flags);
	else if (ret == 0)
		ret = soft_offline_free_page(page);

	return ret;
}
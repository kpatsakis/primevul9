static int __get_any_page(struct page *p, unsigned long pfn, int flags)
{
	int ret;

	if (flags & MF_COUNT_INCREASED)
		return 1;

	/*
	 * When the target page is a free hugepage, just remove it
	 * from free hugepage list.
	 */
	if (!get_hwpoison_page(p)) {
		if (PageHuge(p)) {
			pr_info("%s: %#lx free huge page\n", __func__, pfn);
			ret = 0;
		} else if (is_free_buddy_page(p)) {
			pr_info("%s: %#lx free buddy page\n", __func__, pfn);
			ret = 0;
		} else {
			pr_info("%s: %#lx: unknown zero refcount page type %lx\n",
				__func__, pfn, p->flags);
			ret = -EIO;
		}
	} else {
		/* Not a free page */
		ret = 1;
	}
	return ret;
}
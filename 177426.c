int get_hwpoison_page(struct page *page)
{
	struct page *head = compound_head(page);

	if (!PageHuge(head) && PageTransHuge(head)) {
		/*
		 * Non anonymous thp exists only in allocation/free time. We
		 * can't handle such a case correctly, so let's give it up.
		 * This should be better than triggering BUG_ON when kernel
		 * tries to touch the "partially handled" page.
		 */
		if (!PageAnon(head)) {
			pr_err("Memory failure: %#lx: non anonymous thp\n",
				page_to_pfn(page));
			return 0;
		}
	}

	if (get_page_unless_zero(head)) {
		if (head == compound_head(page))
			return 1;

		pr_info("Memory failure: %#lx cannot catch tail\n",
			page_to_pfn(page));
		put_page(head);
	}

	return 0;
}
static int soft_offline_free_page(struct page *page)
{
	int rc = 0;
	struct page *head = compound_head(page);

	if (PageHuge(head))
		rc = dissolve_free_huge_page(page);
	if (!rc) {
		if (set_hwpoison_free_buddy_page(page))
			num_poisoned_pages_inc();
		else
			rc = -EBUSY;
	}
	return rc;
}
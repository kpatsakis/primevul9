static void scrub_page_put(struct scrub_page *spage)
{
	if (atomic_dec_and_test(&spage->refs)) {
		if (spage->page)
			__free_page(spage->page);
		kfree(spage);
	}
}
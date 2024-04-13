static void nfs4_free_pages(struct page **pages, size_t size)
{
	int i;

	if (!pages)
		return;

	for (i = 0; i < size; i++) {
		if (!pages[i])
			break;
		__free_page(pages[i]);
	}
	kfree(pages);
}
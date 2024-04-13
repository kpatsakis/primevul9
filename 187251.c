static struct page **nfs4_alloc_pages(size_t size, gfp_t gfp_flags)
{
	struct page **pages;
	int i;

	pages = kcalloc(size, sizeof(struct page *), gfp_flags);
	if (!pages) {
		dprintk("%s: can't alloc array of %zu pages\n", __func__, size);
		return NULL;
	}

	for (i = 0; i < size; i++) {
		pages[i] = alloc_page(gfp_flags);
		if (!pages[i]) {
			dprintk("%s: failed to allocate page\n", __func__);
			nfs4_free_pages(pages, size);
			return NULL;
		}
	}

	return pages;
}
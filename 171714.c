int af_alg_make_sg(struct af_alg_sgl *sgl, struct iov_iter *iter, int len)
{
	size_t off;
	ssize_t n;
	int npages, i;

	n = iov_iter_get_pages(iter, sgl->pages, len, ALG_MAX_PAGES, &off);
	if (n < 0)
		return n;

	npages = (off + n + PAGE_SIZE - 1) >> PAGE_SHIFT;
	if (WARN_ON(npages == 0))
		return -EINVAL;
	/* Add one extra for linking */
	sg_init_table(sgl->sg, npages + 1);

	for (i = 0, len = n; i < npages; i++) {
		int plen = min_t(int, len, PAGE_SIZE - off);

		sg_set_page(sgl->sg + i, sgl->pages[i], plen, off);

		off = 0;
		len -= plen;
	}
	sg_mark_end(sgl->sg + npages - 1);
	sgl->npages = npages;

	return n;
}
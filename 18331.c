static ssize_t iter_xarray_populate_pages(struct page **pages, struct xarray *xa,
					  pgoff_t index, unsigned int nr_pages)
{
	XA_STATE(xas, xa, index);
	struct page *page;
	unsigned int ret = 0;

	rcu_read_lock();
	for (page = xas_load(&xas); page; page = xas_next(&xas)) {
		if (xas_retry(&xas, page))
			continue;

		/* Has the page moved or been split? */
		if (unlikely(page != xas_reload(&xas))) {
			xas_reset(&xas);
			continue;
		}

		pages[ret] = find_subpage(page, xas.xa_index);
		get_page(pages[ret]);
		if (++ret == nr_pages)
			break;
	}
	rcu_read_unlock();
	return ret;
}
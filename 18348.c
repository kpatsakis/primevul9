static ssize_t iter_xarray_get_pages_alloc(struct iov_iter *i,
					   struct page ***pages, size_t maxsize,
					   size_t *_start_offset)
{
	struct page **p;
	unsigned nr, offset;
	pgoff_t index, count;
	size_t size = maxsize, actual;
	loff_t pos;

	if (!size)
		return 0;

	pos = i->xarray_start + i->iov_offset;
	index = pos >> PAGE_SHIFT;
	offset = pos & ~PAGE_MASK;
	*_start_offset = offset;

	count = 1;
	if (size > PAGE_SIZE - offset) {
		size -= PAGE_SIZE - offset;
		count += size >> PAGE_SHIFT;
		size &= ~PAGE_MASK;
		if (size)
			count++;
	}

	p = get_pages_array(count);
	if (!p)
		return -ENOMEM;
	*pages = p;

	nr = iter_xarray_populate_pages(p, i->xarray, index, count);
	if (nr == 0)
		return 0;

	actual = PAGE_SIZE * nr;
	actual -= offset;
	if (nr == count && size > 0) {
		unsigned last_offset = (nr > 1) ? 0 : offset;
		actual -= PAGE_SIZE - (last_offset + size);
	}
	return actual;
}
static int iov_npages(const struct iov_iter *i, int maxpages)
{
	size_t skip = i->iov_offset, size = i->count;
	const struct iovec *p;
	int npages = 0;

	for (p = i->iov; size; skip = 0, p++) {
		unsigned offs = offset_in_page(p->iov_base + skip);
		size_t len = min(p->iov_len - skip, size);

		if (len) {
			size -= len;
			npages += DIV_ROUND_UP(offs + len, PAGE_SIZE);
			if (unlikely(npages > maxpages))
				return maxpages;
		}
	}
	return npages;
}
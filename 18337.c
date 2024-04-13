static unsigned long first_iovec_segment(const struct iov_iter *i,
					 size_t *size, size_t *start,
					 size_t maxsize, unsigned maxpages)
{
	size_t skip;
	long k;

	for (k = 0, skip = i->iov_offset; k < i->nr_segs; k++, skip = 0) {
		unsigned long addr = (unsigned long)i->iov[k].iov_base + skip;
		size_t len = i->iov[k].iov_len - skip;

		if (unlikely(!len))
			continue;
		if (len > maxsize)
			len = maxsize;
		len += (*start = addr % PAGE_SIZE);
		if (len > maxpages * PAGE_SIZE)
			len = maxpages * PAGE_SIZE;
		*size = len;
		return addr & PAGE_MASK;
	}
	BUG(); // if it had been empty, we wouldn't get called
}
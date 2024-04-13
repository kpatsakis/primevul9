unsigned long iov_iter_gap_alignment(const struct iov_iter *i)
{
	unsigned long res = 0;
	unsigned long v = 0;
	size_t size = i->count;
	unsigned k;

	if (WARN_ON(!iter_is_iovec(i)))
		return ~0U;

	for (k = 0; k < i->nr_segs; k++) {
		if (i->iov[k].iov_len) {
			unsigned long base = (unsigned long)i->iov[k].iov_base;
			if (v) // if not the first one
				res |= base | v; // this start | previous end
			v = base + i->iov[k].iov_len;
			if (size <= i->iov[k].iov_len)
				break;
			size -= i->iov[k].iov_len;
		}
	}
	return res;
}
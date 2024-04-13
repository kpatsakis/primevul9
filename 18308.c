static unsigned long iov_iter_alignment_iovec(const struct iov_iter *i)
{
	unsigned long res = 0;
	size_t size = i->count;
	size_t skip = i->iov_offset;
	unsigned k;

	for (k = 0; k < i->nr_segs; k++, skip = 0) {
		size_t len = i->iov[k].iov_len - skip;
		if (len) {
			res |= (unsigned long)i->iov[k].iov_base + skip;
			if (len > size)
				len = size;
			res |= len;
			size -= len;
			if (!size)
				break;
		}
	}
	return res;
}
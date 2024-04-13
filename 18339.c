size_t fault_in_iov_iter_readable(const struct iov_iter *i, size_t size)
{
	if (iter_is_iovec(i)) {
		size_t count = min(size, iov_iter_count(i));
		const struct iovec *p;
		size_t skip;

		size -= count;
		for (p = i->iov, skip = i->iov_offset; count; p++, skip = 0) {
			size_t len = min(count, p->iov_len - skip);
			size_t ret;

			if (unlikely(!len))
				continue;
			ret = fault_in_readable(p->iov_base + skip, len);
			count -= len - ret;
			if (ret)
				break;
		}
		return count + size;
	}
	return 0;
}
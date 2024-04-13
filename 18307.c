static size_t copy_page_to_iter_iovec(struct page *page, size_t offset, size_t bytes,
			 struct iov_iter *i)
{
	size_t skip, copy, left, wanted;
	const struct iovec *iov;
	char __user *buf;
	void *kaddr, *from;

	if (unlikely(bytes > i->count))
		bytes = i->count;

	if (unlikely(!bytes))
		return 0;

	might_fault();
	wanted = bytes;
	iov = i->iov;
	skip = i->iov_offset;
	buf = iov->iov_base + skip;
	copy = min(bytes, iov->iov_len - skip);

	if (IS_ENABLED(CONFIG_HIGHMEM) && !fault_in_writeable(buf, copy)) {
		kaddr = kmap_atomic(page);
		from = kaddr + offset;

		/* first chunk, usually the only one */
		left = copyout(buf, from, copy);
		copy -= left;
		skip += copy;
		from += copy;
		bytes -= copy;

		while (unlikely(!left && bytes)) {
			iov++;
			buf = iov->iov_base;
			copy = min(bytes, iov->iov_len);
			left = copyout(buf, from, copy);
			copy -= left;
			skip = copy;
			from += copy;
			bytes -= copy;
		}
		if (likely(!bytes)) {
			kunmap_atomic(kaddr);
			goto done;
		}
		offset = from - kaddr;
		buf += copy;
		kunmap_atomic(kaddr);
		copy = min(bytes, iov->iov_len - skip);
	}
	/* Too bad - revert to non-atomic kmap */

	kaddr = kmap(page);
	from = kaddr + offset;
	left = copyout(buf, from, copy);
	copy -= left;
	skip += copy;
	from += copy;
	bytes -= copy;
	while (unlikely(!left && bytes)) {
		iov++;
		buf = iov->iov_base;
		copy = min(bytes, iov->iov_len);
		left = copyout(buf, from, copy);
		copy -= left;
		skip = copy;
		from += copy;
		bytes -= copy;
	}
	kunmap(page);

done:
	if (skip == iov->iov_len) {
		iov++;
		skip = 0;
	}
	i->count -= wanted - bytes;
	i->nr_segs -= iov - i->iov;
	i->iov = iov;
	i->iov_offset = skip;
	return wanted - bytes;
}
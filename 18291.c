static size_t __copy_page_to_iter(struct page *page, size_t offset, size_t bytes,
			 struct iov_iter *i)
{
	if (likely(iter_is_iovec(i)))
		return copy_page_to_iter_iovec(page, offset, bytes, i);
	if (iov_iter_is_bvec(i) || iov_iter_is_kvec(i) || iov_iter_is_xarray(i)) {
		void *kaddr = kmap_local_page(page);
		size_t wanted = _copy_to_iter(kaddr + offset, bytes, i);
		kunmap_local(kaddr);
		return wanted;
	}
	if (iov_iter_is_pipe(i))
		return copy_page_to_iter_pipe(page, offset, bytes, i);
	if (unlikely(iov_iter_is_discard(i))) {
		if (unlikely(i->count < bytes))
			bytes = i->count;
		i->count -= bytes;
		return bytes;
	}
	WARN_ON(1);
	return 0;
}
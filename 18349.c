size_t copy_page_from_iter(struct page *page, size_t offset, size_t bytes,
			 struct iov_iter *i)
{
	if (unlikely(!page_copy_sane(page, offset, bytes)))
		return 0;
	if (likely(iter_is_iovec(i)))
		return copy_page_from_iter_iovec(page, offset, bytes, i);
	if (iov_iter_is_bvec(i) || iov_iter_is_kvec(i) || iov_iter_is_xarray(i)) {
		void *kaddr = kmap_local_page(page);
		size_t wanted = _copy_from_iter(kaddr + offset, bytes, i);
		kunmap_local(kaddr);
		return wanted;
	}
	WARN_ON(1);
	return 0;
}
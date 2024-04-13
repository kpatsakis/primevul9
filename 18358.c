size_t copy_page_from_iter_atomic(struct page *page, unsigned offset, size_t bytes,
				  struct iov_iter *i)
{
	char *kaddr = kmap_atomic(page), *p = kaddr + offset;
	if (unlikely(!page_copy_sane(page, offset, bytes))) {
		kunmap_atomic(kaddr);
		return 0;
	}
	if (unlikely(iov_iter_is_pipe(i) || iov_iter_is_discard(i))) {
		kunmap_atomic(kaddr);
		WARN_ON(1);
		return 0;
	}
	iterate_and_advance(i, bytes, base, len, off,
		copyin(p + off, base, len),
		memcpy(p + off, base, len)
	)
	kunmap_atomic(kaddr);
	return bytes;
}
size_t copy_page_to_iter(struct page *page, size_t offset, size_t bytes,
			 struct iov_iter *i)
{
	size_t res = 0;
	if (unlikely(!page_copy_sane(page, offset, bytes)))
		return 0;
	page += offset / PAGE_SIZE; // first subpage
	offset %= PAGE_SIZE;
	while (1) {
		size_t n = __copy_page_to_iter(page, offset,
				min(bytes, (size_t)PAGE_SIZE - offset), i);
		res += n;
		bytes -= n;
		if (!bytes || !n)
			break;
		offset += n;
		if (offset == PAGE_SIZE) {
			page++;
			offset = 0;
		}
	}
	return res;
}
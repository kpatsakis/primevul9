static struct page *first_bvec_segment(const struct iov_iter *i,
				       size_t *size, size_t *start,
				       size_t maxsize, unsigned maxpages)
{
	struct page *page;
	size_t skip = i->iov_offset, len;

	len = i->bvec->bv_len - skip;
	if (len > maxsize)
		len = maxsize;
	skip += i->bvec->bv_offset;
	page = i->bvec->bv_page + skip / PAGE_SIZE;
	len += (*start = skip % PAGE_SIZE);
	if (len > maxpages * PAGE_SIZE)
		len = maxpages * PAGE_SIZE;
	*size = len;
	return page;
}
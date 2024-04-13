static int bvec_npages(const struct iov_iter *i, int maxpages)
{
	size_t skip = i->iov_offset, size = i->count;
	const struct bio_vec *p;
	int npages = 0;

	for (p = i->bvec; size; skip = 0, p++) {
		unsigned offs = (p->bv_offset + skip) % PAGE_SIZE;
		size_t len = min(p->bv_len - skip, size);

		size -= len;
		npages += DIV_ROUND_UP(offs + len, PAGE_SIZE);
		if (unlikely(npages > maxpages))
			return maxpages;
	}
	return npages;
}
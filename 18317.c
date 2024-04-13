size_t iov_iter_single_seg_count(const struct iov_iter *i)
{
	if (i->nr_segs > 1) {
		if (likely(iter_is_iovec(i) || iov_iter_is_kvec(i)))
			return min(i->count, i->iov->iov_len - i->iov_offset);
		if (iov_iter_is_bvec(i))
			return min(i->count, i->bvec->bv_len - i->iov_offset);
	}
	return i->count;
}
void iov_iter_restore(struct iov_iter *i, struct iov_iter_state *state)
{
	if (WARN_ON_ONCE(!iov_iter_is_bvec(i) && !iter_is_iovec(i)) &&
			 !iov_iter_is_kvec(i))
		return;
	i->iov_offset = state->iov_offset;
	i->count = state->count;
	/*
	 * For the *vec iters, nr_segs + iov is constant - if we increment
	 * the vec, then we also decrement the nr_segs count. Hence we don't
	 * need to track both of these, just one is enough and we can deduct
	 * the other from that. ITER_KVEC and ITER_IOVEC are the same struct
	 * size, so we can just increment the iov pointer as they are unionzed.
	 * ITER_BVEC _may_ be the same size on some archs, but on others it is
	 * not. Be safe and handle it separately.
	 */
	BUILD_BUG_ON(sizeof(struct iovec) != sizeof(struct kvec));
	if (iov_iter_is_bvec(i))
		i->bvec -= state->nr_segs - i->nr_segs;
	else
		i->iov -= state->nr_segs - i->nr_segs;
	i->nr_segs = state->nr_segs;
}
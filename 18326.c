const void *dup_iter(struct iov_iter *new, struct iov_iter *old, gfp_t flags)
{
	*new = *old;
	if (unlikely(iov_iter_is_pipe(new))) {
		WARN_ON(1);
		return NULL;
	}
	if (unlikely(iov_iter_is_discard(new) || iov_iter_is_xarray(new)))
		return NULL;
	if (iov_iter_is_bvec(new))
		return new->bvec = kmemdup(new->bvec,
				    new->nr_segs * sizeof(struct bio_vec),
				    flags);
	else
		/* iovec and kvec have identical layout */
		return new->iov = kmemdup(new->iov,
				   new->nr_segs * sizeof(struct iovec),
				   flags);
}
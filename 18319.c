size_t csum_and_copy_to_iter(const void *addr, size_t bytes, void *_csstate,
			     struct iov_iter *i)
{
	struct csum_state *csstate = _csstate;
	__wsum sum, next;

	if (unlikely(iov_iter_is_discard(i))) {
		WARN_ON(1);	/* for now */
		return 0;
	}

	sum = csum_shift(csstate->csum, csstate->off);
	if (unlikely(iov_iter_is_pipe(i)))
		bytes = csum_and_copy_to_pipe_iter(addr, bytes, i, &sum);
	else iterate_and_advance(i, bytes, base, len, off, ({
		next = csum_and_copy_to_user(addr + off, base, len);
		sum = csum_block_add(sum, next, off);
		next ? 0 : len;
	}), ({
		sum = csum_and_memcpy(base, addr + off, len, sum, off);
	})
	)
	csstate->csum = csum_shift(sum, csstate->off);
	csstate->off += bytes;
	return bytes;
}
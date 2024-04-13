size_t csum_and_copy_from_iter(void *addr, size_t bytes, __wsum *csum,
			       struct iov_iter *i)
{
	__wsum sum, next;
	sum = *csum;
	if (unlikely(iov_iter_is_pipe(i) || iov_iter_is_discard(i))) {
		WARN_ON(1);
		return 0;
	}
	iterate_and_advance(i, bytes, base, len, off, ({
		next = csum_and_copy_from_user(base, addr + off, len);
		sum = csum_block_add(sum, next, off);
		next ? 0 : len;
	}), ({
		sum = csum_and_memcpy(addr + off, base, len, sum, off);
	})
	)
	*csum = sum;
	return bytes;
}
size_t _copy_from_iter_flushcache(void *addr, size_t bytes, struct iov_iter *i)
{
	if (unlikely(iov_iter_is_pipe(i))) {
		WARN_ON(1);
		return 0;
	}
	iterate_and_advance(i, bytes, base, len, off,
		__copy_from_user_flushcache(addr + off, base, len),
		memcpy_flushcache(addr + off, base, len)
	)

	return bytes;
}
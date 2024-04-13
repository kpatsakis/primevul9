size_t _copy_to_iter(const void *addr, size_t bytes, struct iov_iter *i)
{
	if (unlikely(iov_iter_is_pipe(i)))
		return copy_pipe_to_iter(addr, bytes, i);
	if (iter_is_iovec(i))
		might_fault();
	iterate_and_advance(i, bytes, base, len, off,
		copyout(base, addr + off, len),
		memcpy(base, addr + off, len)
	)

	return bytes;
}
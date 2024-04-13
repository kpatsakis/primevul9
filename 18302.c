size_t iov_iter_zero(size_t bytes, struct iov_iter *i)
{
	if (unlikely(iov_iter_is_pipe(i)))
		return pipe_zero(bytes, i);
	iterate_and_advance(i, bytes, base, len, count,
		clear_user(base, len),
		memset(base, 0, len)
	)

	return bytes;
}
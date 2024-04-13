static int move_iovec_hdr(struct iovec *from, struct iovec *to,
			  size_t len, int iov_count)
{
	int seg = 0;
	size_t size;

	while (len && seg < iov_count) {
		size = min(from->iov_len, len);
		to->iov_base = from->iov_base;
		to->iov_len = size;
		from->iov_len -= size;
		from->iov_base += size;
		len -= size;
		++from;
		++to;
		++seg;
	}
	return seg;
}
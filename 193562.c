static void copy_iovec_hdr(const struct iovec *from, struct iovec *to,
			   size_t len, int iovcount)
{
	int seg = 0;
	size_t size;

	while (len && seg < iovcount) {
		size = min(from->iov_len, len);
		to->iov_base = from->iov_base;
		to->iov_len = size;
		len -= size;
		++from;
		++to;
		++seg;
	}
}
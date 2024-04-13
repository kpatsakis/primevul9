evbuffer_write_sendfile(struct evbuffer *buffer, evutil_socket_t dest_fd,
    ev_ssize_t howmuch)
{
	struct evbuffer_chain *chain = buffer->first;
	struct evbuffer_chain_file_segment *info =
	    EVBUFFER_CHAIN_EXTRA(struct evbuffer_chain_file_segment,
		chain);
	const int source_fd = info->segment->fd;
#if defined(SENDFILE_IS_MACOSX) || defined(SENDFILE_IS_FREEBSD)
	int res;
	ev_off_t len = chain->off;
#elif defined(SENDFILE_IS_LINUX) || defined(SENDFILE_IS_SOLARIS)
	ev_ssize_t res;
	ev_off_t offset = chain->misalign;
#endif

	ASSERT_EVBUFFER_LOCKED(buffer);

#if defined(SENDFILE_IS_MACOSX)
	res = sendfile(source_fd, dest_fd, chain->misalign, &len, NULL, 0);
	if (res == -1 && !EVUTIL_ERR_RW_RETRIABLE(errno))
		return (-1);

	return (len);
#elif defined(SENDFILE_IS_FREEBSD)
	res = sendfile(source_fd, dest_fd, chain->misalign, chain->off, NULL, &len, 0);
	if (res == -1 && !EVUTIL_ERR_RW_RETRIABLE(errno))
		return (-1);

	return (len);
#elif defined(SENDFILE_IS_LINUX)
	/* TODO(niels): implement splice */
	res = sendfile(dest_fd, source_fd, &offset, chain->off);
	if (res == -1 && EVUTIL_ERR_RW_RETRIABLE(errno)) {
		/* if this is EAGAIN or EINTR return 0; otherwise, -1 */
		return (0);
	}
	return (res);
#elif defined(SENDFILE_IS_SOLARIS)
	{
		const off_t offset_orig = offset;
		res = sendfile(dest_fd, source_fd, &offset, chain->off);
		if (res == -1 && EVUTIL_ERR_RW_RETRIABLE(errno)) {
			if (offset - offset_orig)
				return offset - offset_orig;
			/* if this is EAGAIN or EINTR and no bytes were
			 * written, return 0 */
			return (0);
		}
		return (res);
	}
#endif
}
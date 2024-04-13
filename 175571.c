evbuffer_write_atmost(struct evbuffer *buffer, evutil_socket_t fd,
    ev_ssize_t howmuch)
{
	int n = -1;

	EVBUFFER_LOCK(buffer);

	if (buffer->freeze_start) {
		goto done;
	}

	if (howmuch < 0 || (size_t)howmuch > buffer->total_len)
		howmuch = buffer->total_len;

	if (howmuch > 0) {
#ifdef USE_SENDFILE
		struct evbuffer_chain *chain = buffer->first;
		if (chain != NULL && (chain->flags & EVBUFFER_SENDFILE))
			n = evbuffer_write_sendfile(buffer, fd, howmuch);
		else {
#endif
#ifdef USE_IOVEC_IMPL
		n = evbuffer_write_iovec(buffer, fd, howmuch);
#elif defined(_WIN32)
		/* XXX(nickm) Don't disable this code until we know if
		 * the WSARecv code above works. */
		void *p = evbuffer_pullup(buffer, howmuch);
		EVUTIL_ASSERT(p || !howmuch);
		n = send(fd, p, howmuch, 0);
#else
		void *p = evbuffer_pullup(buffer, howmuch);
		EVUTIL_ASSERT(p || !howmuch);
		n = write(fd, p, howmuch);
#endif
#ifdef USE_SENDFILE
		}
#endif
	}

	if (n > 0)
		evbuffer_drain(buffer, n);

done:
	EVBUFFER_UNLOCK(buffer);
	return (n);
}
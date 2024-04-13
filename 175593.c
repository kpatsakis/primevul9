evbuffer_write_iovec(struct evbuffer *buffer, evutil_socket_t fd,
    ev_ssize_t howmuch)
{
	IOV_TYPE iov[NUM_WRITE_IOVEC];
	struct evbuffer_chain *chain = buffer->first;
	int n, i = 0;

	if (howmuch < 0)
		return -1;

	ASSERT_EVBUFFER_LOCKED(buffer);
	/* XXX make this top out at some maximal data length?  if the
	 * buffer has (say) 1MB in it, split over 128 chains, there's
	 * no way it all gets written in one go. */
	while (chain != NULL && i < NUM_WRITE_IOVEC && howmuch) {
#ifdef USE_SENDFILE
		/* we cannot write the file info via writev */
		if (chain->flags & EVBUFFER_SENDFILE)
			break;
#endif
		iov[i].IOV_PTR_FIELD = (void *) (chain->buffer + chain->misalign);
		if ((size_t)howmuch >= chain->off) {
			/* XXXcould be problematic when windows supports mmap*/
			iov[i++].IOV_LEN_FIELD = (IOV_LEN_TYPE)chain->off;
			howmuch -= chain->off;
		} else {
			/* XXXcould be problematic when windows supports mmap*/
			iov[i++].IOV_LEN_FIELD = (IOV_LEN_TYPE)howmuch;
			break;
		}
		chain = chain->next;
	}
	if (! i)
		return 0;

#ifdef _WIN32
	{
		DWORD bytesSent;
		if (WSASend(fd, iov, i, &bytesSent, 0, NULL, NULL))
			n = -1;
		else
			n = bytesSent;
	}
#else
	n = writev(fd, iov, i);
#endif
	return (n);
}
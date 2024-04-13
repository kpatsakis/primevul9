evbuffer_remove(struct evbuffer *buf, void *data_out, size_t datlen)
{
	ev_ssize_t n;
	EVBUFFER_LOCK(buf);
	n = evbuffer_copyout_from(buf, NULL, data_out, datlen);
	if (n > 0) {
		if (evbuffer_drain(buf, n)<0)
			n = -1;
	}
	EVBUFFER_UNLOCK(buf);
	return (int)n;
}
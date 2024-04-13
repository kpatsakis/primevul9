evbuffer_get_callbacks_(struct evbuffer *buffer, struct event_callback **cbs,
    int max_cbs)
{
	int r = 0;
	EVBUFFER_LOCK(buffer);
	if (buffer->deferred_cbs) {
		if (max_cbs < 1) {
			r = -1;
			goto done;
		}
		cbs[0] = &buffer->deferred;
		r = 1;
	}
done:
	EVBUFFER_UNLOCK(buffer);
	return r;
}
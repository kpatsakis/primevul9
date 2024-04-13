evbuffer_defer_callbacks(struct evbuffer *buffer, struct event_base *base)
{
	EVBUFFER_LOCK(buffer);
	buffer->cb_queue = base;
	buffer->deferred_cbs = 1;
	event_deferred_cb_init_(&buffer->deferred,
	    event_base_get_npriorities(base) / 2,
	    evbuffer_deferred_callback, buffer);
	EVBUFFER_UNLOCK(buffer);
	return 0;
}
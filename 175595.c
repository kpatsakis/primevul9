evbuffer_deferred_callback(struct event_callback *cb, void *arg)
{
	struct bufferevent *parent = NULL;
	struct evbuffer *buffer = arg;

	/* XXXX It would be better to run these callbacks without holding the
	 * lock */
	EVBUFFER_LOCK(buffer);
	parent = buffer->parent;
	evbuffer_run_callbacks(buffer, 1);
	evbuffer_decref_and_unlock_(buffer);
	if (parent)
		bufferevent_decref_(parent);
}
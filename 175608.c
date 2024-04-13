evbuffer_invoke_callbacks_(struct evbuffer *buffer)
{
	if (LIST_EMPTY(&buffer->callbacks)) {
		buffer->n_add_for_cb = buffer->n_del_for_cb = 0;
		return;
	}

	if (buffer->deferred_cbs) {
		if (event_deferred_cb_schedule_(buffer->cb_queue, &buffer->deferred)) {
			evbuffer_incref_and_lock_(buffer);
			if (buffer->parent)
				bufferevent_incref_(buffer->parent);
		}
		EVBUFFER_UNLOCK(buffer);
	}

	evbuffer_run_callbacks(buffer, 0);
}
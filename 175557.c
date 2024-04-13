evbuffer_run_callbacks(struct evbuffer *buffer, int running_deferred)
{
	struct evbuffer_cb_entry *cbent, *next;
	struct evbuffer_cb_info info;
	size_t new_size;
	ev_uint32_t mask, masked_val;
	int clear = 1;

	if (running_deferred) {
		mask = EVBUFFER_CB_NODEFER|EVBUFFER_CB_ENABLED;
		masked_val = EVBUFFER_CB_ENABLED;
	} else if (buffer->deferred_cbs) {
		mask = EVBUFFER_CB_NODEFER|EVBUFFER_CB_ENABLED;
		masked_val = EVBUFFER_CB_NODEFER|EVBUFFER_CB_ENABLED;
		/* Don't zero-out n_add/n_del, since the deferred callbacks
		   will want to see them. */
		clear = 0;
	} else {
		mask = EVBUFFER_CB_ENABLED;
		masked_val = EVBUFFER_CB_ENABLED;
	}

	ASSERT_EVBUFFER_LOCKED(buffer);

	if (LIST_EMPTY(&buffer->callbacks)) {
		buffer->n_add_for_cb = buffer->n_del_for_cb = 0;
		return;
	}
	if (buffer->n_add_for_cb == 0 && buffer->n_del_for_cb == 0)
		return;

	new_size = buffer->total_len;
	info.orig_size = new_size + buffer->n_del_for_cb - buffer->n_add_for_cb;
	info.n_added = buffer->n_add_for_cb;
	info.n_deleted = buffer->n_del_for_cb;
	if (clear) {
		buffer->n_add_for_cb = 0;
		buffer->n_del_for_cb = 0;
	}
	for (cbent = LIST_FIRST(&buffer->callbacks);
	     cbent != LIST_END(&buffer->callbacks);
	     cbent = next) {
		/* Get the 'next' pointer now in case this callback decides
		 * to remove itself or something. */
		next = LIST_NEXT(cbent, next);

		if ((cbent->flags & mask) != masked_val)
			continue;

		if ((cbent->flags & EVBUFFER_CB_OBSOLETE))
			cbent->cb.cb_obsolete(buffer,
			    info.orig_size, new_size, cbent->cbarg);
		else
			cbent->cb.cb_func(buffer, &info, cbent->cbarg);
	}
}
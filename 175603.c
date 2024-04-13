evbuffer_remove_cb(struct evbuffer *buffer, evbuffer_cb_func cb, void *cbarg)
{
	struct evbuffer_cb_entry *cbent;
	int result = -1;
	EVBUFFER_LOCK(buffer);
	LIST_FOREACH(cbent, &buffer->callbacks, next) {
		if (cb == cbent->cb.cb_func && cbarg == cbent->cbarg) {
			result = evbuffer_remove_cb_entry(buffer, cbent);
			goto done;
		}
	}
done:
	EVBUFFER_UNLOCK(buffer);
	return result;
}
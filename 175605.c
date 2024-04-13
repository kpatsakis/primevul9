evbuffer_decref_and_unlock_(struct evbuffer *buffer)
{
	struct evbuffer_chain *chain, *next;
	ASSERT_EVBUFFER_LOCKED(buffer);

	EVUTIL_ASSERT(buffer->refcnt > 0);

	if (--buffer->refcnt > 0) {
		EVBUFFER_UNLOCK(buffer);
		return;
	}

	for (chain = buffer->first; chain != NULL; chain = next) {
		next = chain->next;
		evbuffer_chain_free(chain);
	}
	evbuffer_remove_all_callbacks(buffer);
	if (buffer->deferred_cbs)
		event_deferred_cb_cancel_(buffer->cb_queue, &buffer->deferred);

	EVBUFFER_UNLOCK(buffer);
	if (buffer->own_lock)
		EVTHREAD_FREE_LOCK(buffer->lock, EVTHREAD_LOCKTYPE_RECURSIVE);
	mm_free(buffer);
}
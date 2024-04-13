evbuffer_remove_cb_entry(struct evbuffer *buffer,
			 struct evbuffer_cb_entry *ent)
{
	EVBUFFER_LOCK(buffer);
	LIST_REMOVE(ent, next);
	EVBUFFER_UNLOCK(buffer);
	mm_free(ent);
	return 0;
}
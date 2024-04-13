evbuffer_remove_all_callbacks(struct evbuffer *buffer)
{
	struct evbuffer_cb_entry *cbent;

	while ((cbent = LIST_FIRST(&buffer->callbacks))) {
		LIST_REMOVE(cbent, next);
		mm_free(cbent);
	}
}
evbuffer_free(struct evbuffer *buffer)
{
	EVBUFFER_LOCK(buffer);
	evbuffer_decref_and_unlock_(buffer);
}
evbuffer_incref_and_lock_(struct evbuffer *buf)
{
	EVBUFFER_LOCK(buf);
	++buf->refcnt;
}
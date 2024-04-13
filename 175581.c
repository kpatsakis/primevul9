evbuffer_incref_(struct evbuffer *buf)
{
	EVBUFFER_LOCK(buf);
	++buf->refcnt;
	EVBUFFER_UNLOCK(buf);
}
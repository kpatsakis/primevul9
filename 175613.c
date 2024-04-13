evbuffer_set_parent_(struct evbuffer *buf, struct bufferevent *bev)
{
	EVBUFFER_LOCK(buf);
	buf->parent = bev;
	EVBUFFER_UNLOCK(buf);
}
evbuffer_copyout(struct evbuffer *buf, void *data_out, size_t datlen)
{
	return evbuffer_copyout_from(buf, NULL, data_out, datlen);
}
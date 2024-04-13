evbuffer_getchr(struct evbuffer_ptr *it)
{
	struct evbuffer_chain *chain = it->internal_.chain;
	size_t off = it->internal_.pos_in_chain;

	if (chain == NULL)
		return -1;

	return (unsigned char)chain->buffer[chain->misalign + off];
}
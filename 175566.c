evbuffer_reserve_space(struct evbuffer *buf, ev_ssize_t size,
    struct evbuffer_iovec *vec, int n_vecs)
{
	struct evbuffer_chain *chain, **chainp;
	int n = -1;

	EVBUFFER_LOCK(buf);
	if (buf->freeze_end)
		goto done;
	if (n_vecs < 1)
		goto done;
	if (n_vecs == 1) {
		if ((chain = evbuffer_expand_singlechain(buf, size)) == NULL)
			goto done;

		vec[0].iov_base = CHAIN_SPACE_PTR(chain);
		vec[0].iov_len = (size_t) CHAIN_SPACE_LEN(chain);
		EVUTIL_ASSERT(size<0 || (size_t)vec[0].iov_len >= (size_t)size);
		n = 1;
	} else {
		if (evbuffer_expand_fast_(buf, size, n_vecs)<0)
			goto done;
		n = evbuffer_read_setup_vecs_(buf, size, vec, n_vecs,
				&chainp, 0);
	}

done:
	EVBUFFER_UNLOCK(buf);
	return n;

}
evbuffer_commit_space(struct evbuffer *buf,
    struct evbuffer_iovec *vec, int n_vecs)
{
	struct evbuffer_chain *chain, **firstchainp, **chainp;
	int result = -1;
	size_t added = 0;
	int i;

	EVBUFFER_LOCK(buf);

	if (buf->freeze_end)
		goto done;
	if (n_vecs == 0) {
		result = 0;
		goto done;
	} else if (n_vecs == 1 &&
	    (buf->last && vec[0].iov_base == (void*)CHAIN_SPACE_PTR(buf->last))) {
		/* The user only got or used one chain; it might not
		 * be the first one with space in it. */
		if ((size_t)vec[0].iov_len > (size_t)CHAIN_SPACE_LEN(buf->last))
			goto done;
		buf->last->off += vec[0].iov_len;
		added = vec[0].iov_len;
		if (added)
			advance_last_with_data(buf);
		goto okay;
	}

	/* Advance 'firstchain' to the first chain with space in it. */
	firstchainp = buf->last_with_datap;
	if (!*firstchainp)
		goto done;
	if (CHAIN_SPACE_LEN(*firstchainp) == 0) {
		firstchainp = &(*firstchainp)->next;
	}

	chain = *firstchainp;
	/* pass 1: make sure that the pointers and lengths of vecs[] are in
	 * bounds before we try to commit anything. */
	for (i=0; i<n_vecs; ++i) {
		if (!chain)
			goto done;
		if (vec[i].iov_base != (void*)CHAIN_SPACE_PTR(chain) ||
		    (size_t)vec[i].iov_len > CHAIN_SPACE_LEN(chain))
			goto done;
		chain = chain->next;
	}
	/* pass 2: actually adjust all the chains. */
	chainp = firstchainp;
	for (i=0; i<n_vecs; ++i) {
		(*chainp)->off += vec[i].iov_len;
		added += vec[i].iov_len;
		if (vec[i].iov_len) {
			buf->last_with_datap = chainp;
		}
		chainp = &(*chainp)->next;
	}

okay:
	buf->total_len += added;
	buf->n_add_for_cb += added;
	result = 0;
	evbuffer_invoke_callbacks_(buf);

done:
	EVBUFFER_UNLOCK(buf);
	return result;
}
evbuffer_peek(struct evbuffer *buffer, ev_ssize_t len,
    struct evbuffer_ptr *start_at,
    struct evbuffer_iovec *vec, int n_vec)
{
	struct evbuffer_chain *chain;
	int idx = 0;
	ev_ssize_t len_so_far = 0;

	/* Avoid locking in trivial edge cases */
	if (start_at && start_at->internal_.chain == NULL)
		return 0;

	EVBUFFER_LOCK(buffer);

	if (start_at) {
		chain = start_at->internal_.chain;
		len_so_far = chain->off
		    - start_at->internal_.pos_in_chain;
		idx = 1;
		if (n_vec > 0) {
			vec[0].iov_base = chain->buffer + chain->misalign
			    + start_at->internal_.pos_in_chain;
			vec[0].iov_len = len_so_far;
		}
		chain = chain->next;
	} else {
		chain = buffer->first;
	}

	if (n_vec == 0 && len < 0) {
		/* If no vectors are provided and they asked for "everything",
		 * pretend they asked for the actual available amount. */
		len = buffer->total_len;
		if (start_at) {
			len -= start_at->pos;
		}
	}

	while (chain) {
		if (len >= 0 && len_so_far >= len)
			break;
		if (idx<n_vec) {
			vec[idx].iov_base = chain->buffer + chain->misalign;
			vec[idx].iov_len = chain->off;
		} else if (len<0) {
			break;
		}
		++idx;
		len_so_far += chain->off;
		chain = chain->next;
	}

	EVBUFFER_UNLOCK(buffer);

	return idx;
}
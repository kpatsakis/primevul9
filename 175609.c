evbuffer_add_buffer(struct evbuffer *outbuf, struct evbuffer *inbuf)
{
	struct evbuffer_chain *pinned, *last;
	size_t in_total_len, out_total_len;
	int result = 0;

	EVBUFFER_LOCK2(inbuf, outbuf);
	in_total_len = inbuf->total_len;
	out_total_len = outbuf->total_len;

	if (in_total_len == 0 || outbuf == inbuf)
		goto done;

	if (outbuf->freeze_end || inbuf->freeze_start) {
		result = -1;
		goto done;
	}

	if (PRESERVE_PINNED(inbuf, &pinned, &last) < 0) {
		result = -1;
		goto done;
	}

	if (out_total_len == 0) {
		/* There might be an empty chain at the start of outbuf; free
		 * it. */
		evbuffer_free_all_chains(outbuf->first);
		COPY_CHAIN(outbuf, inbuf);
	} else {
		APPEND_CHAIN(outbuf, inbuf);
	}

	RESTORE_PINNED(inbuf, pinned, last);

	inbuf->n_del_for_cb += in_total_len;
	outbuf->n_add_for_cb += in_total_len;

	evbuffer_invoke_callbacks_(inbuf);
	evbuffer_invoke_callbacks_(outbuf);

done:
	EVBUFFER_UNLOCK2(inbuf, outbuf);
	return result;
}
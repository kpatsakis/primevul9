APPEND_CHAIN_MULTICAST(struct evbuffer *dst, struct evbuffer *src)
{
	struct evbuffer_chain *tmp;
	struct evbuffer_chain *chain = src->first;
	struct evbuffer_multicast_parent *extra;

	ASSERT_EVBUFFER_LOCKED(dst);
	ASSERT_EVBUFFER_LOCKED(src);

	for (; chain; chain = chain->next) {
		if (!chain->off || chain->flags & EVBUFFER_DANGLING) {
			/* skip empty chains */
			continue;
		}

		tmp = evbuffer_chain_new(sizeof(struct evbuffer_multicast_parent));
		if (!tmp) {
			event_warn("%s: out of memory", __func__);
			return;
		}
		extra = EVBUFFER_CHAIN_EXTRA(struct evbuffer_multicast_parent, tmp);
		/* reference evbuffer containing source chain so it
		 * doesn't get released while the chain is still
		 * being referenced to */
		evbuffer_incref_(src);
		extra->source = src;
		/* reference source chain which now becomes immutable */
		evbuffer_chain_incref(chain);
		extra->parent = chain;
		chain->flags |= EVBUFFER_IMMUTABLE;
		tmp->buffer_len = chain->buffer_len;
		tmp->misalign = chain->misalign;
		tmp->off = chain->off;
		tmp->flags |= EVBUFFER_MULTICAST|EVBUFFER_IMMUTABLE;
		tmp->buffer = chain->buffer;
		evbuffer_chain_insert(dst, tmp);
	}
}
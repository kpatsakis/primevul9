evbuffer_chain_free(struct evbuffer_chain *chain)
{
	EVUTIL_ASSERT(chain->refcnt > 0);
	if (--chain->refcnt > 0) {
		/* chain is still referenced by other chains */
		return;
	}

	if (CHAIN_PINNED(chain)) {
		/* will get freed once no longer dangling */
		chain->refcnt++;
		chain->flags |= EVBUFFER_DANGLING;
		return;
	}

	/* safe to release chain, it's either a referencing
	 * chain or all references to it have been freed */
	if (chain->flags & EVBUFFER_REFERENCE) {
		struct evbuffer_chain_reference *info =
		    EVBUFFER_CHAIN_EXTRA(
			    struct evbuffer_chain_reference,
			    chain);
		if (info->cleanupfn)
			(*info->cleanupfn)(chain->buffer,
			    chain->buffer_len,
			    info->extra);
	}
	if (chain->flags & EVBUFFER_FILESEGMENT) {
		struct evbuffer_chain_file_segment *info =
		    EVBUFFER_CHAIN_EXTRA(
			    struct evbuffer_chain_file_segment,
			    chain);
		if (info->segment) {
#ifdef _WIN32
			if (info->segment->is_mapping)
				UnmapViewOfFile(chain->buffer);
#endif
			evbuffer_file_segment_free(info->segment);
		}
	}
	if (chain->flags & EVBUFFER_MULTICAST) {
		struct evbuffer_multicast_parent *info =
		    EVBUFFER_CHAIN_EXTRA(
			    struct evbuffer_multicast_parent,
			    chain);
		/* referencing chain is being freed, decrease
		 * refcounts of source chain and associated
		 * evbuffer (which get freed once both reach
		 * zero) */
		EVUTIL_ASSERT(info->source != NULL);
		EVUTIL_ASSERT(info->parent != NULL);
		EVBUFFER_LOCK(info->source);
		evbuffer_chain_free(info->parent);
		evbuffer_decref_and_unlock_(info->source);
	}

	mm_free(chain);
}
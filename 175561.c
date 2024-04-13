evbuffer_chain_unpin_(struct evbuffer_chain *chain, unsigned flag)
{
	EVUTIL_ASSERT((chain->flags & flag) != 0);
	chain->flags &= ~flag;
	if (chain->flags & EVBUFFER_DANGLING)
		evbuffer_chain_free(chain);
}
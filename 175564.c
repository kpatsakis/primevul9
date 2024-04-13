evbuffer_chain_pin_(struct evbuffer_chain *chain, unsigned flag)
{
	EVUTIL_ASSERT((chain->flags & flag) == 0);
	chain->flags |= flag;
}
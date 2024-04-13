evbuffer_chain_incref(struct evbuffer_chain *chain)
{
    ++chain->refcnt;
}
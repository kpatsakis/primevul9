dnsc_nonces_delkeyfunc(void *k, void* ATTR_UNUSED(arg))
{
    struct nonce_cache_key* nk = (struct nonce_cache_key*)k;
    lock_rw_destroy(&nk->entry.lock);
    free(nk);
}
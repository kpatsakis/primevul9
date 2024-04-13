dnsc_nonces_sizefunc(void *k, void* ATTR_UNUSED(d))
{
    struct nonce_cache_key* nk = (struct nonce_cache_key*)k;
    size_t key_size = sizeof(struct nonce_cache_key)
        + lock_get_mem(&nk->entry.lock);
    (void)nk; /* otherwise ssk is unused if no threading, or fixed locksize */
    return key_size;
}
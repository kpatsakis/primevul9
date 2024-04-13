dnsc_shared_secrets_sizefunc(void *k, void* ATTR_UNUSED(d))
{
    struct shared_secret_cache_key* ssk = (struct shared_secret_cache_key*)k;
    size_t key_size = sizeof(struct shared_secret_cache_key)
        + lock_get_mem(&ssk->entry.lock);
    size_t data_size = crypto_box_BEFORENMBYTES;
    (void)ssk; /* otherwise ssk is unused if no threading, or fixed locksize */
    return key_size + data_size;
}
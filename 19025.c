dnsc_shared_secrets_delkeyfunc(void *k, void* ATTR_UNUSED(arg))
{
    struct shared_secret_cache_key* ssk = (struct shared_secret_cache_key*)k;
    lock_rw_destroy(&ssk->entry.lock);
    free(ssk);
}
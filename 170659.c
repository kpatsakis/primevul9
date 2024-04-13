MenuCache* menu_cache_ref(MenuCache* cache)
{
    g_atomic_int_inc( &cache->n_ref );
    return cache;
}
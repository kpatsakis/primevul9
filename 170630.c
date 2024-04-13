MenuCache* menu_cache_lookup( const char* menu_name )
{
    MenuCache* cache;

    /* lookup in a hash table for already loaded menus */
    MENU_CACHE_LOCK;
#if !GLIB_CHECK_VERSION(2, 32, 0)
    /* FIXME: destroy them on application exit? */
    if(!sync_run_mutex)
        sync_run_mutex = g_mutex_new();
    if(!sync_run_cond)
        sync_run_cond = g_cond_new();
#endif
    if( G_UNLIKELY( ! hash ) )
        hash = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL );
    else
    {
        cache = (MenuCache*)g_hash_table_lookup(hash, menu_name);
        if( cache )
        {
            menu_cache_ref(cache);
            MENU_CACHE_UNLOCK;
            return cache;
        }
    }
    MENU_CACHE_UNLOCK;

    cache = menu_cache_create(menu_name);
    cache->cancellable = g_cancellable_new();
#if GLIB_CHECK_VERSION(2, 32, 0)
    cache->thr = g_thread_new(menu_name, menu_cache_loader_thread, cache);
#else
    cache->thr = g_thread_create(menu_cache_loader_thread, cache, TRUE, NULL);
#endif
    return cache;
}
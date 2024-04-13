static void cache_free(Cache* cache)
{
    /* shutdown cache in 10 minutes of inactivity */
    if(!cache->delayed_free_handler)
        cache->delayed_free_handler = g_timeout_add_seconds(600,
                                                            delayed_cache_free,
                                                            cache);
    DEBUG("menu %p cache unused, removing in 600s", cache);
}
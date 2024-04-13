static MenuCache* menu_cache_new( const char* cache_file )
{
    MenuCache* cache;
    cache = g_slice_new0( MenuCache );
    cache->cache_file = g_strdup( cache_file );
    cache->n_ref = 1;
    return cache;
}
char* menu_cache_dir_make_path( MenuCacheDir* dir )
{
    GString* path = g_string_sized_new(1024);
    MenuCacheItem* it;

    MENU_CACHE_LOCK;
    while( (it = MENU_CACHE_ITEM(dir)) ) /* this is not top dir */
    {
        g_string_prepend( path, menu_cache_item_get_id(it) );
        g_string_prepend_c( path, '/' );
        /* FIXME: if parent is already unref'd by another thread then
           path being made will be broken. Is there any way to avoid that? */
        dir = it->parent;
    }
    MENU_CACHE_UNLOCK;
    return g_string_free( path, FALSE );
}
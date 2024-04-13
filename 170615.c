MenuCacheDir* menu_cache_get_dir_from_path( MenuCache* cache, const char* path )
{
    char** names = g_strsplit( path + 1, "/", -1 );
    int i = 0;
    MenuCacheDir* dir = NULL;

    if( !names )
        return NULL;

    if( G_UNLIKELY(!names[0]) )
    {
        g_strfreev(names);
        return NULL;
    }
    /* the topmost dir of the path should be the root menu dir. */
    MENU_CACHE_LOCK;
    dir = cache->root_dir;
    if (G_UNLIKELY(dir == NULL) || strcmp(names[0], MENU_CACHE_ITEM(dir)->id))
    {
        MENU_CACHE_UNLOCK;
        return NULL;
    }

    for( ++i; names[i]; ++i )
    {
        GSList* l;
        for( l = dir->children; l; l = l->next )
        {
            MenuCacheItem* item = MENU_CACHE_ITEM(l->data);
            if( item->type == MENU_CACHE_TYPE_DIR && 0 == strcmp( item->id, names[i] ) )
                dir = MENU_CACHE_DIR(item);
        }
        /* FIXME: we really should ref it on return since other thread may
           destroy the parent at this time and returned data become invalid.
           Therefore this call isn't thread-safe! */
        if( ! dir )
        {
            MENU_CACHE_UNLOCK;
            return NULL;
        }
    }
    MENU_CACHE_UNLOCK;
    return dir;
}
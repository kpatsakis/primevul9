MenuCacheItem* menu_cache_item_from_path( MenuCache* cache, const char* path )
{
    char** names = g_strsplit( path + 1, "/", -1 );
    int i;
    MenuCacheDir* dir;
    MenuCacheItem* item = NULL;

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
    if( G_UNLIKELY(!dir) || strcmp(names[0], MENU_CACHE_ITEM(dir)->id) != 0 )
        goto _end;

    for( i = 1; names[i]; ++i )
    {
        GSList* l;
        item = NULL;
        if( !dir )
            break;
        l = dir->children;
        dir = NULL;
        for( ; l; l = l->next )
        {
            item = MENU_CACHE_ITEM(l->data);
            if( g_strcmp0( item->id, names[i] ) == 0 )
            {
                if( item->type == MENU_CACHE_TYPE_DIR )
                    dir = MENU_CACHE_DIR(item);
                break;
            }
            item = NULL;
        }
        if( !item )
            break;
    }
    if(item)
        menu_cache_item_ref(item);
_end:
    MENU_CACHE_UNLOCK;
    g_strfreev(names);
    return item;
}
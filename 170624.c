static gboolean reload_notify(gpointer data)
{
    MenuCache* cache = (MenuCache*)data;
    GSList* l;
    MENU_CACHE_LOCK;
    /* we have it referenced and there is no source removal so no check */
    for( l = cache->notifiers; l; l = l->next )
    {
        CacheReloadNotifier* n = (CacheReloadNotifier*)l->data;
        if(n->func)
            n->func( cache, n->user_data );
    }
    MENU_CACHE_UNLOCK;
    return FALSE;
}
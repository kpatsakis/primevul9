void menu_cache_remove_reload_notify(MenuCache* cache, MenuCacheNotifyId notify_id)
{
    MENU_CACHE_LOCK;
    g_slice_free( CacheReloadNotifier, ((GSList*)notify_id)->data );
    cache->notifiers = g_slist_delete_link( cache->notifiers, (GSList*)notify_id );
    MENU_CACHE_UNLOCK;
}
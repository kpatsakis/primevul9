MenuCache* menu_cache_lookup_sync( const char* menu_name )
{
    MenuCache* mc = menu_cache_lookup(menu_name);
    MenuCacheDir* root_dir = menu_cache_dup_root_dir(mc);
    /* ensure that the menu cache is loaded */
    if(root_dir)
        menu_cache_item_unref(MENU_CACHE_ITEM(root_dir));
    else /* if it's not yet loaded */
    {
        MenuCacheNotifyId notify_id;
        /* add stub */
        notify_id = menu_cache_add_reload_notify(mc, NULL, NULL);
#if GLIB_CHECK_VERSION(2, 32, 0)
        g_mutex_lock(&sync_run_mutex);
        while(!mc->ready)
            g_cond_wait(&sync_run_cond, &sync_run_mutex);
        g_mutex_unlock(&sync_run_mutex);
#else
        g_mutex_lock(sync_run_mutex);
        g_debug("menu_cache_lookup_sync: enter wait %p", mc);
        while(!mc->ready)
            g_cond_wait(sync_run_cond, sync_run_mutex);
        g_debug("menu_cache_lookup_sync: leave wait");
        g_mutex_unlock(sync_run_mutex);
#endif
        menu_cache_remove_reload_notify(mc, notify_id);
    }
    return mc;
}
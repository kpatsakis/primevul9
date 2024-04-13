GSList* menu_cache_dir_get_children( MenuCacheDir* dir )
{
    /* NOTE: this is very ugly hack but dir may be freed by cache reload
       in server-io thread, so we should keep it alive :( */
    g_timeout_add_seconds(10, (GSourceFunc)menu_cache_item_unref,
                          menu_cache_item_ref(MENU_CACHE_ITEM(dir)));
    return dir->children;
}
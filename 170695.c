GSList* menu_cache_list_all_apps(MenuCache* cache)
{
    GSList* list;
    MENU_CACHE_LOCK;
    if (G_UNLIKELY(!cache->root_dir)) /* empty cache */
        list = NULL;
    else
        list = list_app_in_dir(cache->root_dir, NULL);
    MENU_CACHE_UNLOCK;
    return list;
}
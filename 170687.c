GSList *menu_cache_list_all_for_category(MenuCache* cache, const char *category)
{
    GQuark q;
    GSList *list;

    g_return_val_if_fail(cache != NULL && category != NULL, NULL);
    q = g_quark_try_string(category);
    if (q == 0)
        return NULL;
    MENU_CACHE_LOCK;
    if (G_UNLIKELY(cache->root_dir == NULL))
        list = NULL;
    else
        list = list_app_in_dir_for_cat(cache->root_dir, NULL, g_quark_to_string(q));
    MENU_CACHE_UNLOCK;
    return list;
}
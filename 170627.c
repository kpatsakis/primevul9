GSList *menu_cache_list_all_for_keyword(MenuCache* cache, const char *keyword)
{
    char *casefolded = g_utf8_casefold(keyword, -1);
    GSList *list;

    g_return_val_if_fail(cache != NULL && keyword != NULL, NULL);
    MENU_CACHE_LOCK;
    if (G_UNLIKELY(cache->root_dir == NULL))
        list = NULL;
    else
        list = list_app_in_dir_for_kw(cache->root_dir, NULL, casefolded);
    MENU_CACHE_UNLOCK;
    g_free(casefolded);
    return list;
}
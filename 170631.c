static GSList* list_app_in_dir_for_kw(MenuCacheDir *dir, GSList *list, const char *kw)
{
    GSList *l;

    for (l = dir->children; l; l = l->next)
    {
        MenuCacheItem *item = MENU_CACHE_ITEM(l->data);
        switch (item->type)
        {
        case MENU_CACHE_TYPE_DIR:
            list = list_app_in_dir_for_kw(MENU_CACHE_DIR(item), list, kw);
            break;
        case MENU_CACHE_TYPE_APP:
            if (strstr(MENU_CACHE_APP(item)->keywords, kw) != NULL)
                list = g_slist_prepend(list, menu_cache_item_ref(item));
            break;
        case MENU_CACHE_TYPE_NONE:
        case MENU_CACHE_TYPE_SEP:
            break;
        }
    }
    return list;
}
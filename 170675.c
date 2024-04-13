static GSList* list_app_in_dir_for_cat(MenuCacheDir *dir, GSList *list, const char *id)
{
    const char **cat;
    GSList *l;

    for (l = dir->children; l; l = l->next)
    {
        MenuCacheItem *item = MENU_CACHE_ITEM(l->data);
        switch (item->type)
        {
        case MENU_CACHE_TYPE_DIR:
            list = list_app_in_dir_for_cat(MENU_CACHE_DIR(item), list, id);
            break;
        case MENU_CACHE_TYPE_APP:
            cat = MENU_CACHE_APP(item)->categories;
            if (cat) while (*cat)
                if (*cat++ == id)
                {
                    list = g_slist_prepend(list, menu_cache_item_ref(item));
                    break;
                }
            break;
        case MENU_CACHE_TYPE_NONE:
        case MENU_CACHE_TYPE_SEP:
            break;
        }
    }
    return list;
}
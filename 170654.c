static MenuCacheItem *_scan_by_id(MenuCacheItem *item, const char *id)
{
    GSList *l;

    if (item)
        switch (menu_cache_item_get_type(item))
        {
            case MENU_CACHE_TYPE_DIR:
                for (l = MENU_CACHE_DIR(item)->children; l; l = l->next)
                {
                    item = _scan_by_id(MENU_CACHE_ITEM(l->data), id);
                    if (item)
                        return item;
                }
                break;
            case MENU_CACHE_TYPE_APP:
                if (g_strcmp0(menu_cache_item_get_id(item), id) == 0)
                    return item;
                break;
            default: ;
        }
    return NULL;
}
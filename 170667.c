MenuCacheItem *menu_cache_find_item_by_id(MenuCache *cache, const char *id)
{
    MenuCacheItem *item = NULL;

    MENU_CACHE_LOCK;
    if (cache && id)
        item = _scan_by_id(MENU_CACHE_ITEM(cache->root_dir), id);
    if (item)
        menu_cache_item_ref(item);
    MENU_CACHE_UNLOCK;
    return item;
}
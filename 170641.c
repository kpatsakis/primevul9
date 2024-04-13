MenuCacheDir* menu_cache_item_dup_parent( MenuCacheItem* item )
{
    MenuCacheDir* dir;
    MENU_CACHE_LOCK;
    dir = item->parent;
    if(G_LIKELY(dir))
        menu_cache_item_ref(MENU_CACHE_ITEM(dir));
    MENU_CACHE_UNLOCK;
    return dir;
}
MenuCacheDir* menu_cache_dup_root_dir( MenuCache* cache )
{
    MenuCacheDir* dir;
    MENU_CACHE_LOCK;
    dir = cache->root_dir;
    if(G_LIKELY(dir))
        menu_cache_item_ref(MENU_CACHE_ITEM(dir));
    MENU_CACHE_UNLOCK;
    return dir;
}
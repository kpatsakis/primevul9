MenuCacheItem *menu_cache_find_child_by_name(MenuCacheDir *dir, const char *name)
{
    GSList *child;
    MenuCacheItem *item = NULL;

    if (MENU_CACHE_ITEM(dir)->type != MENU_CACHE_TYPE_DIR || name == NULL)
        return NULL;
    MENU_CACHE_LOCK;
    for (child = dir->children; child; child = child->next)
        if (g_strcmp0(MENU_CACHE_ITEM(child->data)->name, name) == 0)
        {
            item = menu_cache_item_ref(child->data);
            break;
        }
    MENU_CACHE_UNLOCK;
    return item;
}
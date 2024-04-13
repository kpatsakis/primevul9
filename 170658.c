GSList* menu_cache_dir_list_children(MenuCacheDir* dir)
{
    GSList *children, *l;

    if(MENU_CACHE_ITEM(dir)->type != MENU_CACHE_TYPE_DIR)
        return NULL;
    MENU_CACHE_LOCK;
    children = g_slist_copy(dir->children);
    for(l = children; l; l = l->next)
        menu_cache_item_ref(l->data);
    MENU_CACHE_UNLOCK;
    return children;
}
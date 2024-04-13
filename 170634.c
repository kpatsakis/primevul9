static GSList* list_app_in_dir(MenuCacheDir* dir, GSList* list)
{
    GSList* l;
    for( l = dir->children; l; l = l->next )
    {
        MenuCacheItem* item = MENU_CACHE_ITEM(l->data);
        switch( menu_cache_item_get_type(item) )
        {
        case MENU_CACHE_TYPE_DIR:
            list = list_app_in_dir( MENU_CACHE_DIR(item), list );
            break;
        case MENU_CACHE_TYPE_APP:
            list = g_slist_prepend(list, menu_cache_item_ref(item));
            break;
        case MENU_CACHE_TYPE_NONE:
        case MENU_CACHE_TYPE_SEP:
            break;
        }
    }
    return list;
}
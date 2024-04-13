MenuCacheItem* menu_cache_item_ref(MenuCacheItem* item)
{
    g_atomic_int_inc( &item->n_ref );
    /* DEBUG("item_ref %s: %d -> %d", item->id, item->n_ref-1, item->n_ref); */
    return item;
}
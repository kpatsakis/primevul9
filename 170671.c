gboolean menu_cache_item_unref(MenuCacheItem* item)
{
    /* DEBUG("item_unref(%s): %d", item->id, item->n_ref); */
    /* We need a lock here unfortunately since another thread may have access
       to it via some child->parent which isn't protected by reference */
    MENU_CACHE_LOCK; /* lock may be recursive here */
    if( g_atomic_int_dec_and_test( &item->n_ref ) )
    {
        /* DEBUG("free item: %s", item->id); */
        g_free( item->id );
        g_free( item->name );
        g_free( item->comment );
        g_free( item->icon );

        menu_cache_file_dir_unref(item->file_dir);

        if( item->file_name && item->file_name != item->id )
            g_free( item->file_name );

        if( item->parent )
        {
            /* DEBUG("remove %s from parent %s", item->id, MENU_CACHE_ITEM(item->parent)->id); */
            /* remove ourselve from the parent node. */
            item->parent->children = g_slist_remove(item->parent->children, item);
        }

        if( item->type == MENU_CACHE_TYPE_DIR )
        {
            MenuCacheDir* dir = MENU_CACHE_DIR(item);
            GSList* l;
            for(l = dir->children; l; )
            {
                MenuCacheItem* child = MENU_CACHE_ITEM(l->data);
                /* remove ourselve from the children. */
                child->parent = NULL;
                l = l->next;
                menu_cache_item_unref(child);
            }
            g_slist_free( dir->children );
            g_slice_free( MenuCacheDir, dir );
        }
        else
        {
            MenuCacheApp* app = MENU_CACHE_APP(item);
            g_free( app->exec );
            g_free(app->try_exec);
            g_free(app->working_dir);
            g_free(app->categories);
            g_free(app->keywords);
            g_slice_free( MenuCacheApp, app );
        }
    }
    MENU_CACHE_UNLOCK;
    return FALSE;
}
void menu_cache_unref(MenuCache* cache)
{
    /* DEBUG("cache_unref: %d", cache->n_ref); */
    /* we need a lock here unfortunately because item in hash isn't protected
       by reference therefore another thread may get access to it right now */
    MENU_CACHE_LOCK;
    if( g_atomic_int_dec_and_test(&cache->n_ref) )
    {
        /* g_assert(cache->reload_id != 0); */
        unregister_menu_from_server( cache );
        /* DEBUG("unregister to server"); */
        g_hash_table_remove( hash, cache->menu_name );
        if( g_hash_table_size(hash) == 0 )
        {
            /* DEBUG("destroy hash"); */
            g_hash_table_destroy(hash);

            /* DEBUG("disconnect from server"); */
            G_LOCK(connect);
            shutdown(server_fd, SHUT_RDWR); /* the IO thread will terminate itself */
            server_fd = -1;
            G_UNLOCK(connect);
            hash = NULL;
        }
        MENU_CACHE_UNLOCK;

        if(G_LIKELY(cache->thr))
        {
            g_cancellable_cancel(cache->cancellable);
            g_thread_join(cache->thr);
        }
        g_object_unref(cache->cancellable);
        if( G_LIKELY(cache->root_dir) )
        {
            /* DEBUG("unref root dir"); */
            menu_cache_item_unref( MENU_CACHE_ITEM(cache->root_dir) );
            /* DEBUG("unref root dir finished"); */
        }
        g_free( cache->cache_file );
        g_free( cache->menu_name );
        g_free(cache->reg);
        /* g_free( cache->menu_file_path ); */
        g_strfreev(cache->known_des);
        g_slist_free(cache->notifiers);
        g_slice_free( MenuCache, cache );
    }
    else
        MENU_CACHE_UNLOCK;
}
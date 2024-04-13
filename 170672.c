static gboolean delayed_cache_free(gpointer data)
{
    Cache* cache = data;
    int i;

    if(g_source_is_destroyed(g_main_current_source()))
        return FALSE;

    g_hash_table_remove( hash, cache->md5 );
    /* DEBUG("menu cache freed"); */
    for(i = 0; i < cache->n_files; ++i)
    {
        g_file_monitor_cancel( cache->mons[i] );
        g_object_unref( cache->mons[i] );
    }
/*
    g_file_monitor_cancel(cache->cache_mon);
    g_object_unref(cache->cache_mon);
*/
    g_free( cache->mons );
    g_free(cache->menu_name);
    g_free(cache->lang_name);
    g_free(cache->cache_file);
    g_strfreev( cache->env );
    g_strfreev( cache->files );

    if( cache->delayed_reload_handler )
        g_source_remove( cache->delayed_reload_handler );

    g_slice_free( Cache, cache );

    if(g_hash_table_size(hash) == 0)
        g_main_loop_quit(main_loop);
    return FALSE;
}
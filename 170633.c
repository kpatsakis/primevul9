static void do_reload(Cache* cache)
{
    GSList* l;
    char buf[38];
    int i;
    GFile* gf;

    int new_n_files;
    char **new_files = NULL;

    /* DEBUG("Re-generation of cache is needed!"); */
    /* DEBUG("call menu-cache-gen to re-generate the cache"); */
    memcpy( buf, "REL:", 4 );
    memcpy( buf + 4, cache->md5, 32 );
    buf[36] = '\n';
    buf[37] = '\0';

    if( ! regenerate_cache( cache->menu_name, cache->lang_name, cache->cache_file,
                            cache->env, &new_n_files, &new_files ) )
    {
        DEBUG("regeneration of cache failed.");
        return;
    }

    /* cancel old file monitors */
    g_strfreev(cache->files);
    for( i = 0; i < cache->n_files; ++i )
    {
        g_file_monitor_cancel( cache->mons[i] );
        g_signal_handlers_disconnect_by_func( cache->mons[i], on_file_changed, cache );
        g_object_unref( cache->mons[i] );
    }
/*
    g_file_monitor_cancel(cache->cache_mon);
    g_object_unref(cache->cache_mon);
*/

    cache->n_files = new_n_files;
    cache->files = new_files;

    cache->mons = g_realloc( cache->mons, sizeof(GFileMonitor*)*(cache->n_files+1) );
    /* create required file monitors */
    for( i = 0; i < cache->n_files; ++i )
    {
        gf = g_file_new_for_path( cache->files[i] + 1 );
        if( cache->files[i][0] == 'D' )
            cache->mons[i] = g_file_monitor_directory( gf, 0, NULL, NULL );
        else
            cache->mons[i] = g_file_monitor_file( gf, 0, NULL, NULL );
        g_signal_connect(cache->mons[i], "changed",
                         G_CALLBACK(on_file_changed), cache);
        g_object_unref(gf);
    }
/*
    gf = g_file_new_for_path( cache_file );
    cache->cache_mon = g_file_monitor_file( gf, 0, NULL, NULL );
    g_signal_connect( cache->cache_mon, "changed", on_file_changed, cache);
    g_object_unref(gf);
*/

    /* notify the clients that reload is needed. */
    for( l = cache->clients; l; )
    {
        ClientIO *channel_io = (ClientIO *)l->data;
        GIOChannel* ch = channel_io->channel;
        l = l->next; /* do it beforehand, as client may be removed below */
        if(write(g_io_channel_unix_get_fd(ch), buf, 37) < 37)
        {
            on_client_closed(channel_io);
        }
    }
    cache->need_reload = FALSE;
}
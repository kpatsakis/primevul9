void on_file_changed( GFileMonitor* mon, GFile* gf, GFile* other,
                      GFileMonitorEvent evt, Cache* cache )
{
#ifdef G_ENABLE_DEBUG
    char *path = g_file_get_path(gf);
    g_debug("file %s is changed (%d).", path, evt);
    g_free(path);
#endif
    /* if( mon != cache->cache_mon ) */
    {
        /* Optimization: Some files in the dir are changed, but it
         * won't affect the content of the menu. So, just omit them,
         * and update the mtime of the cached file with utime.
         */
        int idx;
        /* dirty hack: get index of the monitor in array */
        for(idx = 0; idx < cache->n_files; ++idx)
        {
            if(mon == cache->mons[idx])
                break;
        }
        /* if the monitored file is a directory */
        if( G_LIKELY(idx < cache->n_files) && cache->files[idx][0] == 'D' )
        {
            char* changed_file = g_file_get_path(gf);
            char* dir_path = cache->files[idx]+1;
            int len = strlen(dir_path);
            /* if the changed file is a file in the monitored dir */
            if( strncmp(changed_file, dir_path, len) == 0 && changed_file[len] == '/' )
            {
                char* base_name = changed_file + len + 1;
                gboolean skip = TRUE;
                /* only *.desktop and *.directory files can affect the content of the menu. */
                if( g_str_has_suffix(base_name, ".desktop") )
                {
                        skip = FALSE;
                }
                else if( g_str_has_suffix(base_name, ".directory") )
                    skip = FALSE;

                if( skip )
                {
                    DEBUG("files are changed, but no re-generation is needed.");
                    return;
                }
            }
            g_free(changed_file);
        }
    }

    if( cache->delayed_reload_handler )
    {
        /* we got some change in last 3 seconds... not reload again */
        cache->need_reload = TRUE;
        g_source_remove(cache->delayed_reload_handler);
    }
    else
    {
        /* no reload in last 3 seconds... good, do it immediately */
        /* mark need_reload anyway. If do_reload succeeds, it is cleaned up */
        cache->need_reload = TRUE;
        do_reload(cache);
    }

    cache->delayed_reload_handler = g_timeout_add_seconds_full( G_PRIORITY_LOW, 3, (GSourceFunc)delayed_reload, cache, NULL );
}
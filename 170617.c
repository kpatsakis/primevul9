static gboolean on_client_data_in(GIOChannel* ch, GIOCondition cond, gpointer user_data)
{
    char *line;
    gsize len;
    GIOStatus st;
    const char* md5;
    Cache* cache;
    GFile* gf;
    gboolean ret = TRUE;

    if(cond & (G_IO_HUP|G_IO_ERR) )
    {
        on_client_closed(user_data);
        return FALSE; /* remove the watch */
    }

retry:
    st = g_io_channel_read_line( ch, &line, &len, NULL, NULL );
    if( st == G_IO_STATUS_AGAIN )
        goto retry;
    if( st != G_IO_STATUS_NORMAL ) {
        on_client_closed(user_data);
        return FALSE;
    }

    --len;
    line[len] = 0;

    DEBUG("line(%d) = %s", (int)len, line);

    if( memcmp(line, "REG:", 4) == 0 )
    {
        int n_files = 0, i;
        char *pline = line + 4;
        char *sep, *menu_name, *lang_name, *cache_dir;
        char **files = NULL;
        char **env;
        char reload_cmd[38] = "REL:";

        len -= 4;
        /* Format of received string, separated by '\t'.
         * Menu Name
         * Language Name
         * XDG_CACHE_HOME
         * XDG_CONFIG_DIRS
         * XDG_MENU_PREFIX
         * XDG_DATA_DIRS
         * XDG_CONFIG_HOME
         * XDG_DATA_HOME
         * (optional) CACHE_GEN_VERSION
         * md5 hash */

        md5 = pline + len - 32; /* the md5 hash of this menu */

        cache = (Cache*)g_hash_table_lookup(hash, md5);
        if( !cache )
        {
            sep = strchr(pline, '\t');
            *sep = '\0';
            menu_name = pline;
            pline = sep + 1;

            sep = strchr(pline, '\t');
            *sep = '\0';
            lang_name = pline;
            pline = sep + 1;

            ((char *)md5)[-1] = '\0';
            env = g_strsplit(pline, "\t", 0);

            cache_dir = env[0]; /* XDG_CACHE_HOME */
            /* obtain cache dir from client's env */

            cache = g_slice_new0( Cache );
            cache->cache_file = g_build_filename(*cache_dir ? cache_dir : g_get_user_cache_dir(), "menus", md5, NULL );
            if( ! cache_file_is_updated(cache->cache_file, &n_files, &files) )
            {
                /* run menu-cache-gen */
                if(! regenerate_cache( menu_name, lang_name, cache->cache_file, env, &n_files, &files ) )
                {
                    DEBUG("regeneration of cache failed!!");
                }
            }
            else
            {
                /* file loaded, schedule update anyway */
                cache->need_reload = TRUE;
                cache->delayed_reload_handler = g_timeout_add_seconds_full(G_PRIORITY_LOW, 3,
                                                    (GSourceFunc)delayed_reload, cache, NULL);
            }
            memcpy( cache->md5, md5, 33 );
            cache->n_files = n_files;
            cache->files = files;
            cache->menu_name = g_strdup(menu_name);
            cache->lang_name = g_strdup(lang_name);
            cache->env = env;
            cache->mons = g_new0(GFileMonitor*, n_files+1);
            /* create required file monitors */
            DEBUG("%d files/dirs are monitored.", n_files);
            for( i = 0; i < n_files; ++i )
            {
                gf = g_file_new_for_path( files[i] + 1 );
                if( files[i][0] == 'D' )
                    cache->mons[i] = g_file_monitor_directory( gf, 0, NULL, NULL );
                else
                    cache->mons[i] = g_file_monitor_file( gf, 0, NULL, NULL );
                DEBUG("monitor: %s", g_file_get_path(gf));
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
            g_hash_table_insert(hash, cache->md5, cache);
            DEBUG("new menu cache %p added to hash", cache);
        }
        else if (access(cache->cache_file, R_OK) != 0)
        {
            /* bug SF#657: if user deleted cache file we have to regenerate it */
            if (!regenerate_cache(cache->menu_name, cache->lang_name, cache->cache_file,
                                  cache->env, &cache->n_files, &cache->files))
            {
                DEBUG("regeneration of cache failed.");
            }
        }
        /* DEBUG("menu %s requested by client %d", md5, g_io_channel_unix_get_fd(ch)); */
        cache->clients = g_slist_prepend(cache->clients, user_data);
        if(cache->delayed_free_handler)
        {
            g_source_remove(cache->delayed_free_handler);
            cache->delayed_free_handler = 0;
        }
        DEBUG("client %p added to cache %p", ch, cache);

        /* generate a fake reload notification */
        DEBUG("fake reload!");
        memcpy(reload_cmd + 4, md5, 32);

        reload_cmd[36] = '\n';
        reload_cmd[37] = '\0';

        DEBUG("reload command: %s", reload_cmd);
        ret = write(g_io_channel_unix_get_fd(ch), reload_cmd, 37) > 0;
    }
    else if( memcmp(line, "UNR:", 4) == 0 )
    {
        md5 = line + 4;
        DEBUG("unregister: %s", md5);
        cache = (Cache*)g_hash_table_lookup(hash, md5);
        if(cache && cache->clients)
        {
            /* remove the IO channel from the cache */
            cache->clients = g_slist_remove(cache->clients, user_data);
            if(cache->clients == NULL)
                cache_free(cache);
        }
        else
            DEBUG("bug! client is not found.");
    }
    g_free( line );

    return ret;
}
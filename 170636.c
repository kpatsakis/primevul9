static MenuCacheItem* read_item(GDataInputStream* f, MenuCache* cache,
                                MenuCacheFileDir** all_used_files, int n_all_used_files)
{
    MenuCacheItem* item;
    char *line;
    gsize len;
    gint idx;

    /* desktop/menu id */
    line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
    if(G_UNLIKELY(line == NULL))
        return NULL;

    if( G_LIKELY(len >= 1) )
    {
        if( line[0] == '+' ) /* menu dir */
        {
            item = (MenuCacheItem*)g_slice_new0( MenuCacheDir );
            item->n_ref = 1;
            item->type = MENU_CACHE_TYPE_DIR;
        }
        else if( line[0] == '-' ) /* menu item */
        {
            item = (MenuCacheItem*)g_slice_new0( MenuCacheApp );
            item->n_ref = 1;
            if( G_LIKELY( len > 1 ) ) /* application item */
                item->type = MENU_CACHE_TYPE_APP;
            else /* separator */
            {
                item->type = MENU_CACHE_TYPE_SEP;
                return item;
            }
        }
        else
            return NULL;

        item->id = g_strndup( line + 1, len - 1 );
        g_free(line);
    }
    else
    {
        g_free(line);
        return NULL;
    }

    /* name */
    line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
    if(G_UNLIKELY(line == NULL))
        goto _fail;
    if(G_LIKELY(len > 0))
        item->name = _unescape_lf(line);
    else
        g_free(line);

    /* comment */
    line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
    if(G_UNLIKELY(line == NULL))
        goto _fail;
    if(G_LIKELY(len > 0))
        item->comment = _unescape_lf(line);
    else
        g_free(line);

    /* icon */
    line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
    if(G_UNLIKELY(line == NULL))
        goto _fail;
    if(G_LIKELY(len > 0))
        item->icon = line;
    else
        g_free(line);

    /* file dir/basename */

    /* file name */
    line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
    if(G_UNLIKELY(line == NULL))
        goto _fail;
    if(G_LIKELY(len > 0))
        item->file_name = line;
    else if( item->type == MENU_CACHE_TYPE_APP )
    {
        /* When file name is the same as desktop_id, which is
         * quite common in desktop files, we use this trick to
         * save memory usage. */
        item->file_name = item->id;
        g_free(line);
    }
    else
        g_free(line);

    /* desktop file dir */
    line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
    if(G_UNLIKELY(line == NULL))
    {
_fail:
        g_free(item->id);
        g_free(item->name);
        g_free(item->comment);
        g_free(item->icon);
        if(item->file_name && item->file_name != item->id)
            g_free(item->file_name);
        if(item->type == MENU_CACHE_TYPE_DIR)
            g_slice_free(MenuCacheDir, MENU_CACHE_DIR(item));
        else
            g_slice_free(MenuCacheApp, MENU_CACHE_APP(item));
        return NULL;
    }
    idx = atoi( line );
    g_free(line);
    if( G_LIKELY( idx >=0 && idx < n_all_used_files ) )
    {
        item->file_dir = all_used_files[ idx ];
        g_atomic_int_inc(&item->file_dir->n_ref);
    }

    if( item->type == MENU_CACHE_TYPE_DIR )
        read_dir( f, MENU_CACHE_DIR(item), cache, all_used_files, n_all_used_files );
    else if( item->type == MENU_CACHE_TYPE_APP )
        read_app( f, MENU_CACHE_APP(item), cache );

    return item;
}
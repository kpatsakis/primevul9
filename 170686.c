static void read_dir(GDataInputStream* f, MenuCacheDir* dir, MenuCache* cache,
                     MenuCacheFileDir** all_used_files, int n_all_used_files)
{
    MenuCacheItem* item;
    char *line;
    gsize len;

    /* nodisplay flag */
    if (cache->version >= 2)
    {
        line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
        if (G_UNLIKELY(line == NULL))
            return;
        dir->flags = (guint32)atoi(line);
        g_free(line);
    }

    /* load child items in the dir */
    while( (item = read_item( f, cache, all_used_files, n_all_used_files )) )
    {
        /* menu_cache_ref shouldn't be called here for dir.
         * Otherwise, circular reference will happen. */
        item->parent = dir;
        dir->children = g_slist_prepend( dir->children, item );
    }

    dir->children = g_slist_reverse( dir->children );

    /* set flag by children if working with old cache generator */
    if (cache->version == 1)
    {
        if (dir->children == NULL)
            dir->flags = FLAG_IS_NODISPLAY;
        else if ((line = menu_cache_item_get_file_path(MENU_CACHE_ITEM(dir))) != NULL)
        {
            GKeyFile *kf = g_key_file_new();
            if (g_key_file_load_from_file(kf, line, G_KEY_FILE_NONE, NULL) &&
                g_key_file_get_boolean(kf, G_KEY_FILE_DESKTOP_GROUP,
                                       G_KEY_FILE_DESKTOP_KEY_NO_DISPLAY, NULL))
                dir->flags = FLAG_IS_NODISPLAY;
            g_key_file_free(kf);
            g_free(line);
        }
    }
}
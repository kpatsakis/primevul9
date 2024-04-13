static gint read_all_used_files(GDataInputStream* f, MenuCache* cache,
                                MenuCacheFileDir*** all_used_files)
{
    char *line;
    gsize len;
    int i, n;
    MenuCacheFileDir** dirs;

    line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
    if(G_UNLIKELY(line == NULL))
        return -1;

    n = atoi( line );
    g_free(line);
    if (G_UNLIKELY(n <= 0))
        return n;

    dirs = g_new0( MenuCacheFileDir *, n );

    for( i = 0; i < n; ++i )
    {
        line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
        if(G_UNLIKELY(line == NULL))
        {
            while (i-- > 0)
                menu_cache_file_dir_unref(dirs[i]);
            g_free(dirs);
            return -1;
        }
        dirs[i] = g_new(MenuCacheFileDir, 1);
        dirs[i]->n_ref = 1;
        dirs[i]->dir = line; /* don't include \n */
    }
    *all_used_files = dirs;
    return n;
}
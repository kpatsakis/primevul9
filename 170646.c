static gboolean read_all_known_des(GDataInputStream* f, MenuCache* cache)
{
    char *line;
    gsize len;
    line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
    if(G_UNLIKELY(line == NULL))
        return FALSE;
    cache->known_des = g_strsplit_set( line, ";\n", 0 );
    g_free(line);
    return TRUE;
}
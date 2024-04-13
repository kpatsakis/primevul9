static void read_app(GDataInputStream* f, MenuCacheApp* app, MenuCache* cache)
{
    char *line;
    gsize len;
    GString *str;

    /* generic name */
    line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
    if(G_UNLIKELY(line == NULL))
        return;
    if(G_LIKELY(len > 0))
        app->generic_name = _unescape_lf(line);
    else
        g_free(line);

    /* exec */
    line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
    if(G_UNLIKELY(line == NULL))
        return;
    if(G_LIKELY(len > 0))
        app->exec = _unescape_lf(line);
    else
        g_free(line);

    /* terminal / startup notify */
    line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
    if(G_UNLIKELY(line == NULL))
        return;
    app->flags = (guint32)atoi(line);
    g_free(line);

    /* ShowIn flags */
    line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
    if(G_UNLIKELY(line == NULL))
        return;
    app->show_in_flags = (guint32)atol(line);
    g_free(line);

    if (cache->version < 2)
        return;

    /* TryExec */
    line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
    if (G_UNLIKELY(line == NULL))
        return;
    if (G_LIKELY(len > 0))
        app->try_exec = g_strchomp(line);
    else
        g_free(line);

    /* Path */
    line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
    if (G_UNLIKELY(line == NULL))
        return;
    if (G_LIKELY(len > 0))
        app->working_dir = line;
    else
        g_free(line);

    /* Categories */
    line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
    if (G_UNLIKELY(line == NULL))
        return;
    if (G_LIKELY(len > 0))
    {
        const char **x;

        /* split and intern all the strings so categories can be processed
           later for search doing g_quark_try_string()+g_quark_to_string() */
        app->categories = x = (const char **)g_strsplit(line, ";", 0);
        while (*x != NULL)
        {
            char *cat = (char *)*x;
            *x = g_intern_string(cat);
            g_free(cat);
            x++;
        }
    }
    g_free(line);

    /* Keywords */
    str = g_string_new(MENU_CACHE_ITEM(app)->name);
    if (G_LIKELY(app->exec != NULL))
    {
        char *sp = strchr(app->exec, ' ');
        char *bn = strrchr(app->exec, G_DIR_SEPARATOR);

        g_string_append_c(str, ',');
        if (bn == NULL && sp == NULL)
            g_string_append(str, app->exec);
        else if (bn == NULL || (sp != NULL && sp < bn))
            g_string_append_len(str, app->exec, sp - app->exec);
        else if (sp == NULL)
            g_string_append(str, &bn[1]);
        else
            g_string_append_len(str, &bn[1], sp - &bn[1]);
    }
    if (app->generic_name != NULL)
    {
        g_string_append_c(str, ',');
        g_string_append(str, app->generic_name);
    }
    line = g_data_input_stream_read_line(f, &len, cache->cancellable, NULL);
    if (G_UNLIKELY(line == NULL))
        return;
    if (len > 0)
    {
        g_string_append_c(str, ',');
        g_string_append(str, line);
    }
    app->keywords = g_utf8_casefold(str->str, str->len);
    g_string_free(str, TRUE);
    g_free(line);
}
static GKeyFile *ga_parse_osrelease(const char *fname)
{
    gchar *content = NULL;
    gchar *content2 = NULL;
    GError *err = NULL;
    GKeyFile *keys = g_key_file_new();
    const char *group = "[os-release]\n";

    if (!g_file_get_contents(fname, &content, NULL, &err)) {
        slog("failed to read '%s', error: %s", fname, err->message);
        goto fail;
    }

    if (!g_utf8_validate(content, -1, NULL)) {
        slog("file is not utf-8 encoded: %s", fname);
        goto fail;
    }
    content2 = g_strdup_printf("%s%s", group, content);

    if (!g_key_file_load_from_data(keys, content2, -1, G_KEY_FILE_NONE,
                                   &err)) {
        slog("failed to parse file '%s', error: %s", fname, err->message);
        goto fail;
    }

    g_free(content);
    g_free(content2);
    return keys;

fail:
    g_error_free(err);
    g_free(content);
    g_free(content2);
    g_key_file_free(keys);
    return NULL;
}
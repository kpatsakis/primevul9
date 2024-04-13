static MenuCache* menu_cache_create(const char* menu_name)
{
    MenuCache* cache;
    const gchar * const * langs = g_get_language_names();
    const char* xdg_cfg_env = g_getenv("XDG_CONFIG_DIRS");
    const char* xdg_prefix_env = g_getenv("XDG_MENU_PREFIX");
    const char* xdg_data_env = g_getenv("XDG_DATA_DIRS");
    const char* xdg_cfg_home_env = g_getenv("XDG_CONFIG_HOME");
    const char* xdg_data_home_env = g_getenv("XDG_DATA_HOME");
    const char* xdg_cache_home_env = g_getenv("XDG_CACHE_HOME");
    char *xdg_cfg, *xdg_prefix, *xdg_data, *xdg_cfg_home, *xdg_data_home, *xdg_cache_home;
    char* buf;
    const char* md5;
    char* file_name;
    int len = 0;
    GChecksum *sum;
    char *langs_list;

    xdg_cfg = _validate_env(xdg_cfg_env);
    xdg_prefix = _validate_env(xdg_prefix_env);
    xdg_data = _validate_env(xdg_data_env);
    xdg_cfg_home = _validate_env(xdg_cfg_home_env);
    xdg_data_home = _validate_env(xdg_data_home_env);
    xdg_cache_home = _validate_env(xdg_cache_home_env);

    /* reconstruct languages list in form as it should be in $LANGUAGES */
    langs_list = g_strjoinv(":", (char **)langs);
    for (buf = langs_list; *buf; buf++) /* reusing buf var as char pointer */
        if (*buf == '\n' || *buf == '\t')
            *buf = ' ';

    buf = g_strdup_printf( "REG:%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t" CACHE_VERSION
                           "\t00000000000000000000000000000000\n",
                            menu_name,
                            langs_list,
                            xdg_cache_home,
                            xdg_cfg,
                            xdg_prefix,
                            xdg_data,
                            xdg_cfg_home,
                            xdg_data_home );

    /* calculate the md5 sum of menu name + lang + all environment variables */
    sum = g_checksum_new(G_CHECKSUM_MD5);
    len = strlen(buf);
    g_checksum_update(sum, (guchar*)buf + 4, len - 38);
    md5 = g_checksum_get_string(sum);
    file_name = g_build_filename( g_get_user_cache_dir(), "menus", md5, NULL );
    DEBUG("cache file_name = %s", file_name);
    cache = menu_cache_new( file_name );
    cache->reg = buf;
    cache->md5 = buf + len - 33;
    memcpy( cache->md5, md5, 32 );
    cache->menu_name = g_strdup(menu_name);
    g_free( file_name );
    g_free(langs_list);
    g_free(xdg_cfg);
    g_free(xdg_prefix);
    g_free(xdg_data);
    g_free(xdg_cfg_home);
    g_free(xdg_data_home);
    g_free(xdg_cache_home);

    g_checksum_free(sum); /* md5 is also freed here */

    MENU_CACHE_LOCK;
    g_hash_table_insert( hash, g_strdup(menu_name), cache );
    MENU_CACHE_UNLOCK;

    return cache;
}
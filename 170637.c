static gboolean regenerate_cache( const char* menu_name,
                                  const char* lang_name,
                                  const char* cache_file,
                                  char** env,
                                  int* n_used_files,
                                  char*** used_files )
{
    FILE* f;
    int n_files, status = 0;
    char** files;
    const char* argv[] = {
        MENUCACHE_LIBEXECDIR "/menu-cache-gen",
        "-l", NULL,
        "-i", NULL,
        "-o", NULL,
        NULL};
    argv[2] = lang_name;
    argv[4] = menu_name;
    argv[6] = cache_file;

    /* DEBUG("cmd: %s", g_strjoinv(" ", argv)); */

    /* run menu-cache-gen */
    /* FIXME: is cast to (char**) valid here? */
    if( !g_spawn_sync(NULL, (char **)argv, NULL, 0,
                    pre_exec, env, NULL, NULL, &status, NULL ))
    {
        DEBUG("error executing menu-cache-gen");
    }
    /* DEBUG("exit status of menu-cache-gen: %d", status); */
    if( status != 0 )
        return FALSE;

    f = fopen( cache_file, "r" );
    if( f )
    {
        if( !read_all_used_files( f, &n_files, &files ) )
        {
            n_files = 0;
            files = NULL;
            /* DEBUG("error: read_all_used_files"); */
        }
        fclose(f);
    }
    else
        return FALSE;
    *n_used_files = n_files;
    *used_files = files;
    return TRUE;
}
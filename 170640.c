static gboolean read_all_used_files( FILE* f, int* n_files, char*** used_files )
{
    char line[ 4096 ];
    int i, n, x;
    char** files;
    int ver_maj, ver_min;

    /* the first line of the cache file is version number */
    if( !fgets(line, G_N_ELEMENTS(line),f) )
        return FALSE;
    if( sscanf(line, "%d.%d", &ver_maj, &ver_min)< 2 )
        return FALSE;
    if (ver_maj != VER_MAJOR ||
        ver_min > VER_MINOR || ver_min < VER_MINOR_SUPPORTED)
        return FALSE;

    /* skip the second line containing menu name */
    if( ! fgets( line, G_N_ELEMENTS(line), f ) )
        return FALSE;

    /* num of files used */
    if( ! fgets( line, G_N_ELEMENTS(line), f ) )
        return FALSE;

    n = atoi( line );
    files = g_new0( char*, n + 1 );

    for( i = 0, x = 0; i < n; ++i )
    {
        int len;

        if( ! fgets( line, G_N_ELEMENTS(line), f ) )
            return FALSE;

        len = strlen( line );
        if( len <= 1 )
            return FALSE;
        files[ x ] = g_strndup( line, len - 1 ); /* don't include \n */
        if (g_file_test(files[x]+1, G_FILE_TEST_EXISTS))
            x++;
        else
        {
            DEBUG("ignoring not existent file from menu-cache-gen: %s", files[x]);
            g_free(files[x]);
            files[x] = NULL;
        }
    }
    *n_files = x;
    *used_files = files;
    return TRUE;
}
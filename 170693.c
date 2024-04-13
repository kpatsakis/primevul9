static gboolean cache_file_is_updated( const char* cache_file, int* n_used_files, char*** used_files )
{
    gboolean ret = FALSE;
    struct stat st;
#if 0
    time_t cache_mtime;
    char** files;
    int n, i;
#endif
    FILE* f;

    f = fopen( cache_file, "r" );
    if( f )
    {
        if( fstat( fileno(f), &st) == 0 )
        {
#if 0
            cache_mtime = st.st_mtime;
            if( read_all_used_files(f, &n, &files) )
            {
                for( i =0; i < n; ++i )
                {
                    /* files[i][0] is 'D' or 'F' indicating file type. */
                    if( stat( files[i] + 1, &st ) == -1 )
                        continue;
                    if( st.st_mtime > cache_mtime )
                        break;
                }
                if( i >= n )
                {
                    ret = TRUE;
                    *n_used_files = n;
                    *used_files = files;
                }
            }
#else
            ret = read_all_used_files(f, n_used_files, used_files);
#endif
        }
        fclose( f );
    }
    return ret;
}
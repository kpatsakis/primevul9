BOOL INSTAPI SQLCreateDataSourceW( HWND hwndParent, LPCWSTR lpszDSN )
{
    BOOL ret;
    char *ms = _multi_string_alloc_and_copy( lpszDSN );

    inst_logClear();

    ret = SQLCreateDataSource( hwndParent, ms );

    free( ms );

    return ret;
}
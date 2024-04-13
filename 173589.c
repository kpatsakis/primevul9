BOOL SQLCreateDataSource( HWND hWnd, LPCSTR pszDS )
{
    HODBCINSTWND  hODBCInstWnd = (HODBCINSTWND)hWnd;
    char          szName[FILENAME_MAX];
    char          szNameAndExtension[FILENAME_MAX];
    char          szPathAndName[FILENAME_MAX];
    void *        hDLL;
    BOOL          (*pSQLCreateDataSource)(HWND, LPCSTR);

    inst_logClear();

    /* ODBC specification states that hWnd is mandatory. */
    if ( !hWnd )
    {
        inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_INVALID_HWND, "" );
        return FALSE;
    }

    /* initialize libtool */
    if ( lt_dlinit() )
    {
        inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_GENERAL_ERR, "lt_dlinit() failed" );
        return FALSE;
    }

    /* get plugin name */
    _appendUIPluginExtension( szNameAndExtension, _getUIPluginName( szName, hODBCInstWnd->szUI ) );

    /* lets try loading the plugin using an implicit path */
    hDLL = lt_dlopen( szNameAndExtension );
    if ( hDLL )
    {
        /* change the name, as it avoids it finding it in the calling lib */
        pSQLCreateDataSource = (BOOL (*)(HWND, LPCSTR))lt_dlsym( hDLL, "ODBCCreateDataSource" );
        if ( pSQLCreateDataSource )
            return pSQLCreateDataSource( ( *(hODBCInstWnd->szUI) ? hODBCInstWnd->hWnd : NULL ), pszDS );
        else
            inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_GENERAL_ERR, (char*)lt_dlerror() );
    }
    else
    {
        /* try with explicit path */
        _prependUIPluginPath( szPathAndName, szNameAndExtension );
        hDLL = lt_dlopen( szPathAndName );
        if ( hDLL )
        {
            /* change the name, as it avoids linker finding it in the calling lib */
            pSQLCreateDataSource = (BOOL (*)(HWND,LPCSTR))lt_dlsym( hDLL, "ODBCCreateDataSource" );
            if ( pSQLCreateDataSource )
                return pSQLCreateDataSource( ( *(hODBCInstWnd->szUI) ? hODBCInstWnd->hWnd : NULL ), pszDS );
            else
                inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_GENERAL_ERR, (char*)lt_dlerror() );
        }
    }

    /* report failure to caller */
    inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_GENERAL_ERR, "" );

    return FALSE;
}
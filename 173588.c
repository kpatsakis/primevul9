static int OpenDatabase( SQLHENV *phEnv, SQLHDBC *phDbc, char *szDSN, char *szUID, char *szPWD )
{
    SQLCHAR dsn[ 1024 ], uid[ 1024 ], pwd[ 1024 ];
    SQLTCHAR cstr[ 1024 ];
    char zcstr[ 1024 ], tmp[ 1024 ];
    int i;
    size_t zclen;

    if ( SQLAllocEnv( phEnv ) != SQL_SUCCESS )
    {
        fprintf( stderr, "[ISQL]ERROR: Could not SQLAllocEnv\n" );
        return 0;
    }

    if ( SQLAllocConnect( *phEnv, phDbc ) != SQL_SUCCESS )
    {
        if ( bVerbose ) DumpODBCLog( hEnv, 0, 0 );
        fprintf( stderr, "[ISQL]ERROR: Could not SQLAllocConnect\n" );
        SQLFreeEnv( *phEnv );
        return 0;
    }

    if ( szDSN )
    {
        size_t DSNlen=strlen( szDSN );
        for ( i = 0; i < DSNlen; i ++ )
        {
            dsn[ i ] = szDSN[ i ];
        }
        dsn[ i ] = '\0';
    }
    else
    {
        dsn[ 0 ] = '\0';
    }

    if ( szUID )
    {
        size_t UIDlen=strlen( szUID );
        for ( i = 0; i < UIDlen; i ++ )
        {
            uid[ i ] = szUID[ i ];
        }
        uid[ i ] = '\0';
    }
    else
    {
        uid[ 0 ] = '\0';
    }

    if ( szPWD )
    {
        size_t PWDlen=strlen( szPWD );
        for ( i = 0; i < PWDlen; i ++ )
        {
            pwd[ i ] = szPWD[ i ];
        }
        pwd[ i ] = '\0';
    }
    else
    {
        pwd[ 0 ] = '\0';
    }

    sprintf( zcstr, "DSN=%s", dsn );
    if ( szUID )
    {
        sprintf( tmp, ";UID=%s", uid );
        strcat( zcstr, tmp );
    }
    if ( szPWD )
    {
        sprintf( tmp, ";PWD=%s", pwd );
        strcat( zcstr, tmp );
    }

    zclen=strlen( zcstr );
    for ( i = 0; i < zclen; i ++ )
    {
        cstr[ i ] = zcstr[ i ];
    }
    cstr[ i ] = 0;

    if ( !SQL_SUCCEEDED( SQLDriverConnect( *phDbc, NULL, cstr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT  )))
    {
        if ( bVerbose ) DumpODBCLog( hEnv, hDbc, 0 );
        fprintf( stderr, "[ISQL]ERROR: Could not SQLDriverConnect\n" );
        SQLFreeConnect( *phDbc );
        SQLFreeEnv( *phEnv );
        return 0;
    }
    if ( bVerbose ) DumpODBCLog( hEnv, hDbc, 0 );

    return 1;
}
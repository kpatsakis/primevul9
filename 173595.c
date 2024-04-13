static int DumpODBCLog( SQLHENV hEnv, SQLHDBC hDbc, SQLHSTMT hStmt )
{
    SQLTCHAR        szError[501];
    SQLTCHAR        szSqlState[10];
    SQLINTEGER  nNativeError;
    SQLSMALLINT nErrorMsg;

    if ( hStmt )
    {
        while ( SQLError( hEnv, hDbc, hStmt, szSqlState, &nNativeError, szError, 500, &nErrorMsg ) == SQL_SUCCESS )
        {
            printf( "%s\n", uc_to_ascii( szError ));
        }
    }

    if ( hDbc )
    {
        while ( SQLError( hEnv, hDbc, 0, szSqlState, &nNativeError, szError, 500, &nErrorMsg ) == SQL_SUCCESS )
        {
            printf( "%s\n", uc_to_ascii( szError ));
        }
    }

    if ( hEnv )
    {
        while ( SQLError( hEnv, 0, 0, szSqlState, &nNativeError, szError, 500, &nErrorMsg ) == SQL_SUCCESS )
        {
            printf( "%s\n", uc_to_ascii( szError ));
        }
    }

    return 1;
}
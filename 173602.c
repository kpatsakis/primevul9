static void WriteBodyDelimited( SQLHSTMT hStmt, char cDelimiter )
{
    SQLINTEGER      nCol                            = 0;
    SQLSMALLINT     nColumns                        = 0;
    SQLLEN          nIndicator                      = 0;
    SQLTCHAR            szColumnValue[MAX_DATA_WIDTH+1];
    SQLRETURN       nReturn                         = 0;
    SQLRETURN       ret;

    szColumnValue[ 0 ]  = 0;

    if ( SQLNumResultCols( hStmt, &nColumns ) != SQL_SUCCESS )
        nColumns = -1;

    /* ROWS */
    while (( ret = SQLFetch( hStmt )) == SQL_SUCCESS )
    {
        /* COLS */
        for ( nCol = 1; nCol <= nColumns; nCol++ )
        {
            nReturn = SQLGetData( hStmt, nCol, SQL_C_WCHAR, (SQLPOINTER)szColumnValue, sizeof(szColumnValue), &nIndicator );
            if ( nReturn == SQL_SUCCESS && nIndicator != SQL_NULL_DATA )
            {
                uc_to_ascii( szColumnValue );
                fputs((char*) szColumnValue, stdout );
                if ( nCol < nColumns )
                    putchar( cDelimiter );
            }
            else if ( nReturn == SQL_ERROR )
            {
                ret = SQL_ERROR;
                break;
            }
            else
            {
                if ( nCol < nColumns )
                    putchar( cDelimiter );
            }
        }
        if (ret != SQL_SUCCESS)
            break;
        printf( "\n" );
    }
    if ( ret == SQL_ERROR )
    {
        if ( bVerbose ) DumpODBCLog( 0, 0, hStmt );
    }
}
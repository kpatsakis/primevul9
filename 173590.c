static void WriteBodyHTMLTable( SQLHSTMT hStmt )
{
    SQLINTEGER      nCol                            = 0;
    SQLSMALLINT     nColumns                        = 0;
    SQLLEN          nIndicator                      = 0;
    SQLTCHAR        szColumnValue[MAX_DATA_WIDTH+1];
    SQLRETURN       nReturn                         = 0;
    SQLRETURN       ret;

    szColumnValue[ 0 ]  = 0;

    if ( SQLNumResultCols( hStmt, &nColumns ) != SQL_SUCCESS )
        nColumns = -1;

    while ( (ret = SQLFetch( hStmt )) == SQL_SUCCESS ) /* ROWS */
    {
        printf( "<tr>\n" );

        for ( nCol = 1; nCol <= nColumns; nCol++ ) /* COLS */
        {
            printf( "<td>\n" );
            printf( "<font face=Arial,Helvetica>\n" );

            nReturn = SQLGetData( hStmt, nCol, SQL_C_WCHAR, (SQLPOINTER)szColumnValue, sizeof(szColumnValue), &nIndicator );
            if ( nReturn == SQL_SUCCESS && nIndicator != SQL_NULL_DATA )
            {
                uc_to_ascii( szColumnValue );
                fputs((char*) szColumnValue, stdout );
            }
            else if ( nReturn == SQL_ERROR )
            {
                ret = SQL_ERROR;
                break;
            }
            else
                printf( "%s\n", "" );

            printf( "</font>\n" );
            printf( "</td>\n" );
        }
        if (ret != SQL_SUCCESS)
            break;
        printf( "</tr>\n" );
    }
}
static SQLLEN WriteBodyNormal( SQLHSTMT hStmt )
{
    SQLINTEGER      nCol                            = 0;
    SQLSMALLINT     nColumns                        = 0;
    SQLLEN          nIndicator                      = 0;
    SQLTCHAR        szColumn[MAX_DATA_WIDTH+20];
    SQLTCHAR        szColumnValue[MAX_DATA_WIDTH+1];
    SQLTCHAR        szColumnName[MAX_DATA_WIDTH+1]; 
    SQLULEN         nMaxLength                      = 10;
    SQLRETURN       nReturn                         = 0;
    SQLRETURN       ret;
    SQLLEN          nRows                           = 0;

    szColumn[ 0 ]       = 0;
    szColumnValue[ 0 ]  = 0;
    szColumnName[ 0 ]   = 0;    

    if ( SQLNumResultCols( hStmt, &nColumns ) != SQL_SUCCESS )
        nColumns = -1;

    /* ROWS */
    while (( ret = SQLFetch( hStmt )) == SQL_SUCCESS )
    {
        /* COLS */
        for ( nCol = 1; nCol <= nColumns; nCol++ )
        {
            SQLColAttribute( hStmt, nCol, SQL_DESC_LABEL, szColumnName, sizeof(szColumnName), NULL, NULL );
            SQLColAttribute( hStmt, nCol, SQL_DESC_DISPLAY_SIZE, NULL, 0, NULL, (SQLLEN*)&nMaxLength );

            uc_to_ascii( szColumnName );

            if ( nMaxLength > MAX_DATA_WIDTH ) nMaxLength = MAX_DATA_WIDTH;
            nReturn = SQLGetData( hStmt, nCol, SQL_C_WCHAR, (SQLPOINTER)szColumnValue, sizeof(szColumnValue), &nIndicator );
            szColumnValue[MAX_DATA_WIDTH] = '\0';
            uc_to_ascii( szColumnValue );

            if ( nReturn == SQL_SUCCESS && nIndicator != SQL_NULL_DATA )
            {
                if ( strlen((char*)szColumnValue) < max( nMaxLength, strlen((char*)szColumnName )))
                {
                    int i;
                    size_t maxlen=max( nMaxLength, strlen((char*)szColumnName ));
                    strcpy((char*) szColumn, "| " );
                    strcat((char*) szColumn, (char*) szColumnValue );

                    for ( i = strlen((char*) szColumnValue ); i < maxlen; i ++ )
                    {
                        strcat((char*) szColumn, " " );
                    }
                }
                else
                {
                    strcpy((char*) szColumn, "| " );
                    strcat((char*) szColumn, (char*) szColumnValue );
                }
            }
            else if ( nReturn == SQL_ERROR )
            {
                ret = SQL_ERROR;
                break;
            }
            else
            {
                sprintf((char*)  szColumn, "| %-*s", (int)max( nMaxLength, strlen((char*) szColumnName) ), "" );
            }
            fputs((char*)  szColumn, stdout );
        }
        if (ret != SQL_SUCCESS)
            break;
        printf( "|\n" );
        nRows++;
    } 
    if ( ret == SQL_ERROR )
    {
        if ( bVerbose ) DumpODBCLog( 0, 0, hStmt );
    }

    return nRows;
}
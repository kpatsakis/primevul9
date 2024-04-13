static void WriteHeaderDelimited( SQLHSTMT hStmt, char cDelimiter )
{
    SQLINTEGER      nCol                            = 0;
    SQLSMALLINT     nColumns                        = 0;
    SQLTCHAR            szColumnName[MAX_DATA_WIDTH+1]; 

    szColumnName[ 0 ]   = 0;    

    if ( SQLNumResultCols( hStmt, &nColumns ) != SQL_SUCCESS )
        nColumns = -1;

    for ( nCol = 1; nCol <= nColumns; nCol++ )
    {
        SQLColAttribute( hStmt, nCol, SQL_DESC_LABEL, szColumnName, sizeof(szColumnName), NULL, NULL );
        fputs((char*) uc_to_ascii( szColumnName ), stdout );
        if ( nCol < nColumns )
            putchar( cDelimiter );
    }
    putchar( '\n' );
}
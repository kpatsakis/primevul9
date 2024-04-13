void UWriteHeaderNormal( SQLHSTMT hStmt, SQLTCHAR *szSepLine )
{
    SQLINTEGER      nCol                            = 0;
    SQLSMALLINT     nColumns                        = 0;
    SQLULEN         nMaxLength                      = 10;
    SQLTCHAR            szColumn[MAX_DATA_WIDTH+20];    
    SQLTCHAR            szColumnName[MAX_DATA_WIDTH+1]; 
    SQLTCHAR            szHdrLine[32001];   

    szColumn[ 0 ]       = 0;    
    szColumnName[ 0 ]   = 0;    
    szHdrLine[ 0 ]      = 0;    

    if ( SQLNumResultCols( hStmt, &nColumns ) != SQL_SUCCESS )
        nColumns = -1;

    for ( nCol = 1; nCol <= nColumns; nCol++ )
    {
        SQLColAttribute( hStmt, nCol, SQL_DESC_DISPLAY_SIZE, NULL, 0, NULL, (SQLLEN*)&nMaxLength );
        SQLColAttribute( hStmt, nCol, SQL_DESC_LABEL, szColumnName, sizeof(szColumnName), NULL, NULL );
        if ( nMaxLength > MAX_DATA_WIDTH ) nMaxLength = MAX_DATA_WIDTH;

        uc_to_ascii( szColumnName );

        /* SEP */
        memset( szColumn, '\0', sizeof(szColumn) );
        memset( szColumn, '-', max( nMaxLength, strlen((char*)szColumnName) ) + 1 );
        strcat((char*) szSepLine, "+" );
        strcat((char*) szSepLine,(char*) szColumn );

        /* HDR */
        sprintf((char*) szColumn, "| %-*s", (int)max( nMaxLength, strlen((char*)szColumnName) ), (char*)szColumnName );
        strcat((char*) szHdrLine,(char*) szColumn );
    }
    strcat((char*) szSepLine, "+\n" );
    strcat((char*) szHdrLine, "|\n" );

    puts((char*) szSepLine );
    puts((char*) szHdrLine );
    puts((char*) szSepLine );
}
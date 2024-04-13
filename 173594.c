static int ExecuteHelp( SQLHDBC hDbc, char *szSQL, char cDelimiter, int bColumnNames, int bHTMLTable )
{
    char            szTable[250]                        = "";
    SQLHSTMT        hStmt;
    SQLTCHAR        szSepLine[32001];   
    SQLLEN          nRows               = 0;

    szSepLine[ 0 ] = 0;

    /****************************
     * EXECUTE SQL
     ***************************/
    if ( SQLAllocStmt( hDbc, &hStmt ) != SQL_SUCCESS )
    {
        if ( bVerbose ) DumpODBCLog( hEnv, hDbc, 0 );
        fprintf( stderr, "[ISQL]ERROR: Could not SQLAllocStmt\n" );
        return 0;
    }

    if ( iniElement( szSQL, ' ', '\0', 1, szTable, sizeof(szTable) ) == INI_SUCCESS )
    {
        SQLWCHAR tname[ 1024 ];

        ansi_to_unicode( szTable, tname );
        /* COLUMNS */
        if ( SQLColumns( hStmt, NULL, 0, NULL, 0, tname, SQL_NTS, NULL, 0 ) != SQL_SUCCESS )
        {
            if ( bVerbose ) DumpODBCLog( hEnv, hDbc, hStmt );
            fprintf( stderr, "[ISQL]ERROR: Could not SQLColumns\n" );
            SQLFreeStmt( hStmt, SQL_DROP );
            return 0;
        }
    }
    else
    {
        /* TABLES */
        if ( SQLTables( hStmt, NULL, 0, NULL, 0, NULL, 0, NULL, 0 ) != SQL_SUCCESS )
        {
            if ( bVerbose ) DumpODBCLog( hEnv, hDbc, hStmt );
            fprintf( stderr, "[ISQL]ERROR: Could not SQLTables\n" );
            SQLFreeStmt( hStmt, SQL_DROP );
            return 0;
        }
    }

    /****************************
     * WRITE HEADER
     ***************************/
    if ( bHTMLTable )
        WriteHeaderHTMLTable( hStmt );
    else if ( cDelimiter == 0 )
        UWriteHeaderNormal( hStmt, szSepLine );
    else if ( cDelimiter && bColumnNames )
        WriteHeaderDelimited( hStmt, cDelimiter );

    /****************************
     * WRITE BODY
     ***************************/
    if ( bHTMLTable )
        WriteBodyHTMLTable( hStmt );
    else if ( cDelimiter == 0 )
        nRows = WriteBodyNormal( hStmt );
    else
        WriteBodyDelimited( hStmt, cDelimiter );

    /****************************
     * WRITE FOOTER
     ***************************/
    if ( bHTMLTable )
        WriteFooterHTMLTable( hStmt );
    else if ( cDelimiter == 0 )
        UWriteFooterNormal( hStmt, szSepLine, nRows );

    /****************************
     * CLEANUP
     ***************************/
    SQLFreeStmt( hStmt, SQL_DROP );

    return 1;
}
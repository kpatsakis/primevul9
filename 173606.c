static void WriteHeaderHTMLTable( SQLHSTMT hStmt )
{
    SQLINTEGER      nCol                            = 0;
    SQLSMALLINT     nColumns                        = 0;
    SQLTCHAR        szColumnName[MAX_DATA_WIDTH+1]; 

    szColumnName[ 0 ] = 0;

    printf( "<table BORDER>\n" );
    printf( "<tr BGCOLOR=#000099>\n" );

    if ( SQLNumResultCols( hStmt, &nColumns ) != SQL_SUCCESS )
        nColumns = -1;

    for ( nCol = 1; nCol <= nColumns; nCol++ )
    {
        SQLColAttribute( hStmt, nCol, SQL_DESC_LABEL, szColumnName, sizeof(szColumnName), NULL, NULL );

        printf( "<td>\n" );
        printf( "<font face=Arial,Helvetica><font color=#FFFFFF>\n" );
        printf( "%s\n", uc_to_ascii( szColumnName ));
        printf( "</font></font>\n" );
        printf( "</td>\n" );
    }
    printf( "</tr>\n" );
}
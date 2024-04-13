void UWriteFooterNormal( SQLHSTMT hStmt, SQLTCHAR   *szSepLine, SQLLEN nRows )
{
    SQLLEN  nRowsAffected   = -1;

    puts( (char*)szSepLine );

    SQLRowCount( hStmt, &nRowsAffected );
    printf( "SQLRowCount returns %ld\n", nRowsAffected );

    if ( nRows )
    {
        printf( "%ld rows fetched\n", nRows );
    }
}
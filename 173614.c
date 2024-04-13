int CloseDatabase( SQLHENV hEnv, SQLHDBC hDbc )
{
    SQLDisconnect( hDbc );
    SQLFreeConnect( hDbc );
    SQLFreeEnv( hEnv );

    return 1;
}
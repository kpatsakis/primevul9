static void ansi_to_unicode( char *szSQL, SQLWCHAR *szUcSQL )
{
    int i;

    for ( i = 0; szSQL[ i ]; i ++ )
    {
        szUcSQL[ i ] = szSQL[ i ];
    }
    szUcSQL[ i ] = 0;
}
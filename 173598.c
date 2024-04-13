static char * uc_to_ascii( SQLWCHAR *uc )
{
    char *ascii = (char *)uc;
    int i;

    for ( i = 0; uc[ i ]; i ++ )
    {
        ascii[ i ] = uc[ i ] & 0x00ff;
    }

    ascii[ i ] = 0;

    return ascii;
}
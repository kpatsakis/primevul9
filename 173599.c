void _single_copy_to_wide( SQLWCHAR *out, LPCSTR in, int len )
{
    while ( len >= 0 )
    {
        *out = *in;
        out++;
        in++;
        len --;
    }
}
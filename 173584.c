void _single_copy_from_wide( SQLCHAR *out, LPCWSTR in, int len )
{
    while ( len >= 0 )
    {
        *out = *in;
        out++;
        in++;
        len --;
    }
}
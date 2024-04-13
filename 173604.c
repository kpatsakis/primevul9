void _single_string_copy_to_wide( SQLWCHAR *out, LPCSTR in, int len )
{
    while ( len > 0 && *in )
    {
        *out = *in;
        out++;
        in++;
        len --;
    }
    *out = 0;
}
void _multi_string_copy_to_wide( SQLWCHAR *out, LPCSTR in, int len )
{
    while ( len > 0 && ( in[ 0 ] || in[ 1 ] ))
    {
        *out = *in;
        out++;
        in++;
        len --;
    }
    *out++ = 0;
    *out++ = 0;
}
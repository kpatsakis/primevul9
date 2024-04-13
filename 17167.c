hexhex2nr(char_u *p)
{
    if (!vim_isxdigit(p[0]) || !vim_isxdigit(p[1]))
	return -1;
    return (hex2nr(p[0]) << 4) + hex2nr(p[1]);
}
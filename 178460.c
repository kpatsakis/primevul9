re_multi_type(int c)
{
    if (c == Magic('@') || c == Magic('=') || c == Magic('?'))
	return MULTI_ONE;
    if (c == Magic('*') || c == Magic('+') || c == Magic('{'))
	return MULTI_MULT;
    return NOT_MULTI;
}
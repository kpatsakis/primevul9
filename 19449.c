get_mouse_class(char_u *p)
{
    int		c;

    if (has_mbyte && MB_BYTE2LEN(p[0]) > 1)
	return mb_get_class(p);

    c = *p;
    if (c == ' ' || c == '\t')
	return 0;

    if (vim_iswordc(c))
	return 2;

    // There are a few special cases where we want certain combinations of
    // characters to be considered as a single word.  These are things like
    // "->", "/ *", "*=", "+=", "&=", "<=", ">=", "!=" etc.  Otherwise, each
    // character is in its own class.
    if (c != NUL && vim_strchr((char_u *)"-+*/%<>&|^!=", c) != NULL)
	return 1;
    return c;
}
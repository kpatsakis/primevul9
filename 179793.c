cin_iselse(
    char_u  *p)
{
    if (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    return (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
}
tgoto(char *cm, int x, int y)
{
    static char buf[30];
    char *p, *s, *e;

    if (!cm)
	return "OOPS";
    e = buf + 29;
    for (s = buf; s < e && *cm; cm++)
    {
	if (*cm != '%')
	{
	    *s++ = *cm;
	    continue;
	}
	switch (*++cm)
	{
	case 'd':
	    p = (char *)tltoa((unsigned long)y);
	    y = x;
	    while (*p)
		*s++ = *p++;
	    break;
	case 'i':
	    x++;
	    y++;
	    break;
	case '+':
	    *s++ = (char)(*++cm + y);
	    y = x;
	    break;
	case '%':
	    *s++ = *cm;
	    break;
	default:
	    return "OOPS";
	}
    }
    *s = '\0';
    return buf;
}
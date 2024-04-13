check_stl_option(char_u *s)
{
    int		itemcnt = 0;
    int		groupdepth = 0;
    static char_u   errbuf[80];

    while (*s && itemcnt < STL_MAX_ITEM)
    {
	/* Check for valid keys after % sequences */
	while (*s && *s != '%')
	    s++;
	if (!*s)
	    break;
	s++;
	if (*s != '%' && *s != ')')
	    ++itemcnt;
	if (*s == '%' || *s == STL_TRUNCMARK || *s == STL_MIDDLEMARK)
	{
	    s++;
	    continue;
	}
	if (*s == ')')
	{
	    s++;
	    if (--groupdepth < 0)
		break;
	    continue;
	}
	if (*s == '-')
	    s++;
	while (VIM_ISDIGIT(*s))
	    s++;
	if (*s == STL_USER_HL)
	    continue;
	if (*s == '.')
	{
	    s++;
	    while (*s && VIM_ISDIGIT(*s))
		s++;
	}
	if (*s == '(')
	{
	    groupdepth++;
	    continue;
	}
	if (vim_strchr(STL_ALL, *s) == NULL)
	{
	    return illegal_char(errbuf, *s);
	}
	if (*s == '{')
	{
	    s++;
	    while (*s != '}' && *s)
		s++;
	    if (*s != '}')
		return (char_u *)N_("E540: Unclosed expression sequence");
	}
    }
    if (itemcnt >= STL_MAX_ITEM)
	return (char_u *)N_("E541: too many items");
    if (groupdepth != 0)
	return (char_u *)N_("E542: unbalanced groups");
    return NULL;
}
cleanup_help_tags(int num_file, char_u **file)
{
    int		i, j;
    int		len;
    char_u	buf[4];
    char_u	*p = buf;

    if (p_hlg[0] != NUL && (p_hlg[0] != 'e' || p_hlg[1] != 'n'))
    {
	*p++ = '@';
	*p++ = p_hlg[0];
	*p++ = p_hlg[1];
    }
    *p = NUL;

    for (i = 0; i < num_file; ++i)
    {
	len = (int)STRLEN(file[i]) - 3;
	if (len <= 0)
	    continue;
	if (STRCMP(file[i] + len, "@en") == 0)
	{
	    // Sorting on priority means the same item in another language may
	    // be anywhere.  Search all items for a match up to the "@en".
	    for (j = 0; j < num_file; ++j)
		if (j != i && (int)STRLEN(file[j]) == len + 3
			   && STRNCMP(file[i], file[j], len + 1) == 0)
		    break;
	    if (j == num_file)
		// item only exists with @en, remove it
		file[i][len] = NUL;
	}
    }

    if (*buf != NUL)
	for (i = 0; i < num_file; ++i)
	{
	    len = (int)STRLEN(file[i]) - 3;
	    if (len <= 0)
		continue;
	    if (STRCMP(file[i] + len, buf) == 0)
	    {
		// remove the default language
		file[i][len] = NUL;
	    }
	}
}
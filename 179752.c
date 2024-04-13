cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    int	    found = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    if (cinsd_buf == NULL)
	return FALSE;

    for (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (int)cinsd_len, ",");
	if (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    if (*skip == ':' && skip[1] != ':')
	    {
		found = TRUE;
		break;
	    }
	}
    }

    vim_free(cinsd_buf);
    return found;
}
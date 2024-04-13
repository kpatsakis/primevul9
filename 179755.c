cin_is_cinword(char_u *line)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    int		cinw_len;
    int		retval = FALSE;
    int		len;

    cinw_len = (int)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    if (cinw_buf != NULL)
    {
	line = skipwhite(line);
	for (cinw = curbuf->b_p_cinw; *cinw; )
	{
	    len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	    if (STRNCMP(line, cinw_buf, len) == 0
		    && (!vim_iswordc(line[len]) || !vim_iswordc(line[len - 1])))
	    {
		retval = TRUE;
		break;
	    }
	}
	vim_free(cinw_buf);
    }
    return retval;
}
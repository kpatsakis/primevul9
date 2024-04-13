has_format_option(int x)
{
    if (p_paste)
	return FALSE;
    return (vim_strchr(curbuf->b_p_fo, x) != NULL);
}